#include "StdAfx.h"
#include "CmdLine.h"
#include "JStd.h"
#include <map>

namespace JStd
{
namespace CmdLine
{

using namespace std;

CmdLine::Arg::Arg()
:	m_isOption(false),
	m_isRead(false)
{
}

CmdLine::CmdLine(int argc, wchar_t* argv[])
:	m_argc(argc), m_argv(argv)
{
	m_vectArgs.reserve(argc);
	for(int i = 0; i < argc; ++i)
	{
		Arg* curr = &*m_vectArgs.insert(m_vectArgs.end(), Arg());
		const wchar_t* currArg = argv[i];
		if(*currArg == '-')
		{
			curr->m_isOption = true;
			++currArg;
			if(*currArg == '-')
			{
				++currArg;
				curr->m_value = currArg;
				continue;
			}
			while(*currArg != '\0')
			{
				if(!curr)
				{
					curr = &*m_vectArgs.insert(m_vectArgs.end(), Arg());
					curr->m_isOption = true;
				}

				curr->m_value.push_back(*currArg);
				curr = NULL;
				++currArg;
			}
			continue;
		}
		curr->m_value = currArg;
	}
}

std::wstring CmdLine::next(const wchar_t* longOption)
{
	return next(0, longOption);
}

std::wstring CmdLine::next(wchar_t shortOption, const wchar_t* longOption)
{
	VectorArg::iterator i = toOption(shortOption, longOption);
	if(i == m_vectArgs.end())
		return L"";
	if(!shortOption && !longOption)
		return i->m_value; //Since this is a value without option, i points at the value already.

	++i;
	if(i == m_vectArgs.end() || i->m_isOption)
		throw std::runtime_error(String::Format("Expected value after option %s", String::ToMult((--i)->m_value, CP_ACP).c_str()));
	if(i->m_isRead)
		throw std::logic_error(String::Format("Value of option %s was already read. This is a software error.", String::ToMult((--i)->m_value, CP_ACP).c_str()));
	i->m_isRead = true;
	return i->m_value;
}

bool CmdLine::hasOption(const wchar_t* longOption)
{
	return hasOption(0, longOption);
}

bool CmdLine::hasOption(wchar_t shortOption, const wchar_t* longOption)
{
	return toOption(shortOption, longOption) != m_vectArgs.end();
}

CmdLine::VectorArg::iterator CmdLine::toOption(wchar_t shortOption, const wchar_t* longOption)
{
	for(VectorArg::iterator i = m_vectArgs.begin(); i != m_vectArgs.end(); ++i)
	{
		if(i->m_isRead || i->m_value.empty()) continue;
		if(!longOption && !shortOption && !i->m_isOption) { i->m_isRead = true; return i; }
		if(!i->m_isOption) continue;
		if( longOption && i->m_value == longOption ||
			shortOption && i->m_value[0] == shortOption )
		{
			i->m_isRead = true;
			return i;
		}
	}
	return m_vectArgs.end();
}

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

void Call(const wchar_t* cmd, CmdLine& cmdLine)
{
	if (FuncCmd& func = Registrar::I().m_mapCmd[cmd])
		func(std::tr1::ref(cmdLine));
	else
		throw std::runtime_error(String::Format("Command '%s' not understood.", String::ToMult(cmd, 0).c_str()));

}

void CallDefault(int argc, wchar_t* argv[])
{
	CmdLine cmdLine(argc, argv);
	if(cmdLine.next().empty())
		throwUsage(String::Format(L"No options??? Impossible."));
	wstring cmd = cmdLine.next();
	if(cmd.empty())
		throwUsage(String::Format(L"Usage: %s [command] <options...>", argv[0]));

	Call(cmd.c_str(), cmdLine);
}



}//CmdLine
}//JStd