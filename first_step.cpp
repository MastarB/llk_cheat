// first_step.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "cheat.h"
using namespace std;

int main()
{
	cout << "warnning " << endl;
	cout << "This software can be only used for the purpose of study.  the obligation of applying this software to any other thing will be taken by himself." << endl;
	string ans;
	cout << "I accept it or not ? (please input Y/N)" << endl;
	cin >> ans;
	if (ans.compare("Y") == 0 || ans.compare("y") == 0)
	{
		LPCVOID base= (LPCVOID)0x00199F68;//need to change after game update.
		const PCHAR  cap = "QQ游戏 - 连连看角色版";//need to change when window capition alters.
		shared_ptr<Cheat> cheat = Cheat::create(cap, base);
		cout << "please join in a table for a match.(Enter Y or y if u finished)." << endl;
		while (cin >> ans)
			if (ans.compare("Y") == 0 || ans.compare("y") == 0)
				break;
		cout << "start to get the handle of game process!" << endl;
		while (!(cheat->init()))
			::Sleep(500);
		cout << "complete." << endl;
		cout << "press 'start' and wait util game start(Enter Y or y if game starts and we can start cheating)" << endl;;
		while (cin >> ans)
			if (ans.compare("Y") == 0 || ans.compare("y") == 0)
				break;
		while (true)
		{
			int second = 3;
			while (second-- > 0)
			{
				cout << "cheating will start in " << second+1 << " minutes" << endl;
				::Sleep(600);
			}
			cout << "start cheating !!" << endl;
			while(!(cheat->startCheatting()));
			cout << "cheating finished, press anykey when it's ready for the next match !" << endl;
			system("pause");

		}
	}
}

