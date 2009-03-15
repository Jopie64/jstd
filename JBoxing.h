#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>
#include "JStd.h"

namespace JBoxing
{

using namespace JStd;

class CBox;

class IMovable : public CJRefCountObjTempl<IMovable>
{
public:
	typedef boost::weak_ptr<CBox> CBoxWeakPtr;
	virtual ~IMovable(){}
	virtual	void			Move(CRect& P_Rect_Location)=0;
	virtual void			GetLocation(CRect& P_Rect_Location)=0;

	//0 means no limit
	virtual void			GetMaxSize(CSize& P_Size_Max){P_Size_Max=CSize(0,0);}
	virtual void			GetMinSize(CSize& P_Size_Min){P_Size_Min=CSize(0,0);}


	virtual CBoxWeakPtr			GetBox()=0;
protected:
	virtual void			SetBox(CBoxWeakPtr P_BoxPtr)=0;

	friend CBox;
};


class CMovableInBox : public CJRefCountDerivedTempl<CMovableInBox,IMovable>
{
public:
	CMovableInBox(){}//:m_BoxPtr(NULL){}
	virtual ~CMovableInBox();

	CBoxWeakPtr			Parent(){return GetBox();}

protected:
	virtual void			SetBox(CBoxWeakPtr P_BoxPtr){m_BoxPtr=P_BoxPtr;}

	virtual CBoxWeakPtr		GetBox();

	CBoxWeakPtr				m_BoxPtr;
};


class CMovable : public CJRefCountDerivedTempl<CMovable,CMovableInBox>
{
public:
	CMovable(void);
	virtual ~CMovable(void);

	virtual	void			Move(CRect& P_Rect_Location);
	virtual void			GetLocation(CRect& P_Rect_Location);


private:
	CRect m_Rect_Location;

};

class CSpacer : public CJRefCountDerivedTempl<CSpacer,CMovable>
{
public:
	CSpacer(int P_iSize=5):m_Size(P_iSize,P_iSize){}
	CSpacer(int P_iH, int P_iW):m_Size(P_iH,P_iW){}
	virtual void			GetMaxSize(CSize& P_Size_Max){P_Size_Max=m_Size;}
	virtual void			GetMinSize(CSize& P_Size_Min){P_Size_Min=m_Size;}
private:
	CSize m_Size;
};

//For debugging purposes
class CControl : public CJRefCountDerivedTempl<CControl,CSpacer>
{
public:
	CControl(int P_iSize=5, int P_iId=0):CJRefCountDerivedTempl<CControl,CSpacer>(P_iSize),m_iId(P_iId){}
	CControl(int P_iH, int P_iW, int P_iId=0):CJRefCountDerivedTempl<CControl,CSpacer>(P_iH,P_iW),m_iId(P_iId){}

	virtual	void Move(CRect& P_Rect_Location);

	int m_iId;
};

class CMovableWin : public CJRefCountDerivedTempl<CMovableWin,CMovableInBox>
{
public:
	CMovableWin(CWnd* P_WndPtr, bool P_bFixedHSize=true, bool P_bFixedVSize=true):m_WndPtr(P_WndPtr),m_bFixedHSize(P_bFixedHSize),m_bFixedVSize(P_bFixedVSize){}
	CMovableWin():m_WndPtr(NULL),m_bFixedHSize(false),m_bFixedVSize(false){}

	void					Set(CWnd* P_WndPtr, bool P_bFixedHSize=true, bool P_bFixedVSize=true)
	{
		m_WndPtr=P_WndPtr;
		m_bFixedHSize=P_bFixedHSize;
		m_bFixedVSize=P_bFixedVSize;
	}

	virtual	void			Move(CRect& P_Rect_Location);
	virtual void			GetLocation(CRect& P_Rect_Location);

	//0 means no limit
	virtual void			GetMaxSize(CSize& P_Size_Max);
	virtual void			GetMinSize(CSize& P_Size_Min);
	

