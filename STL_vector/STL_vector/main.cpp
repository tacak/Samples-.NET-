#include <iostream>
#include <vector>
#include <algorithm>
#include <list>

// ��������Ȃ��Ɠ��͂��߂�ǂ�
// �� ) vector<int> vi; �́Astd::vector<int> vi; �Ƃ���K�v������
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