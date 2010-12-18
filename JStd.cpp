#include "StdAfx.h"
#include "JStd.h"
#include <cstdarg>
#include <cstdio>


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
	_vsnprintf(&*W_sReturn.begin(),W_iSize,P_FormatPtr,W_va);
	return W_sReturn;
}

std::wstring Format(const wchar_t* P_FormatPtr, ...)
{
	va_list W_va;
	va_start(W_va,P_FormatPtr);
	size_t W_iSize = _vscwprintf(P_FormatPtr,W_va);
	std::wstring W_sReturn;
	W_sReturn.resize(W_iSize);
	_vsnwprintf(&*W_sReturn.begin(),W_iSize,P_FormatPtr,W_va);
	return W_sReturn;
}



} //String


}
