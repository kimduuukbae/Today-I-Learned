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
		auto initial_suspend() noexcept { return std::experimental::suspend_always(); }
		auto final_suspend() noexcept { return std::experimental::suspend_always(); }
		void return_void() {}
		void unhandled_exception() { std::terminate(); }
	};
	resumable(coHandle handle) : resumableHandle(handle) {}
	resumable(resumable&) = delete;
	resumable(resumable&&) = delete;

	bool resume() {
		if (!resumableHandle.done())
			resumableHandle.resume();
		return !resumableHandle.done();
	}
	~resumable() { resumableHandle.destroy(); }

private:
	coHandle resumableHandle;
};

resumable func() {
	std::cout << "Hello" << std::endl;
	co_await std::experimental::suspend_always();
	std::cout << "Coroutine" << std::endl;
}

int main() {
	resumable res{ func() };
	while (res.resume());
}
