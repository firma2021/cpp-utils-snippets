#include <functional>
#include <print>
#include <memory>
using namespace std;

template <typename T>
class Function
{
    static_assert(false);
};

template <typename Ret, typename... Args>
class Function<Ret(Args...)>
{

    struct FuncBase 
    {
        virtual Ret call(Args... args) = 0;
        virtual ~FuncBase() = default;
    };

    template <typename T>
    struct FuncImpl: FuncBase 
    {
    private:
        T t;
        
    public:
        template <typename... CArgs>
        explicit FuncImpl(CArgs&&... cargs) : t{std::forward<CArgs>(cargs)...} {}

        Ret call(Args... args) override 
        {
            return std::invoke(t, std::forward<Args>(args)...);
        }

    };

    unique_ptr<FuncBase> ctrl; 

public:

    template <typename Func>
    Function(Func&& f)
    : ctrl{make_unique<FuncImpl<std::decay_t<Func>>>(std::forward<Func>(f))}
    {}

    Ret operator()(Args... args) const 
    {
        if (!ctrl) [[unlikely]] { throw std::bad_function_call(); }
        return ctrl->call(std::forward<Args>(args)...);
    }
};

int main()
{
    Function<void(int, double)> f {[](int a, double b){ println("a = {}, b = {}", a, b);}};
    f(1, 3.14);
}
