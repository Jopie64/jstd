#pragma once

#include "JBoxing.h"
// CBoxingDlg dialog
namespace JBoxing 
{ 

class IEventSync
{
public:
	virtual CWnd* GetParentWnd()=0;
};

//Derive a CWnd derived class also from this class
class CEventSyncWnd : public IEventSync
{
public:
	virtual CWnd* GetParentWnd(){return dynamic_cast<CWnd*>(this);}
};

class CMovableWndOwner : public CJRefCountDerivedTempl<CMovableWndOwner,CMovableWin> 
{
public:
	CMovableWndOwner(IEventSync* P_EventSyncPtr, std::string P_sId):m_EventSyncPtr(P_EventSyncPtr),m_sId(P_sId){}
	CMovableWndOwner(IEventSync* P_EventSyncPtr, std::string P_sId, CWnd* P_WndPtr, bool P_bFixedHSize, bool P_bFixedVSize)
	:	CJRefCountDerivedTempl<CMovableWndOwner,CMovableWin> (P_WndPtr,P_bFixedHSize,P_bFixedVSize),
		m_EventSyncPtr(P_EventSyncPtr),
		m_sId(P_sId)
	{
	}
	virtual ~CMovableWndOwner()
	{
		delete m_WndPtr;
	}

//	virtual void			GetMaxSize(CSize& P_Size_Max);
//	virtual void			GetMinSize(CSize& P_Size_Min);

	std::string m_sId;
	IEventSync* m_EventSyncPtr;
};

class CMovStatic : public CMovableWndOwner
{
public:
	CMovStatic(IEventSync* P_EventSyncPtr, std::string P_sId, std::string P_sText)
	:	CMovableWndOwner(P_EventSyncPtr,P_sId)
	{
		CWnd* W_ParentWndPtr=P_EventSyncPtr->GetParentWnd();
		CStatic* W_WndPtr=new CStatic();
		CRect W_Rect(CPoint(0,0),DetermineSize(W_ParentWndPtr,P_sText.c_str()));
		W_WndPtr->Create(P_sText.c_str(),WS_CHILD|WS_VISIBLE,W_Rect,W_ParentWndPtr,atoi(P_sId.c_str()));
		W_WndPtr->SetFont(W_ParentWndPtr->GetFont());
		Set(W_WndPtr,true,true);
	}

	CSize	DetermineSize(CWnd* P_WndPtr, CString P_csText)
	{
		CPaintDC W_Dc(P_WndPtr);
		CRect W_Rect;
		
		CFont* W_FontPtr=W_Dc.SelectObject(P_WndPtr->GetFont());
		W_Dc.DrawText(P_csText,&W_Rect,DT_CALCRECT);
		W_Dc.SelectObject(W_FontPtr);
		return W_Rect.Size();
	}

};
	
namespace Builder 
{
	class CWndBuilder : public CBuilder
	{
	public:
		CWndBuilder(IEventSync*	P_EventSyncPtr):m_EventSyncPtr(P_EventSyncPtr){}
		
		IEventSync*	m_EventSyncPtr;
	};

	class CWndElemBase : public CElemBase
	{
	public:
		virtual void AddTo(CBuilder& P_Builder)
		{
			AddTo(dynamic_cast<CWndBuilder&>(P_Builder));
		}
		virtual void AddTo(CWndBuilder& P_Builder)=0;
	};

	class Text : public CWndElemBase
	{
	public:
		Text(std::string P_sText, std::string P_sId=""):m_sText(P_sText),m_sId(P_sId){}
		virtual void AddTo(CWndBuilder& P_Builder)
		{
			P_Builder.AddMovable(new CMovStatic(P_Builder.m_EventSyncPtr,m_sId,m_sText));
		}
		std::string m_sText;
		std::string m_sId;

	};

}
};

class CBoxingDlg : public CDialog, public JBoxing::CEventSyncWnd
{
	DECLARE_DYNAMIC(CBoxingDlg)

public:
	CBoxingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBoxingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_BOXING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	JBoxing::CBox::TRefPtr m_MainBoxPtr;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
