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

static CSize DetermineTextSize(CWnd* P_WndPtr, CString P_csText)
{
	CPaintDC W_Dc(P_WndPtr);
	CRect W_Rect;
	
	CFont* W_FontPtr=W_Dc.SelectObject(P_WndPtr->GetFont());
	W_Dc.DrawText(P_csText,&W_Rect,DT_CALCRECT);
	W_Dc.SelectObject(W_FontPtr);
	return W_Rect.Size();
}

class CMovStatic : public CMovableWndOwner
{
public:
	CMovStatic(IEventSync* P_EventSyncPtr, std::string P_sId, std::string P_sText)
	:	CMovableWndOwner(P_EventSyncPtr,P_sId)
	{
		CWnd* W_ParentWndPtr=P_EventSyncPtr->GetParentWnd();
		CStatic* W_WndPtr=new CStatic();
		CRect W_Rect(CPoint(0,0),DetermineTextSize(W_ParentWndPtr,P_sText.c_str()));
		W_WndPtr->Create(P_sText.c_str(),WS_CHILD|WS_VISIBLE,W_Rect,W_ParentWndPtr,atoi(P_sId.c_str()));
		W_WndPtr->SetFont(W_ParentWndPtr->GetFont());
		Set(W_WndPtr,true,true);
	}

};


class CButton_Events : public CButton
{
public:
	CButton_Events(){}
	virtual ~CButton_Events(){}

	DECLARE_MESSAGE_MAP()

private:
//	afx_msg BOOL OnClicked(NMHDR *,LRESULT *);
//	afx_msg BOOL OnDoubleClicked(NMHDR *,LRESULT *);
	afx_msg BOOL OnClicked();
	afx_msg BOOL OnDoubleClicked();

};

class CMovButton : public CMovableWndOwner
{
public:
	CMovButton(IEventSync* P_EventSyncPtr, std::string P_sId, std::string P_sText)
	:	CMovableWndOwner(P_EventSyncPtr,P_sId)
	{
		CWnd* W_ParentWndPtr=P_EventSyncPtr->GetParentWnd();
		CButton_Events* W_WndPtr=new CButton_Events();
		CSize W_ButSize=DetermineTextSize(W_ParentWndPtr,P_sText.c_str());
		W_ButSize.cx+=10;
		W_ButSize.cy+=10;
		CRect W_Rect(CPoint(0,0),W_ButSize);
		W_WndPtr->Create(P_sText.c_str(),WS_CHILD|WS_VISIBLE,W_Rect,W_ParentWndPtr,atoi(P_sId.c_str()));
		W_WndPtr->SetFont(W_ParentWndPtr->GetFont());
		Set(W_WndPtr,true,true);
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
			P_Builder.AddMovable(shared_new CMovStatic(P_Builder.m_EventSyncPtr,m_sId,m_sText));
		}
		std::string m_sText;
		std::string m_sId;

	};

	class Button : public CWndElemBase
	{
	public:
		Button(std::string P_sText, std::string P_sId=""):m_sText(P_sText),m_sId(P_sId){}
		virtual void AddTo(CWndBuilder& P_Builder)
		{
			P_Builder.AddMovable(shared_new CMovButton(P_Builder.m_EventSyncPtr,m_sId,m_sText));
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
	enum { IDD = IDD_JBOXING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	JBoxing::CBox::TRefPtr m_MainBoxPtr;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
