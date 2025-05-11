#include <print>
using namespace std;

template <typename T>
class UI
{
	T* derived()
	{
		return static_cast<T*>(this);
	}
public:
	void draw()
	{
		derived()->erase_background();
		derived()->paint();
	}
};

class Button : public UI<Button>
{
	friend class UI<Button>;

	void erase_background()
	{
		println("erasing button background...");
	}
	void paint()
	{
		println("painting button...");
	}
};

class CheckBox : public UI<CheckBox>
{
	friend class UI<CheckBox>;

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
void draw_ui(UI<T>& u)
{
	u.draw();
}

int main()
{
	Button b;
	CheckBox c;

	draw_ui(b);
	draw_ui(c);
}
