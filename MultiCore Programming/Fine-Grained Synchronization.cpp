#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;
using namespace std::chrono;

class NODE
{
public:
	int key;
	NODE* next;
	mutex mtx;
	NODE() { next = nullptr; }
	NODE(int key_value) {
		next = nullptr;
		key = key_value;
	}
	~NODE() {}
	void lock()
	{
		mtx.lock();
	}

	void unlock()
	{
		mtx.unlock();
	}
};

class CLIST {
	NODE head, tail;
public:
	CLIST()
	{
		head.key = 0x80000000;
		tail.key = 0x7FFFFFFF;
		head.next = &tail;
	}
	~CLIST() {}
	void Init()
	{
		NODE* ptr;
		while (head.next != &tail) {
			ptr = head.next;
			head.next = head.next->next;
			delete ptr;
		}
	}
	bool Add(int key)
	{
		NODE* pred, *curr;
		head.lock();
		pred = &head;

		curr = pred->next;
		curr->lock();

		while (curr->key < key) {
			pred->lock();
			pred = curr;
			curr = curr->next;
			curr->lock();
		}

		if (key == curr->key) {
			pred->unlock();
			curr->unlock();
			return false;
		}
		
		else { // 현재 pred와 curr는 lock 상태
			NODE* node = new NODE(key);
			node->next = curr;
			pred->next = node;
			pred->unlock();
			curr->unlock();
			return true;
		}
	}
	bool Remove(int key)
	{
		NODE* pred, *curr;

		head.lock();
		pred = &head;
		curr = pred->next;
		curr->lock();

		while (curr->key < key) {
			pred->unlock();
			pred = curr;
			curr = curr->next;
			curr->lock();
		}

		if (key == curr->key) {
			pred->next = curr->next;
			pred->unlock();
			curr->unlock();
			delete curr;
			return true;
		}

		else {
			pred->unlock();
			curr->unlock();
			return false;
		}
	}
	bool Contains(int key)
	{
		NODE* pred, *curr;

		head.lock();
		pred = &head;
		curr = pred->next;
		curr->lock();

		while (curr->key < key) {
			pred->unlock();
			pred = curr;
			curr = curr->next;
			curr->lock();
		}
		if (key == curr->key) {
			pred->unlock();
			curr->unlock();
			return true;
		}
		else {
			pred->unlock();
			curr->unlock();
			return false;
		}
	}
	void PrintFirst(int count)
	{
		NODE* cur{ head.next };

		for (int i = 0; i < count; ++i) {
			if (&tail == cur) 
				break;
			cout << cur->key << ", ";
			cur = cur->next;
		}
		cout << endl;
	}
};


CLIST clist;

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
			clist.Add(key);
			break;
		case 1:
			key = rand() % KEY_RANGE;
			clist.Remove(key);
			break;
		case 2:
			key = rand() % KEY_RANGE;
			clist.Contains(key);
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
		clist.Init();
		vector<thread> v{};
		auto start{ steady_clock::now() };
	
		for (int j = 0; j < i; ++j)
			v.emplace_back(ThreadFunc, i);

		for (int j = 0; j < i; ++j)
			v[j].join();

		cout << i << " thread : " <<  duration_cast<milliseconds>(steady_clock::now() - start).count() << " ms "<< endl;
		clist.PrintFirst(20);
	}
}
