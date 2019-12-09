#include <stdio.h>
#include <string.h>
#include "BreakModule.h"

int TestFunction1(char *p, int n1, int n2)
{
	int ans;
	ans = n1 - n2;

	TestFunction3(p, ans);

	return ans;
}

void TestFunction2(char *p)
{
	*p = '!';
}

void TestFunction3(char *p, int n)
{
	char buf[20];

	TestFunction2(p);
	sprintf(buf, "%d", 255 / n);
	strcat(p, buf);
}