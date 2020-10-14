#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <memory>

using namespace std;
using namespace std::chrono;

class NODE
{
public:
	int key;
	shared_ptr<NODE> next;
	bool isRemoved;
	mutex mtx;
	NODE() { isRemoved = false; }
	NODE(int key_value)
	{
		key = key_value;
		isRemoved = false;
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


class OLIST {
	shared_ptr<NODE> head, tail;
public:
	OLIST()
	{
		head = make_shared<NODE>(0x80000000);
		tail = make_shared<NODE>(0x7FFFFFFF);

	}
	~OLIST() {}
	void Init()
	{
		head->next = tail;
	}

	bool validate(shared_ptr<NODE>& pred, shared_ptr<NODE>& cur)
	{
		return !pred->isRemoved &&
			!cur->isRemoved &&
			pred->next == cur;
	}

	bool Add(int key)
	{
		while (true) {
			shared_ptr<NODE> pred{ head };
			shared_ptr<NODE> curr{ pred->next };

			while (curr->key < key) {
				pred = curr;
				curr = pred->next;
			}

			pred->lock();
			curr->lock();

			if (validate(pred, curr)) {
				if (key == curr->key) {
					pred->unlock();
					curr->unlock();
					return false;
				}
				else {
					shared_ptr<NODE> node{ make_shared<NODE>(key) };
					node->next = curr;
					pred->next = node;
					pred->unlock();
					curr->unlock();
					return true;
				}
			}
			pred->unlock();
			curr->unlock();
		}
	}
	bool Remove(int key)
	{
		while (true) {
			shared_ptr<NODE> pred{ head }, curr{ pred->next };

			while (curr->key < key) {
				pred = curr;
				curr = pred->next;
			}

			pred->lock();
			curr->lock();

			if (validate(pred, curr)) {
				if (key == curr->key) {
					pred->next->isRemoved = true;
					pred->next = curr->next;
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
			pred->unlock();
			curr->unlock();
		}
	}
	bool Contains(int key)
	{
		shared_ptr<NODE> cur{ head };
		while (cur->key < key)
			cur = cur->next;
		return !cur->isRemoved && cur->key == key;
	}
	void PrintFirst(int count)
	{
		shared_ptr<NODE> cur{ head->next };

		for (int i = 0; i < count; ++i) {
			if (tail == cur)
				break;
			cout << cur->key << ", ";
			cur = cur->next;
		}
		cout << endl;
	}
};


OLIST olist;

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