	CWnd*	m_WndPtr;
	bool	m_bFixedHSize;
	bool	m_bFixedVSize;
};

typedef std::vector<IMovable::TRefPtr> CvMovable;

enum eBoxOrientation
{
	eBO_Horizontal,
	eBO_Vertical,
	eBO_Last
};

enum eBoxHAlign
{
	eBHA_Left,
	eBHA_Center,
	eBHA_Right,
	eBHA_Last
};

enum eBoxVAlign
{
	eBVA_Top,
	eBVA_Center,
	eBVA_Bottom,
	eBVA_Last
};


class CBox : public CJRefCountDerivedTempl<CBox,CMovable>, public boost::enable_shared_from_this<CBox>
{
public:
	virtual bool	AddMovable(IMovable::TRefPtr P_MovablePtr);
	virtual void	DelMovable(IMovable::TRefPtr P_MovablePtr);

protected:
	void	SetBoxMovable(IMovable::TRefPtr P_MovablePtr,bool P_bSet=true);
};

class CBoxWithMovables : public CJRefCountDerivedTempl<CBoxWithMovables,CBox>
{
public:
	virtual ~CBoxWithMovables(){Clear();}

	virtual bool	AddMovable(IMovable::TRefPtr P_MovablePtr);
	virtual void	DelMovable(IMovable::TRefPtr P_MovablePtr);
	void			Clear();

protected:
	CvMovable		m_vMovable;
};

class CHvBox : public CJRefCountDerivedTempl<CHvBox,CBoxWithMovables>
{
public:
	CHvBox():m_eOrientation(eBO_Horizontal),m_eHAlign(eBHA_Center),m_eVAlign(eBVA_Center),m_bHasMaximum(false){}
	virtual ~CHvBox(){}


	virtual	void	Move(CRect& P_Rect_Location);
	virtual void	GetMaxSize(CSize& P_Size_Max);
	virtual void	GetMinSize(CSize& P_Size_Min);

	void			SetHasMaximum(bool P_bSet=true){m_bHasMaximum=P_bSet;}
	void			SetOrientation(eBoxOrientation P_eOrientation, eBoxHAlign P_eHAlign=eBHA_Center, eBoxVAlign P_eVAlign=eBVA_Center)
	{
		m_eOrientation=P_eOrientation;
		m_eHAlign=P_eHAlign;
		m_eVAlign=P_eVAlign;
	}



private:
	//Checks if Size should be inverted according to orientation
	//(Inverted when vertical)
	bool			ShouldInvert();
	void			CheckInvertSize(CSize& P_Size);
	void			CheckInvertRect(CRect& P_Rect);

	//To retrieve eventually inverted HAlign and VAlign (inverted when orientation is vertical)
	eBoxHAlign		GetInvertedHAlign();
	eBoxVAlign		GetInvertedVAlign();

	//Returns P_fTrySize when between limits, or else one of the limits
	static double	GetOkSize(double P_fTrySize, int P_iMinSize, int P_iMaxSize);
	static double	GetOkSize(bool P_bX, double P_fTrySize, IMovable& P_Movable);//P_bX=true: P_iTrySize -> X, else P_iTrySize -> Y


	eBoxOrientation	m_eOrientation;
	eBoxHAlign		m_eHAlign;
	eBoxVAlign		m_eVAlign;
	bool			m_bHasMaximum;
};

class CGridBox : public CJRefCountDerivedTempl<CGridBox,CBox>
{
public:
	virtual ~CGridBox(){Clear();}

	virtual bool	AddMovable(IMovable::TRefPtr P_MovablePtr);
	virtual void	DelMovable(IMovable::TRefPtr P_MovablePtr);
	void			Clear();

protected:
	CvMovable		m_vMovable;
};

class COverlapBox : public CJRefCountDerivedTempl<COverlapBox,CBoxWithMovables>
{
public:
	COverlapBox(bool P_bHToSmallest=true, bool P_bVToSmallest=true):m_bHToSmallest(P_bHToSmallest),m_bVToSmallest(P_bVToSmallest){}

