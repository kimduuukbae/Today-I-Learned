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

const int KEY_RANGE = 1000;
const int SIZE = 4000;
const int MAX_THREAD = 8;
thread_local int threadID;

struct UNODE;

enum class E_METHOD_TYPE
{
	ADD,
	REMOVE,
	CONTAIN,
	DISPLAY,
	CLEAR
};

struct Invocation
{
	E_METHOD_TYPE type;
	int value;
};

struct Response
{
	bool ret;
};

class SequenceObject
{
public:
	Response apply(const Invocation& invoc)
	{
		switch (invoc.type)
		{
		case E_METHOD_TYPE::ADD:
			if (m_set.count(invoc.value) != 0)
				return { false };
			m_set.insert(invoc.value);
			return { true };
			break;
		case E_METHOD_TYPE::REMOVE:
			if (m_set.count(invoc.value) == 0)
				return { false };
			m_set.erase(invoc.value);
			return { true };
			break;
		case E_METHOD_TYPE::CONTAIN:
			return { m_set.count(invoc.value) != 0 };
			break;
		case E_METHOD_TYPE::DISPLAY: {
			int c{ 0 };
			for (auto num : m_set) {
				if (c++ > invoc.value) break;
				cout << num << ", ";
			}
			cout << endl;
		}
			break;
		case E_METHOD_TYPE::CLEAR:
			m_set.clear();
			break;
		}
		return { false };
	}

private:
	set<int> m_set;
};

class Consensus{
	std::atomic_uint64_t cnext{ 0 };
public:
	UNODE* decide(UNODE* next)
	{
		uint64_t ov{};
		if (atomic_compare_exchange_strong(&cnext, &ov, reinterpret_cast<uint64_t>(next)))
			return next;
		return reinterpret_cast<UNODE*>(cnext.load());
	}
	Consensus& operator=(const Consensus& other)
	{
		cnext = other.cnext.load();
		return *this;
	}
};

struct UNODE {
	volatile int seq{ 0 };
	UNODE* next{ nullptr };
	Invocation invoc;
	Consensus decideNext;
	UNODE(const Invocation& invoc){ this->invoc = invoc; }
};

class LFUniversalSet 
{
public:
	void Init()
	{
		UNODE* pred{ tail->next };
		while (pred) {
			UNODE* backup{ pred };
			pred = pred->next;
			delete backup;
		}
		for (auto& h : head)
			h = tail;
		tail->next = nullptr;
		tail->decideNext = Consensus();
	}

	LFUniversalSet()
	{
		tail = new UNODE{ Invocation{} };
		tail->seq = 1;
		for (auto& h : head)
			h = tail;
	}
	UNODE* GetLastNode()
	{
		UNODE* p{ head[0] };
		for (auto h : head) {
			if (h->seq > p->seq) p = h;
		}
		return p;
	}
	Response apply(const Invocation& inv)
	{
		UNODE* prefer{ new UNODE{inv} };

		while (prefer->seq == 0) {
			UNODE* before{ GetLastNode() };
			UNODE* after{ before->decideNext.decide(prefer) };
			before->next = after;
			after->seq = before->seq + 1;
			head[threadID] = after;
		}
		SequenceObject soSet;
		UNODE* exec{ tail->next };
		while (exec != prefer) {
			soSet.apply(exec->invoc);
			exec = exec->next;
		}
		return soSet.apply(inv);
	}
private:
	UNODE* tail;
	UNODE* head[MAX_THREAD]{ nullptr, };
};

class LFUN_SET {
	LFUniversalSet m_set;
public:
	LFUN_SET() = default;
	~LFUN_SET() = default;

	void Init()
	{
		m_set.Init();
	}

	bool Add(int key)
	{
		return m_set.apply({ E_METHOD_TYPE::ADD, key }).ret;
	}
	bool Remove(int key)
	{
		return m_set.apply({ E_METHOD_TYPE::REMOVE, key }).ret;
	}
	bool Contains(int key)
	{
		return m_set.apply({ E_METHOD_TYPE::CONTAIN, key }).ret;
	}
	void PrintFirst(int count)
	{
		m_set.apply({ E_METHOD_TYPE::DISPLAY, count });
	}
};


LFUN_SET olist;

void ThreadFunc(int numThread, int id)
{
	int key;

	threadID = id;

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
			v.emplace_back(ThreadFunc, i, j);

		for (int j = 0; j < i; ++j)
			v[j].join();

		cout << i << " thread : " << duration_cast<milliseconds>(steady_clock::now() - start).count() << " ms " << endl;
		olist.PrintFirst(20);
	}
}