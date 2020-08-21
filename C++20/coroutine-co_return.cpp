#include <iostream>
#include <experimental/coroutine>
#include <type_traits>

class resumable {
public:
	struct promise_type;
	using coHandle = std::experimental::coroutine_handle<promise_type>;
	struct promise_type {
		auto get_return_object() noexcept {
			return coHandle::from_promise(*this);
		}
		auto initial_suspend() noexcept { return std::experimental::suspend_always(); }
		auto final_suspend() noexcept { return std::experimental::suspend_always(); }
		const char* coString;
		void return_value(const char* string) { coString = string; }
		void unhandled_exception() { std::terminate(); }
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
	const char* return_val() { return resumableHandle.promise().coString; }
	~resumable() { resumableHandle.destroy(); }

private:
	coHandle resumableHandle;
};

resumable func() {
	std::cout << "Hello" << std::endl;
	co_await std::experimental::suspend_always();
	co_return "Coroutine";
}

int main() {
	resumable foo{ func() };
	while (foo.resume());
	std::cout << foo.return_val() << std::endl;
}
