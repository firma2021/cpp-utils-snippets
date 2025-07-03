// 在计算 a + b + c时, 先计算 temp = a + b, 再计算 temp + c, 有临时变量产生，性能低。
// 表达式模板解决了这个问题。

#include <chrono>
#include <iostream>
#include <vector>
using namespace std;

struct Vector
{
    vector<double> data;

    explicit Vector(size_t size) : data(size) {}

    Vector operator+(const Vector& rhs) const
    {
        Vector result(data.size());
        for (size_t i = 0; i < data.size(); ++i) { result.data[i] = data[i] + rhs.data[i]; }
        return result;
    }
};

void test_normal()
{
    size_t N = 1'000'000;
    Vector a(N), b(N), c(N), d(N);
    for (size_t i = 0; i < N; ++i)
    {
        b.data[i] = i;
        c.data[i] = i * 2;
        d.data[i] = i * 3;
    }

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < 100; ++i) { a = b + c + d; }

    auto end = chrono::high_resolution_clock::now();
    cout << "Normal: " << chrono::duration<double>(end - start).count() << "s\n";
}

template <typename L, typename R>
struct AddExpr
{
    const L& l;
    const R& r;

    AddExpr(const L& l_, const R& r_) : l(l_), r(r_) {}

    double operator[](size_t i) const { return l[i] + r[i]; }

    size_t size() const { return l.size(); }
};

// 包装 Vector 结构
struct Vector2
{
    vector<double> data;

    Vector2(size_t size) : data(size) {}

    double operator[](size_t i) const { return data[i]; }
    double& operator[](size_t i) { return data[i]; }

    size_t size() const { return data.size(); }

    // no operator+ here, we use expression template instead

    // assign expression template result
    template <typename Expr>
    Vector2& operator=(const Expr& expr)
    {
        for (size_t i = 0; i < size(); ++i) { data[i] = expr[i]; }
        return *this;
    }
};

// 运算符重载：表达式组合
template <typename L, typename R>
AddExpr<L, R> operator+(const L& l, const R& r)
{
    return AddExpr<L, R>(l, r);
}

void test_expr_template()
{
    size_t N = 1'000'000;
    Vector2 a(N), b(N), c(N), d(N);
    for (size_t i = 0; i < N; ++i)
    {
        b[i] = i;
        c[i] = i * 2;
        d[i] = i * 3;
    }

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < 100; ++i) { a = b + c + d; }

    auto end = chrono::high_resolution_clock::now();
    cout << "Expression Template: " << chrono::duration<double>(end - start).count() << "s\n";
}

int main()
{
    test_normal();
    test_expr_template();
}

// Normal: 0.137148s
// Expression Template: 0.00108828s
