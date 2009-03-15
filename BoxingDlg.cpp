// BoxingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "BoxingDlg.h"


namespace JBoxing
{

BEGIN_MESSAGE_MAP(CButton_Events, CButton)
//	ON_NOTIFY_REFLECT_EX(BN_CLICKED,OnClicked)
//	ON_NOTIFY_REFLECT_EX(BN_DOUBLECLICKED,OnDoubleClicked)
	ON_CONTROL_REFLECT_EX(BN_CLICKED,OnClicked)
	ON_CONTROL_REFLECT_EX(BN_DOUBLECLICKED,OnDoubleClicked)
END_MESSAGE_MAP()

/*
BOOL CButton_Events::OnClicked(NMHDR *,LRESULT *)
{
	Beep(1000,100);
	return TRUE;
}

BOOL CButton_Events::OnDoubleClicked(NMHDR *,LRESULT *)
{
	Beep(2000,100);
	return TRUE;
}
*/

BOOL CButton_Events::OnClicked()
{
	Beep(1000,100);
	return TRUE;
}

BOOL CButton_Events::OnDoubleClicked()
{
	Beep(2000,100);
	return TRUE;
}


};



// CBoxingDlg dialog

IMPLEMENT_DYNAMIC(CBoxingDlg, CDialog)

CBoxingDlg::CBoxingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBoxingDlg::IDD, pParent)
{

}

CBoxingDlg::~CBoxingDlg()
{
}

void CBoxingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBoxingDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CBoxingDlg message handlers


BOOL CBoxingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	using namespace JBoxing;
	using namespace JBoxing::Builder;
	CWndBuilder W_Builder(this);
	W_Builder
		<<VBox()
			<<Text("Dit is de titel van dit window...")
			<<Spacer()
			<<HBox()
				<<VBox()
					<<Text("Hoi")
					<<Text("Dag")
				<<Parent()
				<<Spacer()
				<<VBox()
					<<Text("Hihi")
					<<Text("Haha")
					<<Text("Hoho")
				<<Parent()
				<<Spacer()
				<<Button("Knoppie")
			<<Parent();

	m_MainBoxPtr=W_Builder.Box();

	CRect W_Rect;
	GetClientRect(W_Rect);
	m_MainBoxPtr->Move(W_Rect);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CBoxingDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect W_Rect;
	GetClientRect(W_Rect);

	if(m_MainBoxPtr!=NULL)
		m_MainBoxPtr->Move(W_Rect);
	Invalidate(FALSE);
}
