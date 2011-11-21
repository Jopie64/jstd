#include "StdAfx.h"
#include "CmdLine.h"
#include "JStd.h"
#include <map>

namespace JStd
{
namespace CmdLine
{

using namespace std;

typedef map<wstring, FuncCmd> MapFunc;

struct Registrar: CSingleton<Registrar>
{
	MapFunc m_mapCmd;
};

bool Register(const wchar_t* cmd, const FuncCmd& cmdFunc)
{
	Registrar::I().m_mapCmd[cmd] = cmdFunc;
	return true;
}

void Call(const wchar_t* cmd, int argc, wchar_t* argv[])
{
	if (FuncCmd& func = Registrar::I().m_mapCmd[cmd])
		func(argc, argv);
	else
		throw std::runtime_error(String::Format("Command '%s' not understood.", String::ToMult(cmd, 0).c_str()));

}

void CallDefault(int argc, wchar_t* argv[])
{
	if(argc < 1)
		throwUsage(String::Format(L"No options??? Impossible."));
	if(argc < 2)
		throwUsage(String::Format(L"Usage: %s [command] <options...>", argv[0]));
	Call(argv[1], argc, argv);
}



}//CmdLine
}//JStd