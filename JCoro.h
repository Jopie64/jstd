#pragma once
#include <windows.h>
#include <memory>
#include <functional>
#include <stdexcept>
#include "JStd.h"

namespace JStd {
namespace Coro {

class CExitException
{
};

class CNotInitializedException : public std::logic_error
{
public:
	CNotInitializedException() : std::logic_error("Coroutine was not initialized. Should call operator() with parameter once before the nothrow version.") {}
};

class CWaitingException : public std::exception
{
public:
	CWaitingException() : std::exception("Coroutine cannot return any value because it is waiting for an event."){}
};

class CCoro;

class CMainCoro
{
public:
	CCoro*	Get(){return m_MainCoroPtr.get();}
	CCoro*	operator->(){return Get();}
	CCoro&	operator*(){return *Get();}
private:
	std::tr1::shared_ptr<CCoro> m_MainCoroPtr;
	friend CCoro;
};

class CCoro
{
public:
	CCoro(CCoro* P_MainCoroPtr);
	virtual ~CCoro();

	static CMainCoro	Initialize();

	static CCoro* Cur();

	bool IsMain() const;

	static CCoro* Main();

	template<class TP_Cb>
	class FcStartFunc;

	template<class TP_Cb>
	static CCoro* Create(const TP_Cb& P_Cb)
	{
		CCoro* W_CoroPtr = new FcStartFunc<TP_Cb>(Main(), P_Cb);
		if((W_CoroPtr->m_AddressPtr = CreateFiber(0, &CCoro::StartFunc, W_CoroPtr)) == NULL)
			throw std::runtime_error("Error creating fiber.");
		return W_CoroPtr;
	}

	static void YieldDefault();
	void		yield(CCoro* P_YieldBackCoroPtr = Cur());
	void		Exit();
	bool		Ended() const;



protected:
	virtual void operator()() const =0;

	void OnResume();

private:
	CCoro(const CCoro&);
	CCoro& operator=(const CCoro&);

	static void CALLBACK StartFunc(void* P_FuncPtr);

	enum eExit { eA_No, eA_ExitInitiated, eA_ExitRunning, eA_ExitDone };

	CCoro*	m_MainCoroPtr;
	CCoro*	m_YieldingCoroPtr;
	CCoro*	m_DefaultYieldCoroPtr;
	void*	m_AddressPtr;
	bool	m_bEnded;
	eExit	m_eExit;
};

template<class TP_Cb>
class CCoro::FcStartFunc : public CCoro
{
friend CCoro;
	FcStartFunc(CCoro* P_MainCoroPtr, const TP_Cb& P_Cb):CCoro(P_MainCoroPtr), m_Cb(P_Cb){}
	void operator()() const { m_Cb(); }
	TP_Cb m_Cb;
};

CMainCoro	Initialize();
void		yield(); //No capital y, because winbase.h defines Yield() as a macro, pfff.

template<class TP_Cb>
static CCoro* Create(const TP_Cb& P_Cb) { return CCoro::Create(P_Cb); }

template<class TP_Return>
class CFuture
{
public:
	CFuture():m_CoroPtr(CCoro::Cur()){}
	TP_Return& operator*(){ return *m_Val; }

	void Signal(const TP_Return& P_Return){ m_Val(P_Return); m_CoroPtr->yield(); }

	bool IsResolved()const{return m_Val;}

	typedef bool (CFuture::*T_IsValidFuncPtr)() const;
	operator T_IsValidFuncPtr () const {return IsResolved() ? &CFuture::IsResolved : NULL; }

	class CCb
	{
	public:
		CCb(CFuture* P_ThisPtr):m_ThisPtr(P_ThisPtr){}

		CFuture* m_ThisPtr;

		void operator()(const TP_Return& P_Ret){ m_ThisPtr->Signal(P_Ret); }
	};

	CCb MakeCallback(){return CCb(this);}

	void Invalidate(){ m_Val.Destroy(); }

private:
	COptional<TP_Return>	m_Val;
	CCoro*					m_CoroPtr;
};


class CCoroutineBase
{
public:

	virtual ~CCoroutineBase()
	{
		delete m_CoroPtr;
	}

	bool IsValid() const { return m_CoroPtr != NULL; }
	bool IsWaiting() const { return m_bWaiting; }
	bool IsRunnable() const { return IsValid() && !IsWaiting(); }
	void EnsureValid() const { if(!IsValid()) throw std::logic_error("Used uninitialized coroutine."); }
	void EnsureRunnable() const { EnsureValid(); if(!IsRunnable()) throw std::logic_error("Coroutine is currently waiting."); }



	typedef bool (CCoroutineBase::*T_IsValidFuncPtr)() const;
	operator T_IsValidFuncPtr () const {return IsRunnable() ? &CCoroutineBase::IsRunnable : NULL; }

protected:
	CCoroutineBase(CCoro* P_CoroPtr)
	:	m_CoroPtr(P_CoroPtr),
		m_bWaiting(false)
	{
	}

	void Init(CCoro* P_CoroPtr) { m_CoroPtr = P_CoroPtr; }

	template<class T>
	void Wait(CFuture<T>& P_Future)
	{
		P_Future.Invalidate();
		m_bWaiting = true;
		while(!P_Future)
			m_CoroPtr->yield();
		m_bWaiting = false;
	}

	template<class TP_List>
	int WaitSome(TP_List& P_Futures)
	{
		for(typename TP_List::iterator i = P_Futures.begin(); i != P_Futures.end(); ++i)
			i->Invalidate();
		m_bWaiting = true;
		while(true)
		{
			int W_iCount = 0;
			for(typename TP_List::iterator i = P_Futures.begin(); i != P_Futures.end(); ++i)
			{
				if(*i)
				{
					m_bWaiting = false;
					return W_iCount;
				}
				++W_iCount;
			}
			m_CoroPtr->yield();
		}
	}

