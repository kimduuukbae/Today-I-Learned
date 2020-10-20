#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <memory>
#include <atomic>

using namespace std;
using namespace std::chrono;

class LFNODE;

class CPTR
{
public:
	CPTR() {}

	~CPTR() {}

	CPTR(LFNODE* addr, bool marking)
	{
		uint64_t next{ reinterpret_cast<uint64_t>(addr) };
		if (true == marking)
			next = next | 0x01;
		ptr = next;
	}

	LFNODE* GetAddr()
	{
		return reinterpret_cast<LFNODE*>(ptr & masking);
	}
	LFNODE* GetAddr(bool* marking)
	{
		uint64_t next{ ptr };
		*marking = (0x01 == (next & 0x01));
		return reinterpret_cast<LFNODE*>(next & masking);
	}

	bool CAS(LFNODE* oldAddr, LFNODE* newAddr, bool oldMark, bool newMark)
	{
		uint64_t oldNext{ reinterpret_cast<uint64_t>(oldAddr) };
		if (oldMark)
			++oldNext;

		uint64_t newNext{ reinterpret_cast<uint64_t>(newAddr) };
		if (newMark)
			++newNext;
		return atomic_compare_exchange_strong(&ptr, &oldNext, newNext);
	}

	void Store(LFNODE* addr, bool marking)
	{
		uint64_t next{ reinterpret_cast<uint64_t>(addr) };
		if (true == marking)
			next = next | 0x01;
		ptr = next;
	}
private:
	atomic_uint64_t ptr;
	const uint64_t masking{ 0xFF'FF'FF'FF'FF'FF'FF'FE };
};

class LFNODE
{
public:
	int key{};
	CPTR next;

	LFNODE() { }
	LFNODE(int key_value) { key = key_value; }
	~LFNODE() {}
};


class LFLIST {
	LFNODE head, tail;
public:
	LFLIST()
	{
		head.key = 0x80000000;
		tail.key = 0x7FFFFFFF;
		head.next.Store(&tail, false);

	}
	~LFLIST() {}
	void Init()
	{
		LFNODE* ptr{ nullptr };
		while (head.next.GetAddr() != &tail) {
			ptr = head.next.GetAddr();
			head.next.Store(head.next.GetAddr()->next.GetAddr(), false);
			delete ptr;
		}
	}

	void Find(LFNODE*& pred, LFNODE*& curr, int key)
	{
	retry:
		pred = &head;
		curr = pred->next.GetAddr();
		while (true) {
			// marking된 curr 지우기
			bool mark{ false };
			LFNODE* succ{ curr->next.GetAddr(&mark) };

			while (mark) {
				if (pred->next.CAS(curr, succ, false, false)) {
					curr = succ;
					succ = curr->next.GetAddr(&mark);
				}
				else
					goto retry;
			}

			if (curr->key >= key) return;
			pred = curr;
			curr = succ;
		}
	}

	bool Add(int key)
	{
		LFNODE* pred, *curr;
		while (true) {

			Find(pred, curr, key);
			if (key == curr->key)
				return false;
			else {
				LFNODE* node{ new LFNODE(key) };
				node->next.Store(curr, false);
				if (pred->next.CAS(curr, node, false, false))
					return true;
			}
		}
	}
	bool Remove(int key)
	{
		LFNODE* pred, *curr;
		while (true) {
			Find(pred, curr, key);
			if (key != curr->key)
				return false;
			else {
				LFNODE* succ{ curr->next.GetAddr() };
				bool snip{ curr->next.CAS(succ, succ, false, true) };

				if (!snip)
					continue;

				pred->next.CAS(curr, succ, false, false);
				return true;
			}
		}
	}
	bool Contains(int key)
	{
		LFNODE* curr{ &head };
		bool marked{ false };
		while (curr->key < key) {
			curr = curr->next.GetAddr();
			curr->next.GetAddr(&marked);
		}
		return (curr->key == key && !marked);
	}
	void PrintFirst(int count)
	{
		LFNODE* cur{ head.next.GetAddr() };

		for (int i = 0; i < count; ++i) {
			if (&tail == cur)
				break;
			cout << cur->key << ", ";
			cur = cur->next.GetAddr();
		}
		cout << endl;
	}
};


LFLIST olist;

const int KEY_RANGE = 1000;
const int SIZE = 400'0000;
const int MAX_THREAD = 8;

void ThreadFunc(int numThread)
{
	int key;

	for (int i = 0; i < SIZE / numThread; ++i) {
		switch (rand() % 3) {
		case 0:
			key = rand() % KEY_RANGE;
			olist.Add(key);
			break;
		case 1:
			key = rand() % KEY_RANGE;
			olist.Remove(key);
			break;
		case 2:
			key = rand() % KEY_RANGE;
			olist.Contains(key);
			break;
		default:
			cout << "ERROR";
			exit(-1);
		}
	}
}

int main()
{
	for (int i = 1; i <= MAX_THREAD; i *= 2) {
		olist.Init();
		vector<thread> v{};
		auto start{ steady_clock::now() };

		for (int j = 0; j < i; ++j)
			v.emplace_back(ThreadFunc, i);

		for (int j = 0; j < i; ++j)
			v[j].join();

		cout << i << " thread : " << duration_cast<milliseconds>(steady_clock::now() - start).count() << " ms " << endl;
		olist.PrintFirst(20);
	}

}