#include <condition_variable>
#include <memory>
#include <mutex>
#include <optional>
#include <print>
#include <string>
#include <thread>

using namespace std;

template <typename T>
class SharedState
{
    optional<T> value;

    mutex mtx;
    condition_variable cv;

public:
    void set_value(T t)
    {
        {
            lock_guard guard {mtx};
            if (value.has_value()) { return; }
            value.emplace(std::move(t));
        }
        cv.notify_all();
    }

    T get()
    {
        unique_lock lock {mtx};
        cv.wait(lock, [this] { return value.has_value(); });
        return std::move(value).value();
    }
};

template <typename T>
class Future
{
    shared_ptr<SharedState<T>> p;

public:
    explicit Future(shared_ptr<SharedState<T>> p_) : p {std::move(p_)} {}

    T get() { return p->get(); }
};

template <typename T>
class Promise
{
    shared_ptr<SharedState<T>> p {make_shared<SharedState<T>>()};

public:
    Future<T> get_future() { return Future<T> {p}; }

    void set_value(T t) { p->set_value(std::move(t)); }
};

int main()
{
    Promise<string> prom;
    jthread t1 {[&prom]()
                {
                    this_thread::sleep_for(1s);
                    prom.set_value("hello!"s);
                }};
    jthread t2 {[&prom]() { println("{}", prom.get_future().get()); }};
}
