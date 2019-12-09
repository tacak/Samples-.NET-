#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

void testfunc2(char *p)
{
	char *pp;
	//strcat(p, " - testfunc2");
	pp = (char *)0xFFFFFFFF;
	*pp = 1000;
	
}

void testfunc1(char *p, int a)
{
	sprintf(p, "%d", a);

	testfunc2(p);
}

void main(void)
{
	//char *p;
	//p = (char *)malloc(20);
	//memset(p, 1, 20);
	char p[20];

	system("pause");
	testfunc1(p, 1000000000);

	printf("%s\n", p);
}