#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <memory>
#include <atomic>
#include <set>
#include <queue>

using namespace std;
using namespace std::chrono;

const int SIZE = 1000'0000;
const int MAX_THREAD = 8;

class NODE
{
public:
	int key{ 0 };
	NODE* next{ nullptr };

	NODE() = default;
	~NODE() = default;
	NODE(int key_value) : key{ key_value } {}
};

struct StampPointer
{
	//NODE* ptr;
	//int stamp;
	// 이건 안됨, STAMP와 ptr 둘이 캐시 boundary 에 걸쳐져서 중간값이 생길 수 있음. 
};

uint64_t MakeStamp(NODE* p, uint32_t stamp)
{
	uint64_t t{ stamp };
	t <<= 32;
	t |= reinterpret_cast<uint32_t>(p);
	return t;
}
NODE* GetAddress(uint64_t sPointer) {
	uint32_t t{ static_cast<uint32_t>(sPointer) };
	return reinterpret_cast<NODE*>(t);
}
uint32_t GetStamp(uint64_t p)
{
	return static_cast<uint32_t>((p >> 32));
}
std::pair<NODE*, uint32_t> GetInformation(uint64_t p)
{
	return { GetAddress(p), GetStamp(p) };
}


bool CAS(uint64_t* addr, NODE* oldNode, NODE* newNode, uint32_t oldStamp, uint32_t newStamp)
{
	uint64_t oldV{ MakeStamp(oldNode, oldStamp) };
	uint64_t newV{ MakeStamp(newNode, newStamp) };

	return atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_uint64_t*>(addr), &oldV, newV);
}

bool CAS(NODE*& addr, NODE* expected, NODE* desired)
{
	uint32_t expect{ reinterpret_cast<uint32_t>(expected) };
	return atomic_compare_exchange_strong(reinterpret_cast<atomic_uint32_t*>(&addr), &expect, reinterpret_cast<uint32_t>(desired));
}

class SPLFQUEUE
{
public:
	SPLFQUEUE()
	{
		NODE* p{ new NODE{ 0 } };
		head = tail = MakeStamp(p, 0);
	}
	~SPLFQUEUE()
	{
		Init();
	}

	void Init()
	{
		NODE* ptr{ nullptr };
		while (GetAddress(head)->next) {
			ptr = GetAddress(head)->next;
			GetAddress(head)->next = GetAddress(head)->next->next;
			delete ptr;
		}
		tail = head;
	}

	void Enqueue(int key)
	{
		NODE* ptr{ new NODE{ key } };
		uint64_t last{};
		NODE* next{ nullptr };
		while (true) {
			last = tail;
			next = GetAddress(last)->next;

			if (last != tail) continue;

			uint32_t lastStamp{ GetStamp(last) };


			if (nullptr == next) {
				if (CAS(GetAddress(last)->next, nullptr, ptr)) {
					CAS(&tail, GetAddress(last), ptr,
						lastStamp, lastStamp + 1);
					return;
				}
			}
			else 
				CAS(&tail, GetAddress(last), next, lastStamp, lastStamp + 1);
		}
	}

	int Dequeue()
	{
		uint64_t first{};
		NODE* next{ nullptr };
		uint64_t last{};
		int value{};
		while (true) {
			first = head;
			next = GetAddress(first)->next;
			last = tail;
			
			if (first != head) continue;
			if (next == nullptr) return -1;

			uint32_t firstStamp{ GetStamp(head) };
			uint32_t lastStamp{ GetStamp(tail) };

			if (first == last) {
				CAS(&tail, GetAddress(last), next, lastStamp, lastStamp + 1);
				continue;
			}

			value = next->key;

			if (!CAS(&head, GetAddress(first), next, firstStamp, firstStamp + 1)) continue;
			delete GetAddress(first);
			return value;
		}
	}

	void print(int count)
	{
		NODE* curr{ GetAddress(head)->next };
		for (int i = 0; i < count; ++i) {
			if (curr == nullptr) break;
			cout << curr->key << ", ";
			curr = curr->next;
		}
		cout << endl;
	}

private:
	uint64_t head, tail;
};

SPLFQUEUE olist;

void ThreadFunc(int numThread)
{
	for (int i = 0; i < SIZE / numThread; ++i) {
		switch (rand() % 2) {
		case 0:
			olist.Enqueue(i);
			break;
		case 1:
			olist.Dequeue();
			break;
		default:
			cout << "ERROR";
			exit(-1);
		}
	}
}

int main()
{
	while (1) {
		for (int i = 1; i <= MAX_THREAD; i *= 2) {
			olist.Init();
			vector<thread> v{};
			auto start{ high_resolution_clock::now() };

			for (int j = 0; j < i; ++j)
				v.emplace_back(ThreadFunc, i);

			for (int j = 0; j < i; ++j)
				v[j].join();

			cout << i << " thread : " << duration_cast<milliseconds>(high_resolution_clock::now() - start).count() << " ms " << endl;
			olist.print(20);

		}
	}
}



//#include <iostream>
//#include <random>
//#include <algorithm>
//
//using namespace std;
//
//class SafeIntArray
//{
//public:
//	SafeIntArray(size_t n) : arraySize{ n }, buf{ new int[arraySize] {0, } } { }
//
//	int& operator[](size_t n) 
//	{
//		return buf[n];
//	}
//	size_t size() const
//	{
//		return arraySize;
//	}
//
//	int* begin() const {
//		return buf;
//	}
//
//	int* end() const {
//		return buf + size();
//	}
//private:
//	size_t arraySize;
//	int* buf;
//};
//
//int main()
//{
//	SafeIntArray a{ 10 };	// 10개의 정수를 저장할 공간, 모든 값은 0으로
//	
//	for (int i = 0; i < 10; ++i)
//		a[i] = i;
//
//	for (auto& it : a)
//		cout << it << endl;
//}