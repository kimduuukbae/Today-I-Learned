#include <iostream>
#include <chrono>
#include <mutex>
#include <atomic>
#include <vector>

using namespace std;
using namespace chrono;

constexpr int NUM_TEST = 1000'0000;
constexpr int MAX_THREAD = 8;

thread_local int thread_id;
int num_thread;

class NODE {
public:
    int key;
    NODE* next;

    NODE() { key = 0; next = nullptr; }
    NODE(int key_value) {
        next = nullptr;
        key = key_value;
    }
    ~NODE() {}
};

class BackOff 
{
    int minDelay, maxDelay;
    int limit;
public:
    void Init(int min, int max)
    {
        minDelay = limit = min;
        maxDelay = max;
    }
    void InterruptedException() {
        int delay = 0;
        if (limit != 0) delay = rand() % limit;
        limit *= 2;
        if (limit > maxDelay) limit = maxDelay;
        this_thread::sleep_for(chrono::microseconds(delay));;
    }

    void backoff2() {
        int delay = 0;
        if (limit != 0)
            delay = rand() % limit;
        limit *= 2;
        if (limit > maxDelay)
            limit = maxDelay;
        int start, current;
        _asm RDTSC;
        _asm mov start, eax;
        do {
            _asm RDTSC;
            _asm mov current, eax;
        } while (current - start < delay);
    }

    void backoff3() {
        int delay = 0;
        if (0 != limit) delay = rand() % limit;
        if (0 == delay) return;
        limit += limit;
        if (limit > maxDelay) limit = maxDelay;
        _asm mov eax, delay;
    myloop:
        _asm dec eax
        _asm jnz myloop;
    }
};


class LFSTACK {
    NODE* volatile top;
    BackOff bo;
public:
    LFSTACK() : top(nullptr) 
    {
        bo.Init(1, 10000);
    }
    ~LFSTACK() {
        clear();
    }

    void clear()
    {
        bo.Init(1, 10000);
        NODE* ptr{};
        while (top != nullptr) {
            ptr = top;
            top = top->next;
            delete ptr;
        }
    }

    bool CAS(NODE* volatile* top, NODE* old_top, NODE* new_top) {
        return atomic_compare_exchange_strong(reinterpret_cast<atomic_uint volatile*>(top),
            reinterpret_cast<uint32_t*>(&old_top), reinterpret_cast<uint32_t>(new_top));
    }

    void Push(int key) {
        NODE* e = new NODE(key);
        NODE* last{};
        while (true) {
            last = top;
            e->next = last;
            if (CAS(&top, last, e)) return;
            bo.backoff3();
        }
    }

    int Pop() { // CAS 사이에 다른 오퍼레이션(top 읽기 등)이 있으므로 텀이 있다. 그래서 큐보다는 성능 낫다.
        NODE* last{};
        int resVal{};
        while (true) {
            last = top;
            if (last == nullptr) {
                return -1; // 오류
            }
            resVal = last->key;
            if (CAS(&top, last, last->next)) {   // last->next 재활용되면 다른 포인터 하나 둬야함.
               //delete last;
                return resVal;
            }
            bo.backoff3();
        }
    }

    void print_first_20()
    {
        NODE* curr = top;
        for (int i = 0; i < 20; ++i) {
            if (nullptr == curr) break;
            cout << curr->key << ", ";
            curr = curr->next;
        }
        cout << endl;
    }
};

LFSTACK myStack;

void Worker(int t_id)
{
    const int loop = NUM_TEST / num_thread;
    thread_id = t_id;
    for (int i = 0; i < loop; i++) {
        if ((rand() % 2) == 0 || i < 1000 / num_thread)
            myStack.Push(i);
        else myStack.Pop();
    }
}

int main()
{
    for (int num = 1; num <= MAX_THREAD; num = num * 2) {
        // 벤치 마크 프로그램을 실행하자. 
        num_thread = num;
        myStack.clear();
        vector<thread> threads{};
        system_clock::time_point start{ system_clock::now() };

        for (int i = 0; i < num; ++i) threads.emplace_back(Worker, num);
        for (auto& th : threads) th.join();

        duration<double> elapsedTime = system_clock::now() - start;

        myStack.print_first_20();

        cout << "NUM OF THREADS = " << num;
        cout << ",   Exec Time = " << duration_cast<milliseconds>(elapsedTime).count() << "ms. " << endl;
    }
}