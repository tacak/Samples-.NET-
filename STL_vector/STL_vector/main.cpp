#include <iostream>
#include <vector>
#include <algorithm>
#include <list>

// これを入れないと入力がめんどい
// 例 ) vector<int> vi; は、std::vector<int> vi; とする必要がある
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