	virtual	void	Move(CRect& P_Rect_Location);
	virtual void	GetMaxSize(CSize& P_Size_Max);
	virtual void	GetMinSize(CSize& P_Size_Min);

private:
	bool m_bHToSmallest;
	bool m_bVToSmallest;
};

namespace Builder
{
	class CBuilder;
	class CElemBase
	{
	public:
		virtual void AddTo(CBuilder& P_Builder)=0;
	};

	class CBuilder
	{
	public:
		CBuilder(){}
		CBuilder(CBox::TRefPtr P_MainBoxPtr){AddBox(P_MainBoxPtr);}
		virtual ~CBuilder(){}

		CBuilder& AddBox(CBox::TRefPtr P_BoxPtr)
		{
			if(P_BoxPtr==NULL)
			{
				ASSERT(FALSE);
				return *this;
			}
			if(m_MainBoxPtr==NULL)
				m_MainBoxPtr=P_BoxPtr;
			else
			{
				ASSERT(m_CurBoxPtr!=NULL);
				if(m_CurBoxPtr==NULL)
					return *this;
				m_CurBoxPtr->AddMovable(P_BoxPtr);
			}
			m_CurBoxPtr=P_BoxPtr;
			return *this;
		}

		CBuilder& AddMovable(CMovableInBox::TRefPtr P_MovablePtr)
		{
			if(m_CurBoxPtr==NULL)
			{
				ASSERT(FALSE);
				return *this;
			}
			m_CurBoxPtr->AddMovable(P_MovablePtr);
			return *this;
		}

		CBuilder& Parent()
		{
			if(m_CurBoxPtr==NULL)
			{
				ASSERT(FALSE);
				return *this;
			}
			m_CurBoxPtr=CBox::TRefPtr(m_CurBoxPtr->Parent());
			return *this;
		}


		CBuilder& operator<<(CElemBase& P_Elem)
		{
			P_Elem.AddTo(*this);
			return *this;
		}

		CBox::TRefPtr	Box(){return m_MainBoxPtr;}

		CBox::TRefPtr	m_MainBoxPtr;
		CBox::TRefPtr	m_CurBoxPtr;

	};

	class HvBox : public CElemBase
	{
	public:
		HvBox(eBoxOrientation P_eOrientation, eBoxHAlign P_eHAlign,eBoxVAlign P_eVAlign):m_eOrientation(P_eOrientation),m_eHAlign(P_eHAlign),m_eVAlign(P_eVAlign),m_bHasMaximum(true){}
		HvBox(eBoxOrientation P_eOrientation, bool P_bHasMaximum):m_eOrientation(P_eOrientation),m_eHAlign(eBHA_Center),m_eVAlign(eBVA_Center),m_bHasMaximum(P_bHasMaximum){}

		virtual void AddTo(CBuilder& P_Builder)
		{
			CHvBox::TRefPtr W_BoxPtr(new CHvBox);
			W_BoxPtr->SetHasMaximum(m_bHasMaximum);
			W_BoxPtr->SetOrientation(m_eOrientation, m_eHAlign, m_eVAlign);
			P_Builder.AddBox(W_BoxPtr);
		}

		eBoxOrientation m_eOrientation;
		eBoxHAlign		m_eHAlign;
		eBoxVAlign		m_eVAlign;
		bool			m_bHasMaximum;
	};

	class Parent : public CElemBase
	{
	public:
		virtual void AddTo(CBuilder& P_Builder)
		{
			P_Builder.Parent();
		}
	};

	class VBox : public HvBox
	{
	public:
		VBox(eBoxHAlign P_eHAlign,eBoxVAlign P_eVAlign):HvBox(eBO_Vertical,P_eHAlign,P_eVAlign){}
		VBox(bool P_bHasMaximum=true):HvBox(eBO_Vertical,P_bHasMaximum){}
	};

