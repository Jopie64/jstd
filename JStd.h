#pragma once

#include <memory>
#include <string>

namespace std { namespace tr1{} using namespace tr1; }

namespace JStd
{

template <class TP_Obj>
class CJRefCountObjTempl
{
public:
    typedef std::shared_ptr<TP_Obj> TRefPtr;
    typedef std::weak_ptr<TP_Obj> TWeakPtr;

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

    typedef std::shared_ptr<TP_Obj> TRefPtr;
    typedef std::weak_ptr<TP_Obj> TWeakPtr;
};

class CSharedNewHelper
{
public:
	template<class TP_Obj>
    std::shared_ptr<TP_Obj> operator<<(TP_Obj* P_ObjPtr)
	{
        return std::shared_ptr<TP_Obj>(P_ObjPtr);
	}
};

#define shared_new JStd::CSharedNewHelper() << new

namespace String
{

	
std::string Format(const char* P_FormatPtr, ...);
std::wstring Format(const wchar_t* P_FormatPtr, ...);

std::string ToMult(const std::wstring& str, unsigned codePage);
inline std::string ToMult(const std::string& str, unsigned) { return str; }
std::wstring ToWide(const std::string& str, unsigned codePage);
inline std::wstring ToWide(const std::wstring& str, unsigned) { return str; }
#ifdef _UNICODE
#define ToAW ToWide
#else
#define ToAW ToMult
#endif

void		TrimRight(std::string& str, const char* charsToTrim);
void		TrimLeft(std::string& str, const char* charsToTrim);
void		Trim(std::string& str, const char* charsToTrim);
void		Remove(std::string& str, char charRem);
void		ToUpper(std::string& str);

} //namespace


template <class T_Derived>
class CSingleton
{
public:
	CSingleton()
	{
#ifdef ASSERT
		ASSERT(m_pInterface == NULL);
#endif
	}

	static T_Derived& I()
	{
		if(m_pInterface == NULL)
			m_pInterface = new T_Derived;
		return *m_pInterface;
	}

	static void Singleton_Destroy()
	{
		delete m_pInterface;
		m_pInterface = NULL;
	}

private:

	static T_Derived* m_pInterface;
};

template <class T_Derived>
T_Derived* JStd::CSingleton<T_Derived>::m_pInterface = NULL;


template<class TP_Val>
class COptional
{
public:
	COptional():m_bConstructed(false){}
	~COptional(){ Destroy(); }

	COptional(const COptional& P_That):m_bConstructed(false){ if(P_That) (*this)(*P_That); }

	COptional operator=(const COptional& P_That){ Destroy(); if(P_That) (*this)(*P_That); }

	void Destroy(){ if(!IsValid())return; Get().~TP_Val(); m_bConstructed = false; }
	TP_Val GetAndDestroy(){ TP_Val W_Return(Get()); Destroy(); return W_Return; }

	//Constructors
	TP_Val& operator()(){Destroy(); new (m_Data) TP_Val(); m_bConstructed = true; return Get(); }
	template<class TP_1>
	TP_Val& operator()(const TP_1& P_1){Destroy(); new (m_Data) TP_Val(P_1); m_bConstructed = true; return Get(); }
	template<class TP_1, class TP_2>
    TP_Val& operator()(const TP_1& P_1, const TP_2& P_2){Destroy(); new (m_Data) TP_Val(P_1, P_2); m_bConstructed = true; return Get(); }
	template<class TP_1, class TP_2, class TP_3>
    TP_Val& operator()(const TP_1& P_1, const TP_2& P_2, const TP_3& P_3){Destroy(); new (m_Data) TP_Val(P_1, P_2, P_3); m_bConstructed = true; return Get(); }

	//Validators
	bool IsValid() const {return m_bConstructed;}
	void CheckValid() const { if(!IsValid()) throw std::logic_error("Not constructed."); }
	typedef bool (COptional::*T_IsValidFuncPtr)() const;
	operator T_IsValidFuncPtr () const {return IsValid() ? &COptional::IsValid : NULL;}

	//Getters
	TP_Val& Get() { CheckValid(); return (TP_Val&)m_Data; }
	const TP_Val& Get() const { CheckValid(); return (TP_Val&)m_Data; }

	TP_Val& operator*() {return Get();}
	const TP_Val& operator*() const { return Get(); }
	TP_Val* operator->() {return &Get();}
	const TP_Val* operator->() const { return &Get(); }


private:
	void* Data(){return m_Data;}
	char m_Data[sizeof(TP_Val)];
	bool m_bConstructed;

};

void Assert(bool ok, const wchar_t* assertion);

#ifndef ASSERT
#	ifdef _DEBUG
#		define ASSERT(assertion) Assert((assertion), L#assertion)
#	else
#		define ASSERT(assertion)
#	endif
#endif

}//namespace JStd

#ifndef _T
#ifdef _UNICODE
#define _T(P) L ## P
#else
#define _T(P) P
#endif
#endif
