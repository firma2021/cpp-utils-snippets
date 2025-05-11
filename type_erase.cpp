#include <print>
#include <memory>
#include <vector>
using namespace std;

struct Button
{
	void erase_background()
	{
		println("erasing button background...");
	}
	void paint()
	{
		println("painting button...");
	}
};

struct CheckBox
{
	void erase_background()
	{
		println("erasing checkbox background...");
	}
	void paint()
	{
		println("painting checkbox...");
	}
};

class UI
{
	struct UI_base
	{
		virtual void draw() = 0;
		virtual ~UI_base() = default;
	};

	template <typename T>
	struct UI_impl: UI_base
	{
	private:
		T t;

	public:
	    template <typename U>
		UI_impl(U&& unit): t{std::forward<U>(unit)} {}

		void draw() override
		{
			t.erase_background();
			t.paint();
		}
	};


	shared_ptr<UI_base> p;

public:
	template <typename O>
	UI(O&& obj): p {make_unique<UI_impl<std::decay_t<O>>>(std::forward<O>(obj))} {}

	void draw()
	{
		p->draw();
	}
};


int main()
{
	vector<UI> v {UI {Button{}}, UI {CheckBox{}} };

	for (auto& u: v)
	{
		u.draw();
	}
}

// 以相同的方式处理那些不继承自公共基类的类型


