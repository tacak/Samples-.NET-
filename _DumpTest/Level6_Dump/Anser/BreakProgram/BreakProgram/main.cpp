#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long TestFunc(void)
{
	long t;

	// EBP ‚ð”j‰ó‚·‚é
	time((time_t *)&t);

	return t;
}

void main(void)
{
	long t;

	system("pause");

	t = TestFunc();
	printf("time=%ld\n", t);
}