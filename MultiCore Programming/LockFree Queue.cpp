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
	NODE(int key_value) : key { key_value }{}
};

bool CAS(NODE* volatile& mem, NODE* expected, NODE* desired)
{
	uint64_t expect{ reinterpret_cast<uint64_t>(expected) };
	return atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_uint64_t*>(&mem), &expect, reinterpret_cast<uint64_t>(desired));
}

class CQUEUE
{
public:
	CQUEUE()
	{
		head = tail = new NODE{};
	}
	~CQUEUE()
	{
		Init();
		delete head;
	}

	void Init()
	{
		NODE* ptr{ nullptr };
		while (head != tail) {
			ptr = head;
			head = ptr->next;
			delete ptr;
		}
	}

	void Enqueue(int key)
	{
		NODE* ptr{ new NODE{ key } };
		while (true) {
			NODE* volatile last{ tail };
			NODE* volatile next{ last->next };
			if (last != tail) continue;
			if (nullptr == next) {
				if (CAS(last->next, nullptr, ptr)) {
					CAS(tail, last, ptr);
					return;
				}
			}
			else
				CAS(tail, last, next);
		}
	}

	int Dequeue()
	{
		while (true) {
			NODE* volatile first{ head };
			NODE* volatile last{ tail };
			NODE* volatile next{ first->next };
			
			if (first != head) continue;
			if (next == nullptr) return -1;
			if (first == last) {
				CAS(tail, last, next);
				continue;
			}

			int value = next->key;

			if (!CAS(head, first, next)) continue;
			delete first;
			return value;
		}
	}

	void print(int count)
	{
		NODE* curr{ head->next };
		for (int i = 0; i < count; ++i) {
			if (curr == nullptr) break;
			cout << curr->key << ", ";
			curr = curr->next;
		}
		cout << endl;
	}

private:
	NODE* volatile head, * volatile tail;
};

CQUEUE olist;

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
	for (int k = 0; k < 100; ++k) {
		for (int i = 1; i <= MAX_THREAD; i *= 2) {
			olist.Init();
			vector<thread> v{};
			auto start{ steady_clock::now() };

			for (int j = 0; j < i; ++j)
				v.emplace_back(ThreadFunc, i);

			for (int j = 0; j < i; ++j)
				v[j].join();

			cout << i << " thread : " << duration_cast<milliseconds>(steady_clock::now() - start).count() << " ms " << endl;
			olist.print(20);
		}
	}
}