	template <class TP_Derived>
	class selfBase
	{
	public:
		template<class T>
		void Wait(CFuture<T>& P_Future)
		{
			((TP_Derived*)this)->GetThisPtr()->Wait(P_Future);
		}

		template<class TP_List>
		int WaitSome(TP_List& P_Futures)
		{
			return ((TP_Derived*)this)->GetThisPtr()->Wait(P_Futures);
		};
	};

private:
	CCoroutineBase(const CCoroutineBase&);
	CCoroutineBase& operator=(const CCoroutineBase&);

protected:
	CCoro*	m_CoroPtr;
	bool	m_bWaiting;
};


template<class TP_Out, class TP_In>
class CCoroutine : public CCoroutineBase
{
public:

	class self : public selfBase<self>
	{
	public:
		self(CCoroutine* P_ThisPtr):m_ThisPtr(P_ThisPtr){}

		TP_In yield(const TP_Out& P_Out)
		{
			m_ThisPtr->m_Out(P_Out);
			Coro::yield();
			if(!m_ThisPtr->m_In)
				throw std::logic_error("Unexpected yield to this coro.");
			return *m_ThisPtr->m_In;
		}

		CCoroutine* GetThisPtr(){return m_ThisPtr;}

		TP_In result(){return *m_thisPtr->m_In;}

	private:
		CCoroutine* m_ThisPtr;
	};

	typedef std::tr1::function<void (self&, TP_In)> T_Func;

	template<class TP_Cb>
	CCoroutine(const TP_Cb& P_Cb)
	:	CCoroutineBase(NULL)
	{
		Init(P_Cb);
	}


	TP_Out operator()(const TP_In& P_In)
	{
		EnsureValid();
		m_In(P_In);
		m_CoroPtr->yield();
		if(m_bWaiting)
			throw CWaitingException();
		if(!m_Out)
			throw std::logic_error("No return value received.");
		return *m_Out;
	}

	void operator()(const TP_In& P_In, std::nothrow_t)
	{
		try
		{
			(*this)(P_In);
		}
		catch(CWaitingException&)
		{//Ignore this because of nothrow
		}
	}

	template<class TP_Cb>
	void Init(const TP_Cb& P_Cb)
	{
		delete m_CoroPtr;
		m_Func = P_Cb;
		CCoroutineBase::Init(Create(std::tr1::bind(&CCoroutine::Start, this)));
	}

private:
	void Start()
	{
		m_Func(self(this), *m_In);
	}



	T_Func	m_Func;

	COptional<TP_In>	m_In;
	COptional<TP_Out>	m_Out;
	bool				m_bInitialized;
};



template<class TP_Out>
class CCoroutine<TP_Out, void> : public CCoroutineBase
{
public:

	class self : public selfBase<self>
	{
	public:
		self(CCoroutine* P_ThisPtr):m_ThisPtr(P_ThisPtr){}

		void yield(const TP_Out& P_Out)
		{
			m_ThisPtr->m_Out(P_Out);
			Coro::yield();
		}

		CCoroutine* GetThisPtr(){return m_ThisPtr;}

	private:
		CCoroutine* m_ThisPtr;
	};

	typedef std::tr1::function<void (self&)> T_Func;

	template<class TP_Cb>
	CCoroutine(const TP_Cb& P_Cb)
	: CCoroutineBase(NULL)
	{
		Init(P_Cb);
	}


	TP_Out operator()()
	{
		EnsureValid();
		m_CoroPtr->yield();
		if(m_bWaiting)
			throw CWaitingException();
		if(!m_Out)
			throw std::logic_error("No return value received.");
		return *m_Out;
	}

	void operator()(std::nothrow_t)
	{
		try
		{
			(*this)();
		}
		catch(CWaitingException&)
		{//Ignore this because of nothrow
		}
	}

	template<class TP_Cb>
	void Init(const TP_Cb& P_Cb)
	{
		delete m_CoroPtr;
		m_Func = P_Cb;
		CCoroutineBase::Init(Create(std::tr1::bind(&CCoroutine::Start, this)));
	}

private:
	void Start()
	{
		m_Func(self(this));
	}



	T_Func	m_Func;

	COptional<TP_Out>	m_Out;
};



template<>
class CCoroutine<void, void> : public CCoroutineBase
{
public:

	class self : public selfBase<self>
	{
	public:
		self(CCoroutine* P_ThisPtr):m_ThisPtr(P_ThisPtr){}

		void yield()
		{
			Coro::yield();
		}

		CCoroutine* GetThisPtr(){return m_ThisPtr;}

	private:
		CCoroutine* m_ThisPtr;
	};

	typedef std::tr1::function<void (self&)> T_Func;

	template<class TP_Cb>
	CCoroutine(const TP_Cb& P_Cb)
	: CCoroutineBase(NULL)
	{
		Init(P_Cb);
	}


	void operator()()
	{
		EnsureValid();
		m_CoroPtr->yield();
		if(m_bWaiting)
			throw CWaitingException(); //For consistency.
	}

	void operator()(std::nothrow_t)
	{
		try
		{
			(*this)();
		}
		catch(CWaitingException&)
		{//Ignore this because of nothrow
		}
	}

	template<class TP_Cb>
	void Init(const TP_Cb& P_Cb)
	{
		delete m_CoroPtr;
		m_Func = P_Cb;
		CCoroutineBase::Init(Create(std::tr1::bind(&CCoroutine::Start, this)));
	}

private:
	void Start()
	{
		m_Func(self(this));
	}

	T_Func	m_Func;

};



}}//JStd, Coro