#include <print>
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

template <typename T>
struct UI: public T
{
	void draw()
	{
		T::erase_background();
		T::paint();
	}
};

template <typename T>
void draw_ui(UI<T>& u)
{
	u.draw();
}

int main()
{
	UI<Button> b;
	UI<CheckBox> c;

	b.draw();
	c.draw();
}

// mixin是倒置的CRTP

// 如果想把UI类放入vector等容器中，
// 需要让它继承一个具有virtual void draw() = 0;的父类。
