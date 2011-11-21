#pragma once
#include <functional>

namespace JStd
{
namespace CmdLine
{

typedef std::tr1::function<void(int argc, wchar_t* argv[])> FuncCmd;

//Returns bool so you can do something like
//static bool registered = CmdLine::Register("command", &onCommand);
bool Register(const wchar_t* cmd, const FuncCmd& cmdFunc);

void Call(const wchar_t* cmd, int argc, wchar_t* argv[]);
void CallDefault(int argc, wchar_t* argv[]);

class CUsageException
{
public:
	CUsageException(const std::wstring& usage):m_usage(usage){}
	std::wstring m_usage;
};

inline void throwUsage(const std::wstring& usage){ throw CUsageException(usage); }

template<class T_Stream>
void CallDefaultWithCatch(T_Stream& str, int argc, wchar_t* argv[])
{
	try
	{
		CallDefault(argc, argv);
	}
	catch(CUsageException& e)
	{
		str << L"Usage: " << e.m_usage.c_str() << endl;
	}
	catch(std::exception& e)
	{
		str << L"Error: " << JStd::String::ToWide(e.what(), CP_ACP) << endl;
	}
}

}//CmdLine
}//JStd