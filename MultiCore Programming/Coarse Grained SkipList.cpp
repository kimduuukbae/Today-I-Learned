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

const int KEY_RANGE = 1000;
const int SIZE = 400'0000;
const int MAX_THREAD = 8;
const int MAX_LEVEL = 8;	// 전체 list 갯수에 log 를 취한것

struct SKNODE
{
	int key;
	SKNODE* next[MAX_LEVEL + 1];
	int topLevel;
	SKNODE() : topLevel {MAX_LEVEL}
	{
		for (auto&& n : next)
			n = nullptr;
	}
	SKNODE(int x, int top) : key{ x }, topLevel{ top }
	{
		for (int i = 0; i <= top; ++i)
			next[i] = nullptr;
	}
};

class SKLIST
{
public:
	SKLIST()
	{
		head.key = 0x80000000;
		tail.key = 0x7FFFFFFF;

		for (auto&& n : head.next) 
			n = &tail;
		head.topLevel = tail.topLevel = MAX_LEVEL;
	}

	~SKLIST()
	{
		Clear();
	}

	void Init()
	{
		Clear();
		head.key = 0x80000000;
		tail.key = 0x7FFFFFFF;

		for (auto&& n : head.next)
			n = &tail;
		head.topLevel = tail.topLevel = MAX_LEVEL;
	}

	void Clear()
	{
		SKNODE* p{ head.next[0] };
		while (p != &tail) {
			SKNODE* toDelete{ p };
			p = p->next[0];
			delete toDelete;
		}
		for (auto&& n : head.next)
			n = &tail;
	}
	
	void Find(int x, SKNODE* preds[], SKNODE* currs[])
	{
		preds[MAX_LEVEL] = &head;
		for (int cl = MAX_LEVEL; cl >= 0; --cl) {
			if(cl != MAX_LEVEL)
				preds[cl] = preds[cl + 1];
			currs[cl] = preds[cl]->next[cl];
			while (currs[cl]->key < x) {
				preds[cl] = currs[cl];
				currs[cl] = currs[cl]->next[cl];
			}
		}
	}

	bool Add(int x)
	{
		SKNODE* preds[MAX_LEVEL + 1];
		SKNODE* currs[MAX_LEVEL + 1];
		
		skMtx.lock();
		Find(x, preds, currs);

		if (currs[0]->key == x) {
			skMtx.unlock();
			return false;
		}
		
		//int topLevel{ rand() % (MAX_LEVEL) };
		int topLevel{ 0 };
		while (rand() % 2 == 1 && topLevel < MAX_LEVEL)
			++topLevel;

		SKNODE* elem{ new SKNODE{x, topLevel} };
		for (int i = 0; i <= topLevel; ++i) {
			preds[i]->next[i] = elem;
			elem->next[i] = currs[i];
		}
		skMtx.unlock();
		return true;
	}

	bool Remove(int x)
	{
		SKNODE* preds[MAX_LEVEL + 1];
		SKNODE* currs[MAX_LEVEL + 1];

		skMtx.lock();
		Find(x, preds, currs);

		if (currs[0]->key != x) {
			skMtx.unlock();
			return false;
		}

		int topLevel{ currs[0]->topLevel };

		for (int i = 0; i <= topLevel; ++i) 
			preds[i]->next[i] = currs[i]->next[i];

		delete currs[0];

		skMtx.unlock();
		return true;
	}

	bool Contains(int x)
	{
		SKNODE* preds[MAX_LEVEL + 1];
		SKNODE* currs[MAX_LEVEL + 1];

		skMtx.lock();
		Find(x, preds, currs);

		if (currs[0]->key == x) {
			skMtx.unlock();
			return true;
		}
		skMtx.unlock();
		return false;
	}

	void Print()
	{
		SKNODE* values = head.next[0];
		for (int i = 0; i < 20; ++i) {
			cout << values->key << ' ';
			if (values == &tail)
				break;
			values = values->next[0];
		}
		cout << endl;
	}

private:
	SKNODE head, tail;
	std::mutex skMtx;
};

SKLIST olist;


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
		olist.Print();
	}

}