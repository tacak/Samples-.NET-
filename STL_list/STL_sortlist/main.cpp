#include <iostream>
#include <algorithm>
#include <list>

// これを入れないと入力がめんどい
// 例 ) vector<int> vi; は、std::vector<int> vi; とする必要がある
using namespace std;

void main(void)
{
	list<double> li;

	li.push_back(10.5);
	li.push_back(20.54);
	li.push_back(123.3);
	li.push_back(15.321);
	li.push_back(7.77);

	printf( "init, size: %d, %f .. %f\n", li.size(), li.front(), li.back() ) ;

	li.sort();
	
	printf( "init, size: %d, %f .. %f\n", li.size(), li.front(), li.back() ) ;
}