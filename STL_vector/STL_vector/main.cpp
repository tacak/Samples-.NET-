#include <iostream>
#include <vector>
#include <algorithm>
#include <list>

// ‚±‚ê‚ğ“ü‚ê‚È‚¢‚Æ“ü—Í‚ª‚ß‚ñ‚Ç‚¢
// —á ) vector<int> vi; ‚ÍAstd::vector<int> vi; ‚Æ‚·‚é•K—v‚ª‚ ‚é
using namespace std;

void main(void)
{
	vector<int> vi;

	for(int i = 0; i < 5; i++){
		vi.push_back(100 * (i + 1));
	}

	for(int i = 0; i < vi.size(); i++){
		//cout << vi[i] << "\n";
		cout << vi.back() << "\n";
		vi.pop_back();
	}
}