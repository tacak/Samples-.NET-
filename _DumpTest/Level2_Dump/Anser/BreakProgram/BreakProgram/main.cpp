#include <stdio.h>
#include "BreakModule.h"

#pragma comment(lib, "BreakModule.lib")

void main(void)
{
	char p[255];
	int n1, n2;

	n1 = n2 = 255;

	TestFunction1(p, n1, n2);
}