#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

char ThreadFunc(char c)
{
	char *p;
	p = (char *)((&c) - 4);

	// リターンアドレスを破壊する
	*p = 0x00;
	*(p + 1) = 0x00;

	return c;
}

void main(void)
{
	char c;

	system("pause");
	c = ThreadFunc('A');

	printf("%c\n", c);
}