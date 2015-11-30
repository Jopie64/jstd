#include "JStd.h"
#include <cstdarg>
#include <cstdio>
#ifdef WIN32
#include <windows.h>
#else
#include <cstring>
#include <cstdarg>
#include <cwchar>
#endif
#include <algorithm>

using namespace std;

#ifdef _MSC_VER
#define jvsnprintf _vsnprintf_s
#define jvsnwprintf _vsnwprintf_s
#define jvscprintf _vscprintf
#define jvscwprintf _vscwprintf
#else
#define jvsnprintf vsnprintf
#define jvsnwprintf vswprintf
int jvscprintf (const char * format, va_list pargs)
{
    int retval;
    va_list argcopy;
    va_copy(argcopy, pargs);
    retval = vsnprintf(NULL, 0, format, argcopy);
    va_end(argcopy);
    return retval;
}
int jvscwprintf (const wchar_t * format, va_list pargs)
{
    int retval;
    va_list argcopy;
    va_copy(argcopy, pargs);
    retval = vswprintf(NULL, 0, format, argcopy);
    va_end(argcopy);
    return retval;
}
#endif

namespace JStd
{

namespace String
{

std::string Format(const char* P_FormatPtr, ...)
{
	va_list W_va;
	va_start(W_va,P_FormatPtr);
    size_t W_iSize = jvscprintf(P_FormatPtr,W_va);
	std::string W_sReturn;
	W_sReturn.resize(W_iSize);
#ifdef _MSC_VER
    jvsnprintf(&*W_sReturn.begin(), W_iSize+1, W_iSize, P_FormatPtr, W_va);
#else
    jvsnprintf(&*W_sReturn.begin(), W_iSize+1, P_FormatPtr, W_va);
#endif
    return W_sReturn;
}

std::wstring Format(const wchar_t* P_FormatPtr, ...)
{
	va_list W_va;
	va_start(W_va,P_FormatPtr);
    size_t W_iSize = jvscwprintf(P_FormatPtr,W_va);
	std::wstring W_sReturn;
    W_sReturn.resize(W_iSize);
#ifdef _MSC_VER
    jvsnwprintf(&*W_sReturn.begin(),W_iSize+1,W_iSize,P_FormatPtr,W_va);
#else
    jvsnwprintf(&*W_sReturn.begin(),W_iSize+1,P_FormatPtr,W_va);
#endif
	return W_sReturn;
}

#ifdef WIN32

std::string ToMult(const std::wstring& str, unsigned codePage)
{
   std::string multStr;
   int length = WideCharToMultiByte(codePage, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
   if (length>1)
   { 
      multStr.resize(length-1);
      WideCharToMultiByte(codePage, 0, str.c_str(), -1, &*multStr.begin(), length, NULL, NULL);
   }
   return multStr;
}

std::wstring ToWide(const std::string& str, unsigned codePage)
{
   std::wstring wideStr;
   int length = MultiByteToWideChar(codePage, 0, str.c_str(), -1, NULL, 0);
   if (length>1)
   { 
      wideStr.resize(length-1);
      MultiByteToWideChar(codePage, 0, str.c_str(), -1, &*wideStr.begin(), length);
   }
   return wideStr;
}
#endif

void TrimRight(std::string& str, const char* charsToTrim)
{
	std::string::size_type place = str.find_last_not_of(charsToTrim);
	if(place == std::string::npos)
	{
		str.clear();
		return;
	}
	str.resize(place + 1);
}

void TrimLeft(std::string& str, const char* charsToTrim)
{
	std::string::size_type place = str.find_first_not_of(charsToTrim);
	if(place == std::string::npos)
	{
		str.clear();
		return;
	}
	if(place == 0)
		return;
	str = str.substr(place);
}

void Trim(std::string& str, const char* charsToTrim)
{
	TrimRight(str, charsToTrim);
	TrimLeft(str, charsToTrim);
}

void Remove(std::string& str, char charRem)
{
	string tempStr = str;
	str.clear();
	for(size_t i = 0; i < tempStr.size(); ++i)
		if(tempStr[i] != charRem)
			str += tempStr[i];
}

void ToUpper(std::string& str)
{
    transform(str.begin(), str.end(), str.begin(), [](char c) -> char { return toupper(c); });
}



} //String

#ifdef WIN32

void Assert(bool ok, const wchar_t* assertion)
{
	if (ok)
		return;
	MessageBox(NULL, _T("Assertion failed"), String::ToAW(wstring(assertion), CP_ACP).c_str(), MB_ICONHAND);
}
#endif


}
