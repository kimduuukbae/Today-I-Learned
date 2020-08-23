#include <iostream>
#include <experimental/coroutine>

class resumable {
public:
	struct promise_type;
	using coHandle = std::experimental::coroutine_handle<promise_type>;
	struct promise_type {
		auto get_return_object() noexcept {
			return coHandle::from_promise(*this);
		}

		const char* yieldString{ nullptr };
		auto yield_value(const char* yield) {
			yieldString = yield;
			return std::experimental::suspend_always();
		}

		auto initial_suspend() noexcept { return std::experimental::suspend_always(); }
		auto final_suspend() noexcept { return std::experimental::suspend_always(); }
		void unhandled_exception() { std::terminate(); }
		void return_value() {}
		static resumable get_return_object_on_allocation_failure() { throw std::bad_alloc(); }
	};
	resumable(coHandle handle) : resumableHandle(handle) {}
	resumable(resumable&) = delete;
	resumable(resumable&&) = delete;

	bool resume() {
		if (!resumableHandle.done())
			resumableHandle.resume();
		return !resumableHandle.done();
	}

	auto yield_val() -> const char* { return resumableHandle.promise().yieldString; }
	
	~resumable() { resumableHandle.destroy(); }

private:
	coHandle resumableHandle;
};

resumable generator() {
	while (true) {
		co_yield "Hello";
		co_yield "Coroutine";
	}
}

int main() {
	resumable res{ generator() };
	for (int i = 0; i < 10; ++i) {
		res.resume();
		std::cout << res.yield_val() << std::endl;
	}
}
