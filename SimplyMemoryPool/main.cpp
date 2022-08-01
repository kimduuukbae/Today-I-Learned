#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>

using namespace std;

class MemoryPool {
public:
	MemoryPool(size_t poolSizeByte) : __poolSize{ poolSizeByte } {
		__buffer = std::malloc(__poolSize);
	}

	~MemoryPool() {
		if ( __buffer ) {
			std::free(__buffer);

			__poolSize = 0;
			__buffer = nullptr;
		}
	}

	template <typename T>
	T* Allocate() {
		size_t typeSize{ sizeof(T) };

		if ( !__IsPossibleAllocateSize(typeSize) {
			__Defragmentation();

			if ( !__IsPossibleAllocateSize(typeSize) && !__TryResize() ) {
				T* ptr{ new T{} };
				return ptr;
			}
		}

		T* ptr{ reinterpret_cast<T*>(::operator new(sizeof(T), __buffer)) };
		return ptr;
	}

	template <typename T> requires std::is_array_v<T>
	T* Allocate() {

	}

	size_t GetPoolSize() const {
		return __poolSize;
	}

private:
	size_t __poolSize;
	size_t __poolHead{ 0 };

	void* __buffer{ nullptr };

	bool __TryResize() {
		size_t newPoolSize{ static_cast< size_t > ( ( static_cast< double >( __poolSize ) * 1.5 ) ) };
		void* buf{ std::malloc(newPoolSize) };
		
		if ( buf == nullptr ) return false;

		std::memcpy(buf, __buffer, __poolSize);
		std::free(__buffer);

		__buffer = buf;
		__poolSize = newPoolSize;

		return true;
	}

	bool __Defragmentation() {

	}

	bool __IsPossibleAllocateSize(size_t objectSize) {
		return __poolHead + objectSize < __poolSize;
	}
};



int main() {
	MemoryPool pool{10000};

	int* test{ pool.Allocate<int>() };
	*test = std::numeric_limits<int>::max();
	
	cout << *test << endl;
}

