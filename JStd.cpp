#include "StdAfx.h"
#include "JStd.h"
#include <stdio.h>


namespace JStd
{

namespace String
{

std::string Format(const char* P_FormatPtr, ...)
{
	va_list W_va;
	va_start(W_va,P_FormatPtr);
	int W_iSize=_vsnprintf(NULL,0,P_FormatPtr,W_va);
	std::string W_sReturn;
	W_sReturn.resize(W_iSize);
	_vsnprintf(&*W_sReturn.begin(),W_iSize,P_FormatPtr,W_va);
	return W_sReturn;
}




} //String


}
