#include <stdio.h>
#include <stdlib.h>

int TestFunc1(int n1, int n2, int n3)
{
	char buf[65536];

	printf("Test Message\n");

	return TestFunc1(n1, n2, n3);
}

void main(void)
{
	int n1, n2, n3;

	system("pause");

	n1 = n2 = n3 = 255;
	TestFunc1(n1, n2, n3);
}