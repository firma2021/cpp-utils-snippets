#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <system_error>
#include <threads.h>
#include <tuple>
#include <utility>
using namespace std;

class Thread
{
    optional<thrd_t> hdl;

    template <typename... TupleTypes>
    static int thread_func(void* arg)
    {
        using TupleType = tuple<TupleTypes...>;
        unique_ptr<TupleType> ptr {static_cast<TupleType*>(arg)};

        std::apply([](auto&&... args) { std::invoke(std::forward<decltype(args)>(args)...); },
                   std::move(*ptr));

        return 0; // threads.h要求返回int
    }

public:
    Thread() = default;

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    Thread(Thread&& another) noexcept : hdl {exchange(another.hdl, nullopt)} {}
    Thread& operator=(Thread&& another) noexcept
    {
        if (joinable()) { terminate(); }
        hdl = exchange(another.hdl, nullopt);
        return *this;
    }

    ~Thread()
    {
        if (joinable()) { terminate(); }
    }

    void swap(Thread& another) noexcept { hdl.swap(another.hdl); }

    template <typename F, typename... Args>
    explicit Thread(F&& func, Args&&... args)
    {
        using TupleType = tuple<decay_t<F>, decay_t<Args>...>;
        auto ptr = make_unique<TupleType>(std::forward<F>(func), std::forward<Args>(args)...);

        thrd_t handle {};
        int err = thrd_create(&handle, &thread_func<decay_t<F>, decay_t<Args>...>, ptr.get());
        if (err == thrd_nomem || err == thrd_error)
        {
            throw system_error {make_error_code(errc::resource_unavailable_try_again)};
        }

        ptr.release(); // noexcept below.

        hdl = handle;
    }

    [[nodiscard]]
    bool joinable() const noexcept
    {
        return hdl.has_value();
    }

    void join()
    {
        if (!joinable()) { throw system_error {make_error_code(errc::invalid_argument)}; }

        int err = thrd_join(*hdl, nullptr);
        if (err == thrd_error) { throw system_error {make_error_code(errc::no_such_process)}; }

        hdl = nullopt;
    }

    void detach()
    {
        if (!joinable()) { throw system_error {make_error_code(errc::invalid_argument)}; }

        int err = thrd_detach(*hdl);
        if (err == thrd_error) { throw system_error {make_error_code(errc::no_such_process)}; }

        hdl = nullopt;
    }
};

namespace std
{
template <>
void swap<Thread>(Thread& a, Thread& b) noexcept
{
    a.swap(b);
}
} // namespace std

// 所有标准库容器和智能指针的移动构造函数都不是 explicit。
// 移动构造通常发生在明确的上下文中，不容易产生意外的隐式转换。
// 如果标记为 explicit，会阻止编译器进行一些重要的优化
class Object
{
public:
    Object() { cout << "Construct at " << this << "\n"; };
    ~Object() { cout << "Destruct at " << this << "\n"; };

    explicit Object(const Object&) { cout << "Const Copy at " << this << "\n"; };
    Object(Object&&) noexcept { cout << "Move at " << this << "\n"; };

    Object& operator=(const Object&)
    {
        cout << "Const Copy Assignment at " << this << "\n";
        return *this;
    };
    Object& operator=(Object&&) noexcept
    {
        cout << "Move Assignment at " << this << "\n";
        return *this;
    };
};

void foo(Object obj)
{
    cout << "Hello, my new thread!\n";
}

int main()
{
    try
    {
        Thread t {foo, Object {}};
        t.join();
        cout << "Check joinable: " << boolalpha << t.joinable() << "\n";
    }
    catch (const exception& e)
    {
        cerr << e.what() << "\n";
    }
}

// explicit移动构造函数：
// Construct at 0x7b26ede00030
// Move at 0x7b46ef1e0010
// Destruct at 0x7b26ede00030
// Const Copy at 0x7b26ecffed00
// Hello, my new thread!
// Destruct at 0x7b26ecffed00
// Destruct at 0x7b46ef1e0010
// Check joinable: false

// 非explicit移动构造函数：
// Construct at 0x7b3c49e00030
// Move at 0x7b5c4b1e0010
// Destruct at 0x7b3c49e00030
// Move at 0x7b3c48ffed00
// Hello, my new thread!
// Destruct at 0x7b3c48ffed00
// Destruct at 0x7b5c4b1e0010
// Check joinable: false
