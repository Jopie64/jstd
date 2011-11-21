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

class CUsageException
{
public:
	CUsageException(const std::wstring& usage):m_usage(usage){}
	std::wstring m_usage;
};

inline void throwUsage(const std::wstring& usage){ throw CUsageException(usage); }


}//CmdLine
}//JStd