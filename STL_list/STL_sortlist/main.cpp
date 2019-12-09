#include <iostream>
#include <algorithm>
#include <list>

// ‚±‚ê‚ğ“ü‚ê‚È‚¢‚Æ“ü—Í‚ª‚ß‚ñ‚Ç‚¢
// —á ) vector<int> vi; ‚ÍAstd::vector<int> vi; ‚Æ‚·‚é•K—v‚ª‚ ‚é
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