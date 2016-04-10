#include "stdafx.h"
#include "cheat.h"
#define  TIME_SLOT 2
#define TIME_BETWEEN_TWO_CLICK 65
using namespace std;

class Cheat_Imp : public Cheat
{
public:
	Cheat_Imp(const PCHAR gameCap, LPCVOID chessBase)
		: Cheat(), gameCap(gameCap), gameh(NULL), processid(0) , baseChess(chessBase)
	{
		wdRect.bottom = wdRect.left = wdRect.right = wdRect.top = 0;
		processH = NULL;
	}
	bool init();
	bool startCheatting();
private:
	const PCHAR gameCap;
	HWND  gameh;
	DWORD processid;
	RECT wdRect;
	HANDLE processH;
	BYTE ChessNum[11][19];
	LPCVOID baseChess;
	void MasterBiu(BYTE(&)[11][19], map< BYTE, vector<pair<int, int>> > &);
	void Cheat_Imp::_SEND_MESSAGE(pair<int, int> p);
	bool Cheat_Imp::checkHorizon(BYTE(&chess)[11][19], pair<int, int>p1, pair<int, int>p2);
	bool Cheat_Imp::checkVertical(BYTE(&chess)[11][19], pair<int, int>p1, pair<int, int>p2);
};

shared_ptr<Cheat> Cheat::create(const PCHAR gameCap, LPCVOID ptr)
{
	return  shared_ptr<Cheat>(new Cheat_Imp(gameCap, ptr));
}

bool  Cheat_Imp::init()
{
	/*
	HANDLE hToken;
	BOOL fOk = FALSE;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))
			printf("Can't lookup privilege value.\n");
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
			printf("Can't adjust privilege value.\n");
		fOk = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	*/
	stat = 1;
	gameh = ::FindWindowA(NULL,	gameCap);
	cout << ::GetLastError() << endl;
	if (gameh == NULL)
	{
		cout << "llk game window not found !" << endl;
		return false;
	}
	stat += 1;
	::GetWindowThreadProcessId(gameh, &processid);
	if (processid  == 0)
	{
		cout << "llk processid not found !" << endl;
		return false;
	}
	stat += 1;
	::GetWindowRect(gameh, &wdRect);
	if (wdRect.bottom == 0  && wdRect.top == 0)
	{
		cout << "llk window size cannot be loaded !" << endl;
		return false;
	}
	stat += 1;
	processH = ::OpenProcess(PROCESS_ALL_ACCESS, false, processid);
	if (processH == NULL)
	{
		cout << "llk process open failed !" << endl;
		return false;
	}
	return true;
}

bool Cheat_Imp::startCheatting()
{
	
	map< BYTE, vector<pair<int, int>> > pairs;

	DWORD count;
	do
	{
		pairs.clear();
		bool success = ::ReadProcessMemory(processH, (LPCVOID)baseChess, (LPVOID)&ChessNum, 11 * 19, &count);
		if (!success || count != 11*19)
		{
			cout << ::GetLastError() << endl;
			cout << "chess field cannot get !" << endl;
			::Sleep(500);
			cout << "reinitiating !" << endl;
			if (this->init() == false)
				return false;
		}
		for (int i = 0; i < 11; i++) 
			for (int j = 0; j < 19; j++)
				if(ChessNum[i][j] != 0)
					pairs[ChessNum[i][j]].push_back(make_pair(i, j));
		if (pairs.empty())
			break;
		MasterBiu(ChessNum, pairs);
	} while (true);
	return true;
}

