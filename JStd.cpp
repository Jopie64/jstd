#include "StdAfx.h"
#include "JStd.h"
#include <cstdarg>
#include <cstdio>
#include <windows.h>
#include <algorithm>

using namespace std;

namespace JStd
{

namespace String
{

std::string Format(const char* P_FormatPtr, ...)
{
	va_list W_va;
	va_start(W_va,P_FormatPtr);
	size_t W_iSize = _vscprintf(P_FormatPtr,W_va);
	std::string W_sReturn;
	W_sReturn.resize(W_iSize);
	_vsnprintf_s(&*W_sReturn.begin(), W_iSize+1, W_iSize, P_FormatPtr, W_va);
	return W_sReturn;
}

std::wstring Format(const wchar_t* P_FormatPtr, ...)
{
	va_list W_va;
	va_start(W_va,P_FormatPtr);
	size_t W_iSize = _vscwprintf(P_FormatPtr,W_va);
	std::wstring W_sReturn;
	W_sReturn.resize(W_iSize);
	_vsnwprintf_s(&*W_sReturn.begin(),W_iSize+1,W_iSize,P_FormatPtr,W_va);
	return W_sReturn;
}



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
	transform(str.begin(), str.end(), str.begin(), toupper);
}



} //String

void Assert(bool ok, const wchar_t* assertion)
{
	if (ok)
		return;
	MessageBox(NULL, L"Assertion failed", assertion, MB_ICONHAND);
}


}
