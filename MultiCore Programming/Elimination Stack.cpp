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

constexpr unsigned char ST_EMPTY = 0;
constexpr unsigned char ST_WAITING = 1;
constexpr unsigned char ST_BUSY = 2;
constexpr int EMPTY_VALUE = 0;

class Exchanger 
{
public:
    Exchanger() : slot{ 0 } {}
    unsigned char getState()
    {
        int t{ slot };
        return (t >> 30) & 0x3;
    }
    int getValue()
    {
        int t{ slot };
        return t & 0x7FFFFFFF;
    }
    bool SLOT_CAS(int old_state, int new_state, int old_value, int new_value)
    {
        int oldV{ (old_state << 30) + old_value };
        int newV{ (new_state << 30) + new_value };
        return std::atomic_compare_exchange_strong(&slot, &oldV, newV);
    }
    int exchange(int value, bool* timeOut, bool* busy)
    {
        for(int i = 0 ; i < 1000; ++i) {
            unsigned char state{ getState() };
            switch (state)
            {
            case ST_EMPTY: {
                if (SLOT_CAS(ST_EMPTY, ST_WAITING, 0, value)) {
                    int counter{ 0 };
                    while (getState() != ST_BUSY) {
                        ++counter;
                        if (counter > 1000) {
                            *timeOut = true;
                            return 0;
                        }
                    }
                    int ret = getValue();
                    slot = EMPTY_VALUE;
                    return ret;
                }
                else
                    continue;
            }
            case ST_WAITING: {
                int oldValue{ getValue() };
                if (SLOT_CAS(ST_WAITING, ST_BUSY, oldValue, value))
                    return oldValue;
                else
                    continue;
            }
            case ST_BUSY:
                break;
            }
        }
        *busy = true;
        return 0;
    }

private:
    atomic_int slot;
};

class EliminationArray 
{
    int range;
    Exchanger exchanger[MAX_THREAD];
public:
    EliminationArray() { range = 1; }
    ~EliminationArray() = default;
    void Init()
    {
        range = 1;
    }
    int Visit(int value, bool* timeOut)
    {
        int slot{ rand() % range };
        bool busy{ false };
        int ret{ exchanger[slot].exchange(value, timeOut, &busy) };
        if ((*timeOut == true) && (range > 1)) --range;
        if ((busy == true) && (range <= num_thread / 2)) ++range;
        return ret;
    }
};

class LFELSTACK {
    NODE* volatile top;
    EliminationArray el;
public:
    LFELSTACK() : top(nullptr)
    {
        el.Init();
    }
    ~LFELSTACK() {
        clear();
    }

    void clear()
    {
        el.Init();
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
            bool timeOut{ false };
            int ret{ el.Visit(key, &timeOut) };
            if (timeOut == false) 
                if (ret == 0) return;
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
            bool timeOut{ false };
            int ret{ el.Visit(0, &timeOut) };
            if (timeOut == false)
                if (ret != 0)
                    return ret;
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

LFELSTACK myStack;

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