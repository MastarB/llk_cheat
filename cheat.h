#pragma once
#include "stdafx.h"
using namespace std;
typedef PVOID Proc_ptr;
typedef LPCVOID cProc_ptr;
class Cheat
{
public:
	Cheat() : stat(0) { }
	virtual bool init() = 0;
	virtual bool startCheatting() = 0;
	static shared_ptr<Cheat> create(const PCHAR , LPCVOID);
protected:
	int stat;
};