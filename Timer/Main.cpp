#include "Timer.hpp"
#include <iostream>

int square(int i)
{
	return i * i;
}

struct Klasse
{
	int k;

	void fn(int i, int j)
	{
		k += i * j;
		std::cout << k << std::endl;
	}
};

int main()
{
	time_accumulator acc;
	int i = acc.measure(square, 15);
	std::cout << acc.get_time() << std::endl;

	Klasse d{ 0 };
	acc.measure(&Klasse::fn, &d, 1, 2);
	std::cout << acc.get_time() << std::endl;
}