void Cheat_Imp::MasterBiu(BYTE(& chess)[11][19], map< BYTE, vector<pair<int, int>> >& pairs)
{
	ofstream log("log.txt");
	log << left;
	int count = 0;
	while (!pairs.empty())
	{
		int newCount = count;
		for (auto first = pairs.begin(); first != pairs.end(); /**/)
		{
			auto &sameColor = first->second;
			bool out = false;
			for (int i = 0; i < (int)(sameColor.size()) - 1; i++)
			{
				for (int j = i + 1; j < (int)(sameColor.size()); j++)
				{
					if (checkHorizon(chess, sameColor[i], sameColor[j]) || checkVertical(chess, sameColor[i], sameColor[j]))
					{
						out = true;				
						_SEND_MESSAGE(sameColor[i]);
						_SEND_MESSAGE(sameColor[j]);
						::Sleep(TIME_BETWEEN_TWO_CLICK);
						log << setw(5) << (int)(first->first) << setw(3) << sameColor[i].first << setw(3) << sameColor[i].second << setw(3)<< sameColor[j].first << setw(3) << sameColor[j].second << endl;
						chess[sameColor[j].first][sameColor[j].second] = chess[sameColor[i].first][sameColor[i].second] = 0;
						std::swap(sameColor[i], sameColor[sameColor.size() - 1]);
						std::swap(sameColor[j], sameColor[sameColor.size() - 2]);
						sameColor.pop_back();
						sameColor.pop_back();
						newCount = count + 2;
						//system("pause");
						::Sleep(TIME_SLOT);
						break;
					}
				}
				if (out)break;
			}
			if (sameColor.empty())
				first = pairs.erase(first);
			else  ++first;
		}
		if (count == newCount)
			return;
		count = newCount;
	}
	return;
}

void Cheat_Imp::_SEND_MESSAGE(pair<int, int> p)
{
	LPARAM loc;
	loc = ((p.first * 35 + 192) << 16) + (p.second * 31 + 21);
	::PostMessageA(gameh, WM_LBUTTONDOWN, 0, loc);
	::PostMessageA(gameh, WM_LBUTTONUP, 0, loc);
}

bool Cheat_Imp::checkHorizon(BYTE(&chess)[11][19], pair<int, int>p1, pair<int, int>p2)
{
	if (p1.second == p2.second && abs(p1.first - p2.first) == 1)
		return true;
	bool horizon[2][19];
	for (int p = 0; p < 2; p++)
	{
		pair<int, int>no = p ? p1 : p2;
		horizon[p][no.second] = true;
		bool connect = true;
		for (int q = no.second + 1; q < 19; q++)
		{
			if (connect && chess[no.first][q] == 0)
				horizon[p][q] = true;
			else
			{
				horizon[p][q] = false;
				connect = false;
			}
		}
		connect = true;
		for (int q = no.second - 1; q >= 0; q--)
		{
			if (connect && chess[no.first][q] == 0)
				horizon[p][q] = true;
			else
			{
				horizon[p][q] = false;
				connect = false;
			}
		}
	}
	int b = min(p1.first, p2.first);
	int e = max(p1.first, p2.first);
	for (int i = 0; i < 19; i++)
		if (horizon[0][i] && horizon[1][i])
		{
			bool noHinder = true;
			for (int j = b + 1 ; j < e; j++)
				if (chess[j][i] != 0)
				{
					noHinder = false;
					break;
				}
			if (noHinder)
				return true;
		}
	return false;
}

bool Cheat_Imp::checkVertical(BYTE(&chess)[11][19], pair<int, int>p1, pair<int, int>p2)
{
	if (p1.first == p2.first && abs(p1.second - p2.second) == 1)
		return true;
	bool vertical[2][11];
	for (int p = 0; p < 2; p++)
	{
		pair<int, int>no = p ? p1 : p2;
		vertical[p][no.first] = true;
		bool connect = true;
		for (int q = no.first + 1; q < 11; q++)
		{
			if (connect && chess[q][no.second] == 0)
				vertical[p][q] = true;
			else
			{
				vertical[p][q] = false;
				connect = false;
			}
		}
		connect = true;
		for (int q = no.first - 1; q >= 0; q--)
		{
			if (connect && chess[q][no.second] == 0)
				vertical[p][q] = true;
			else
			{
				vertical[p][q] = false;
				connect = false;
			}
		}
	}
	int b = min(p1.second, p2.second);
	int e = max(p1.second, p2.second);
	for (int i = 0; i < 11; i++)
		if (vertical[0][i] && vertical[1][i])
		{
			bool noHinder = true;
			for (int j = b; j <= e; j++)
				if (chess[i][j] != 0)
				{
					noHinder = false;
					break;
				}
			if (noHinder)
				return true;
		}
	return false;
}

