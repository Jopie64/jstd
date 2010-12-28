#pragma once

#include <memory>
#include <string>

#ifndef J_CREATING_LIB

#ifdef _DEBUG
#pragma comment(lib,"D_JStd.lib")
#else //ifdef _DEBUG
#pragma comment(lib,"JStd.lib")
#endif //ifdef _DEBUG

#endif //ifndef J_CREATING_LIB

namespace JStd
{

template <class TP_Obj>
class CJRefCountObjTempl
{
public:
	typedef std::tr1::shared_ptr<TP_Obj> TRefPtr;
	typedef std::tr1::weak_ptr<TP_Obj> TWeakPtr;

};

template <class TP_Obj, class TP_Obj_Base>
class CJRefCountDerivedTempl : public TP_Obj_Base
{
public:
	CJRefCountDerivedTempl(){}
	template<class TP_1>
	CJRefCountDerivedTempl(TP_1& P_1):TP_Obj_Base(P_1){}
	template<class TP_1,class TP_2>
	CJRefCountDerivedTempl(TP_1& P_1, TP_2& P_2):TP_Obj_Base(P_1,P_2){}
	template<class TP_1,class TP_2, class TP_3>
	CJRefCountDerivedTempl(TP_1& P_1, TP_2& P_2, TP_3& P_3):TP_Obj_Base(P_1,P_2,P_3){}
	template<class TP_1,class TP_2, class TP_3, class TP_4>
	CJRefCountDerivedTempl(TP_1& P_1, TP_2& P_2, TP_3& P_3, TP_4& P_4):TP_Obj_Base(P_1,P_2,P_3,P_4){}
	template<class TP_1,class TP_2, class TP_3, class TP_4, class TP_5>
	CJRefCountDerivedTempl(TP_1& P_1, TP_2& P_2, TP_3& P_3, TP_4& P_4, TP_5& P_5):TP_Obj_Base(P_1,P_2,P_3,P_4,P_5){}
	template<class TP_1,class TP_2, class TP_3, class TP_4, class TP_5, class TP_6>
	CJRefCountDerivedTempl(TP_1& P_1, TP_2& P_2, TP_3& P_3, TP_4& P_4, TP_5& P_5, TP_6& P_6):TP_Obj_Base(P_1,P_2,P_3,P_4,P_5,P_6){}

	typedef std::tr1::shared_ptr<TP_Obj> TRefPtr;
	typedef std::tr1::weak_ptr<TP_Obj> TWeakPtr;
};

class CSharedNewHelper
{
public:
	template<class TP_Obj>
	std::tr1::shared_ptr<TP_Obj> operator<<(TP_Obj* P_ObjPtr)
	{
		return std::tr1::shared_ptr<TP_Obj>(P_ObjPtr);
	}
};

#define shared_new JStd::CSharedNewHelper() << new

namespace String
{

	
std::string Format(const char* P_FormatPtr, ...);
std::wstring Format(const wchar_t* P_FormatPtr, ...);

std::string ToMult(const std::wstring& str, unsigned codePage);
std::wstring ToWide(const std::string& str, unsigned codePage);

} //namespace

};
