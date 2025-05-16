#include <algorithm>
#include <cstring>
#include <iostream>
#include <string_view>
#include <utility>
using namespace std;

class my_string
{
    size_t size {};
    char* str {};

public:
    my_string() = default;

    explicit my_string(const char* s)
        : size {strlen(s)}, str {(size != 0) ? new char[size] : nullptr}
    {
        std::copy(s, s + size, str);
    }

    ~my_string() { delete[] str; }

    my_string(const my_string& other)
        : size {other.size}, str {(size != 0) ? new char[size] : nullptr}
    {
        std::copy(other.str, other.str + size, str);
    }

    // std::exchange tricks
    my_string(my_string&& other) noexcept
        : size {std::exchange(other.size, 0)}, str {std::exchange(other.str, {})}
    {}

    // cony-and swap for copy assignment
    my_string& operator=(my_string other) noexcept
    {
        swap(*this, other);
        return *this;
    }

    friend void swap(my_string& lhs, my_string& rhs) noexcept
    {
        using std::swap;
        swap(lhs.size, rhs.size);
        swap(lhs.str, rhs.str);
    }

    // Use of overloaded operator '=' is ambiguous
    // my_string& operator=(my_string&& other) noexcept
    // {
    //     size = std::exchange(other.size, 0);
    //     str = std::exchange(other.str, {});
    //     return *this;
    // }

    friend std::ostream& operator<<(std::ostream& os, const my_string& s)
    {
        return os << string_view {s.str, s.size};
    }
};

int main()
{
    {
        cout << "copy constructor: " << '\n';
        my_string s1 {"Hello"};
        my_string s2 {s1};
        cout << s1 << '\n';
        cout << s2 << '\n';
    }

    {
        cout << "copy constructor: " << '\n';
        my_string s1 {"Hello"};
        my_string s2 = s1;
        cout << s1 << '\n';
        cout << s2 << '\n';
    }

    {
        cout << "move constructor: " << '\n';
        my_string s1 {"Hello"};
        my_string s2 {std::move(s1)};
        cout << s1 << '\n';
        cout << s2 << '\n';
    }

    {
        cout << "move constructor: " << '\n';
        my_string s1 {"Hello"};
        my_string s2 = std::move(s1);
        cout << s1 << '\n';
        cout << s2 << '\n';
    }

    {
        cout << "copy-and-swap assignment: " << '\n';
        my_string s1 {"Hello"};
        my_string s2 {"World"};
        s2 = s1;
        cout << s1 << '\n';
        cout << s2 << '\n';
    }

    {
        cout << "copy-and-swap assignment: " << '\n';
        my_string s1 {"Hello"};
        my_string s2 {"World"};
        s2 = std::move(s1);
        cout << s1 << '\n';
        cout << s2 << '\n';
    }
}
