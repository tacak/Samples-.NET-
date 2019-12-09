#include <stdio.h>

template<class T, class U> class Test
{
  public:

	T TestFunc(T n)
	{
		return n + n;
	}

	U TestFunc(U n)
	{
		return n + n;
	}
};

template <class T> T TestFunc(T n)
{
	n += n;

	return n;
}

void main(void)
{
	Test<int, double> ts;

	printf("ans: %d %f\n", ts.TestFunc(100), ts.TestFunc(1.0));

	printf("%f\n", TestFunc(1.5));
}