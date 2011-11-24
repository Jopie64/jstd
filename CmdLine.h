#pragma once
#include <functional>
#include <vector>
#include <string>

namespace JStd
{
namespace CmdLine
{

class CmdLine
{
public:
	class Arg
	{
	public:
		Arg();
		bool			m_isOption;
		std::wstring	m_value;
		bool			m_isRead;
	};

	typedef std::vector<Arg> VectorArg;

	CmdLine(int argc, wchar_t* argv[]);

	std::wstring	next(const wchar_t* longOption = NULL);
	std::wstring	next(wchar_t shortOption, const wchar_t* longOption = NULL);

	bool			hasOption(const wchar_t* longOption = NULL);
	bool			hasOption(wchar_t shortOption, const wchar_t* longOption = NULL);

private:
	VectorArg::iterator toOption(wchar_t shortOption, const wchar_t* longOption);
	int			m_argc;
	wchar_t**	m_argv;
	VectorArg	m_vectArgs;
};

//typedef std::tr1::function<void(int argc, wchar_t* argv[])> FuncCmd;
typedef std::tr1::function<void(CmdLine&)>					FuncCmd;

//Returns bool so you can do something like
//static bool registered = CmdLine::Register("command", &onCommand);
bool Register(const wchar_t* cmd, const FuncCmd& cmdFunc);

void Call(const wchar_t* cmd, CmdLine& cmdLine);
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