	class HBox : public HvBox
	{
	public:
		HBox(eBoxHAlign P_eHAlign,eBoxVAlign P_eVAlign):HvBox(eBO_Horizontal,P_eHAlign,P_eVAlign){}
		HBox(bool P_bHasMaximum=true):HvBox(eBO_Horizontal,P_bHasMaximum){}
	};

	class OverlapBox : public CElemBase
	{
	public:
		OverlapBox(bool P_bHToSmallest=true, bool P_bVToSmallest=true):m_bHToSmallest(P_bHToSmallest),m_bVToSmallest(P_bVToSmallest){}
		bool m_bHToSmallest;
		bool m_bVToSmallest;

		virtual void AddTo(CBuilder& P_Builder)
		{
			P_Builder.AddBox(shared_new COverlapBox(m_bHToSmallest,m_bVToSmallest));
		}

	};

	class Window : public CElemBase
	{
	public:
		Window(CWnd* P_WndPtr, bool P_bFixedHSize=true, bool P_bFixedVSize=true):m_WndPtr(P_WndPtr),m_bFixedHSize(P_bFixedHSize), m_bFixedVSize(P_bFixedVSize){}
		virtual void AddTo(CBuilder& P_Builder)
		{
			P_Builder.AddMovable(shared_new CMovableWin(m_WndPtr,m_bFixedHSize,m_bFixedVSize));
		}

		CWnd*	m_WndPtr;
		bool	m_bFixedHSize;
		bool	m_bFixedVSize;
	};

	class DlgItem : public Window
	{
	public:
		DlgItem(CWnd* P_WndPtr, DWORD P_dwDlgItem, bool P_bFixedHSize=true, bool P_bFixedVSize=true)
		:	Window(P_WndPtr->GetDlgItem(P_dwDlgItem),P_bFixedHSize,P_bFixedVSize){}
	};

	class Spacer : public CElemBase
	{
	public:
		Spacer(int P_iH, int P_iW):m_iH(P_iH),m_iW(P_iW){}
		Spacer(int P_iSpace=5):m_iH(P_iSpace),m_iW(P_iSpace){}
		
		virtual void AddTo(CBuilder& P_Builder)
		{
			P_Builder.AddMovable(CMovableInBox::TRefPtr(new CSpacer(m_iH,m_iW)));
		}

		int m_iH;
		int m_iW;
	};

	class Control : public CElemBase
	{
	public:
		Control(int P_iH, int P_iW, int P_iId=0):m_iH(P_iH),m_iW(P_iW),m_iId(P_iId){}
		//Control(int P_iSpace=5, int P_iId=0):m_iH(P_iSpace),m_iW(P_iSpace),m_iId(P_iId){}
		
		virtual void AddTo(CBuilder& P_Builder)
		{
			P_Builder.AddMovable(shared_new CControl(m_iH,m_iW,m_iId));
		}

		int m_iId;
		int m_iH;
		int m_iW;
	};
}

//Derive a window with content to box from this class
class CBoxWnd
{
public:
	CBoxWnd(CRect P_Rect_EdgeOffsets=CRect(10,10,10,10)):m_Rect_EdgeOffsets(P_Rect_EdgeOffsets){}
	virtual ~CBoxWnd(){}

	void			Box_SetMainBox(CBox::TRefPtr P_MainBoxPtr);
	CBox::TRefPtr	Box_GetMainBox(){return m_MainBoxPtr;}

	void			Box_Move();

	Builder::Window	DlgItem(DWORD P_dwDlgItem, bool P_bFixedHSize=true, bool P_bFixedVSize=true);

private:
	CBox::TRefPtr	m_MainBoxPtr;
	CRect			m_Rect_EdgeOffsets;
};

}