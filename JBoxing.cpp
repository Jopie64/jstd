#include "StdAfx.h"
#include "JBoxing.h"
#include <boost/foreach.hpp>


namespace JBoxing
{

CMovableInBox::~CMovableInBox()
{
}


CMovable::CMovable(void)
{
}

CMovable::~CMovable(void)
{
}

void CMovable::Move(CRect& P_Rect_Location)
{
	m_Rect_Location=P_Rect_Location;
}

void CMovable::GetLocation(CRect& P_Rect_Location)
{
	P_Rect_Location=m_Rect_Location;
}


CBox::TWeakPtr CMovableInBox::GetBox()
{

	return m_BoxPtr;
}



void CMovableWin::GetMaxSize(CSize& P_Size_Max)
{
//	if(!m_bFixedSize)
//	{
//		P_Size_Max=CSize(0,0);
//		return;
//	}
	CRect W_Rect_Window;
	m_WndPtr->GetWindowRect(W_Rect_Window);
	P_Size_Max=W_Rect_Window.Size();
	if(!m_bFixedHSize)
		P_Size_Max.cx=0;
	if(!m_bFixedVSize)
		P_Size_Max.cy=0;
}

void CMovableWin::GetMinSize(CSize& P_Size_Min)
{
	GetMaxSize(P_Size_Min);
}

void CMovableWin::Move(CRect& P_Rect_Location)
{
	m_WndPtr->MoveWindow(P_Rect_Location);
}

void CMovableWin::GetLocation(CRect& P_Rect_Location)
{
	m_WndPtr->GetWindowRect(P_Rect_Location);
}


void CControl::Move(CRect& P_Rect_Location)
{
	CSize W_Size=P_Rect_Location.Size();
	TRACE("Move %d: Size: %d,%d - Pos:(%2d,%2d) (%2d,%2d)\n",
		m_iId,
		W_Size.cx,
		W_Size.cy,
		P_Rect_Location.left,
		P_Rect_Location.top,
		P_Rect_Location.right,
		P_Rect_Location.bottom);
}


// ***** Box

void CBox::SetBoxMovable(IMovable::TRefPtr P_MovablePtr, bool P_bSet)
{
	P_MovablePtr->SetBox(P_bSet?shared_from_this():CBoxWeakPtr());
}

bool CBox::AddMovable(IMovable::TRefPtr P_MovablePtr)
{
//	if(CBox::TRefPtr(P_MovablePtr->GetBox()))
	if(!P_MovablePtr->GetBox().expired())
		return false;//Already added somewhere
	SetBoxMovable(P_MovablePtr,true);
	return true;
}

void CBox::DelMovable(IMovable::TRefPtr P_MovablePtr)
{
	SetBoxMovable(P_MovablePtr,false);
}

void CBoxWithMovables::Clear()
{
	for(CvMovable::iterator W_itMovable=m_vMovable.begin();W_itMovable!=m_vMovable.end();++W_itMovable)
		SetBoxMovable(*W_itMovable,false);

	m_vMovable.clear();
}




bool CBoxWithMovables::AddMovable(IMovable::TRefPtr P_MovablePtr)
{
	if(!CBox::AddMovable(P_MovablePtr))
		return false;
	m_vMovable.push_back(P_MovablePtr);
	return true;
}

void CBoxWithMovables::DelMovable(IMovable::TRefPtr P_MovablePtr)
{
	CBox::DelMovable(P_MovablePtr);
	for(CvMovable::iterator W_itMovable=m_vMovable.begin();W_itMovable!=m_vMovable.end();++W_itMovable)
		if(*W_itMovable==P_MovablePtr)
		{
			m_vMovable.erase(W_itMovable);
			return;
		}
}


void CHvBox::CheckInvertSize(CSize& P_Size)
{
	if(ShouldInvert())
		P_Size=CSize(P_Size.cy,P_Size.cx);
}

void CHvBox::CheckInvertRect(CRect& P_Rect)
{
	if(ShouldInvert())
	{
		std::swap(P_Rect.left,P_Rect.top);
		std::swap(P_Rect.right,P_Rect.bottom);
	}
}


double CHvBox::GetOkSize(double P_fTrySize, int P_iMinSize, int P_iMaxSize)
{
	if(P_iMinSize>0&&P_fTrySize<P_iMinSize)
		return P_iMinSize;
	if(P_iMaxSize>0&&P_fTrySize>P_iMaxSize)
		return P_iMaxSize;
	return P_fTrySize;
}

double CHvBox::GetOkSize(bool P_bX, double P_fTrySize, IMovable& P_Movable)
{
	CSize W_Size;
	P_Movable.GetMinSize(W_Size);
	int W_iMinSize=P_bX?W_Size.cx:W_Size.cy;
	P_Movable.GetMaxSize(W_Size);
	int W_iMaxSize=P_bX?W_Size.cx:W_Size.cy;
	return GetOkSize(P_fTrySize,W_iMinSize,W_iMaxSize);
}

bool CHvBox::ShouldInvert()
{
	return m_eOrientation==eBO_Vertical;
}

eBoxHAlign CHvBox::GetInvertedHAlign()
{
	if(!ShouldInvert())
		return m_eHAlign;
	switch(m_eVAlign)
	{
	case eBVA_Top:		return eBHA_Left;
	case eBVA_Center:	return eBHA_Center;
	case eBVA_Bottom:	return eBHA_Right;
	}
	return eBHA_Last;
}

eBoxVAlign CHvBox::GetInvertedVAlign()
{
	if(!ShouldInvert())
		return m_eVAlign;
	switch(m_eHAlign)
	{
	case eBHA_Left:		return eBVA_Top;
	case eBHA_Center:	return eBVA_Center;
	case eBHA_Right:	return eBVA_Bottom;
	}
	return eBVA_Last;
}


void CHvBox::GetMaxSize(CSize& P_Size_Max)
{
	P_Size_Max=CSize(0,0);
	if(!m_bHasMaximum)
		return;

	bool W_b_cx_Undetermined=false;
	bool W_b_cy_Undetermined=false;

	for(int i=0;i<m_vMovable.size();++i)
	{
		CSize W_Size_MaxMovable;
		m_vMovable[i]->GetMaxSize(W_Size_MaxMovable);
		CheckInvertSize(W_Size_MaxMovable);
		if(W_Size_MaxMovable.cx==0)
			W_b_cx_Undetermined=true;
		if(W_Size_MaxMovable.cy==0)
			W_b_cy_Undetermined=true;
		P_Size_Max.cx+=W_Size_MaxMovable.cx;
		if(W_Size_MaxMovable.cy>P_Size_Max.cy)
			P_Size_Max.cy=W_Size_MaxMovable.cy;
	}
	if(W_b_cx_Undetermined)
		P_Size_Max.cx=0;
	if(W_b_cy_Undetermined)
		P_Size_Max.cy=0;
	CheckInvertSize(P_Size_Max);
}

void CHvBox::GetMinSize(CSize& P_Size_Min)
{
	P_Size_Min=CSize(0,0);
	for(int i=0;i<m_vMovable.size();++i)
	{
		CSize W_Size_MinMovable;
		m_vMovable[i]->GetMinSize(W_Size_MinMovable);
		CheckInvertSize(W_Size_MinMovable);
		P_Size_Min.cx+=W_Size_MinMovable.cx;
		if(W_Size_MinMovable.cy>P_Size_Min.cy)
			P_Size_Min.cy=W_Size_MinMovable.cy;
	}
	CheckInvertSize(P_Size_Min);
}


class CSizeF
{
public:
	CSizeF(){}
	CSizeF(double P_cx, double P_cy):cx(P_cx),cy(P_cy){}
	CSizeF(CSize& P_Size):cx(P_Size.cx),cy(P_Size.cy){}

	double cx;
	double cy;
};

typedef std::vector<CSizeF> CvSizeF;

void CHvBox::Move(CRect& P_Rect_Location)
{
	CMovable::Move(P_Rect_Location);

	if(m_vMovable.empty())
		//Nothing to do
		return;

	//Here the relocate controls job is done

	//Initialize
	CRect W_Rect_Loc;
	GetLocation(W_Rect_Loc);

	CSize W_Size_Box;
	W_Size_Box=W_Rect_Loc.Size();
	CheckInvertSize(W_Size_Box);

	bool W_bHorizontal = m_eOrientation==eBO_Horizontal;

	//Now move all movables
	int W_iMovableCount=m_vMovable.size();
	CvSizeF	W_vUsedSize(W_iMovableCount,CSizeF(0,0));

	//Left in de zin van 'over' (niet 'links')
	double W_fBoxSizeLeft=W_Size_Box.cx;
	int W_iMovableCountLeft=W_iMovableCount;
	double W_fAvgSize=W_fBoxSizeLeft/W_iMovableCount;

	bool W_bAllOk;

	//** Determine new movable sizes
	//First the vertical sizes
	int i;
	for(i=0;i<W_iMovableCount;++i)
		W_vUsedSize[i].cy=GetOkSize(!W_bHorizontal,W_Size_Box.cy,*m_vMovable[i]);

/*
	//Dit blijkt er alleen voor te zorgen dat bij kleiner dan minimale breedte hij altijd links uitlijnt
	do
	{
		W_bAllOk=true;
		for(i=0;i<W_iMovableCount;++i)
		{
			double W_fOkSize=W_vUsedSize[i].cy=GetOkSize(!W_bHorizontal,W_Size_Box.cy,*m_vMovable[i]);
			if(W_fOkSize>W_Size_Box.cy)
			{
				W_Size_Box.cy=W_fOkSize;
				W_bAllOk=false;
				break;
			}
		}
	}while(!W_bAllOk);
*/

	//Then the horizontal sizes
	for(int W_iDoMin=0;W_iDoMin<2;++W_iDoMin)
	{
		//bij W_iDoMin=0, alleen rekening houden met maximum waardes
		//Bij W_iDoMin=1, ook minimum waardes
		do
		{
			W_bAllOk=true;
			for(int i=0;i<W_iMovableCount;++i)
			{
				if(W_vUsedSize[i].cx==0)
				{
					double W_fSize=GetOkSize(W_bHorizontal,W_fAvgSize,*m_vMovable[i]);
					if( (W_iDoMin==0 && W_fSize< W_fAvgSize) ||
						(W_iDoMin==1 && W_fSize!=W_fAvgSize) )
					{
						//W_fAvgSize is not accepted for this movable. Set to acceptable size.
						W_vUsedSize[i].cx=W_fSize;
						W_fBoxSizeLeft-=W_fSize;
						if(W_fBoxSizeLeft<0)
							W_fBoxSizeLeft=0;//Dont allow negative sizes
						--W_iMovableCountLeft;
						if(W_iMovableCountLeft>0)
							W_fAvgSize=W_fBoxSizeLeft/W_iMovableCountLeft;
						W_bAllOk=false;
						break;
					}
				}
			}
		}while(!W_bAllOk&&W_iMovableCountLeft>0);
	}
	
	//Set rest of horizontal sizes to W_fAvgSize and determine total width
	double W_fWidth=0;
	for(i=0;i<W_iMovableCount;++i)
	{
		if(W_vUsedSize[i].cx==0)
			W_vUsedSize[i].cx=W_fAvgSize;
		W_fWidth+=W_vUsedSize[i].cx;
	}


	//Determine left
	eBoxHAlign W_eHAlign=GetInvertedHAlign();
	eBoxVAlign W_eVAlign=GetInvertedVAlign();

	double W_fLeft;//'Links' (niet 'over')
	
	switch(W_eHAlign)
	{
	case eBHA_Left:	W_fLeft=0;break;
	case eBHA_Center: W_fLeft=(W_Size_Box.cx-W_fWidth)/2;break;
	case eBHA_Right: W_fLeft=W_Size_Box.cx-W_fWidth;break;
	}

	for(i=0;i<W_iMovableCount;++i)
	{
		//Determine top current movable
		double W_fTop;
		switch(W_eVAlign)
		{
		case eBVA_Top: W_fTop=0;break;
		case eBVA_Center: W_fTop=(W_Size_Box.cy-W_vUsedSize[i].cy)/2;break;
		case eBVA_Bottom: W_fTop=W_Size_Box.cy-W_vUsedSize[i].cy;break;
		}

		//Determine rect current movable
		CRect W_Rect_Movable(
			CPoint((int)W_fLeft,(int)W_fTop),
			CSize((int)W_vUsedSize[i].cx,(int)W_vUsedSize[i].cy));

		CheckInvertRect(W_Rect_Movable);

		W_Rect_Movable.OffsetRect(W_Rect_Loc.TopLeft());//Make relative to box's own parent

		//Move
		m_vMovable[i]->Move(W_Rect_Movable);

		//Determine next leftside
		W_fLeft+=W_vUsedSize[i].cx;
	}
}

//**** COverlapBox

void COverlapBox::GetMaxSize(CSize& P_Size_Max)
{
	if(m_bHToSmallest||m_bVToSmallest)
	{
		//De grootste van de kleinste
		GetMinSize(P_Size_Max);

		if(m_bHToSmallest&&m_bVToSmallest)
			return;
	}
	else
		P_Size_Max=CSize(0,0);
	//Zoek de grootste van de grootste
//	J_FOREACH(CvMovable,m_vMovable)
	BOOST_FOREACH(IMovable::TRefPtr& W_MovablePtr, m_vMovable)
	{
		CSize W_Size_Cur;
		W_MovablePtr->GetMaxSize(W_Size_Cur);
		if(!m_bHToSmallest)
		{
			if(P_Size_Max.cx==0||W_Size_Cur.cx==0)	P_Size_Max.cx=0;
			else if(P_Size_Max.cx<W_Size_Cur.cx)	P_Size_Max.cx=W_Size_Cur.cx;
		}
		if(!m_bVToSmallest)
		{
			if(P_Size_Max.cy==0||W_Size_Cur.cy==0)	P_Size_Max.cy=0;
			else if(P_Size_Max.cy<W_Size_Cur.cy)	P_Size_Max.cy=W_Size_Cur.cy;
		}
	}

}

void COverlapBox::GetMinSize(CSize& P_Size_Min)
{
	P_Size_Min=CSize(0,0);
	//Zoek de grootste van de kleinste
	BOOST_FOREACH(IMovable::TRefPtr& W_MovablePtr, m_vMovable)
	{
		CSize W_Size_Cur;
		W_MovablePtr->GetMinSize(W_Size_Cur);
		if(P_Size_Min.cx<W_Size_Cur.cx)		P_Size_Min.cx=W_Size_Cur.cx;
		if(P_Size_Min.cy<W_Size_Cur.cy)		P_Size_Min.cy=W_Size_Cur.cy;
	}
}

void COverlapBox::Move(CRect& P_Rect_Location)
{
	BOOST_FOREACH(IMovable::TRefPtr& W_MovablePtr, m_vMovable)
	{
		//Todo: Zo aanpassen dat de size van de rect binnen de minimale en maximale size blijft.
		//Workaround: Voorlopig simpel op te lossen door objecten binnen een HvBox te zetten.
		W_MovablePtr->Move(P_Rect_Location);
	}
}


//**** CBoxWnd

void CBoxWnd::Box_SetMainBox(CBox::TRefPtr P_MainBoxPtr)
{
	m_MainBoxPtr=P_MainBoxPtr;
	Box_Move();
}

void CBoxWnd::Box_Move()
{
	if(m_MainBoxPtr==NULL)
		return;
	CRect W_Rect_Client;
	CWnd* W_WndPtr=dynamic_cast<CWnd*>(this);
	ASSERT(W_WndPtr!=NULL);//This class should be inherited by a CWnd (derived) class
	if(W_WndPtr==NULL)
		return;
	W_WndPtr->GetClientRect(W_Rect_Client);
	W_Rect_Client.DeflateRect(
		m_Rect_EdgeOffsets.left,
		m_Rect_EdgeOffsets.top,
		m_Rect_EdgeOffsets.right,
		m_Rect_EdgeOffsets.bottom);
	m_MainBoxPtr->Move(W_Rect_Client);

	W_WndPtr->Invalidate(FALSE);//Is er een andere manier zonder knipperen?
}

Builder::Window	CBoxWnd::DlgItem(DWORD P_dwDlgItem, bool P_bFixedHSize, bool P_bFixedVSize)
{
	CWnd* W_WndPtr=dynamic_cast<CWnd*>(this);
	ASSERT(W_WndPtr!=NULL);//This class should be inherited by a CWnd (derived) class
	if(W_WndPtr==NULL)
		return Builder::Window(NULL);
	return Builder::Window(W_WndPtr->GetDlgItem(P_dwDlgItem),P_bFixedHSize,P_bFixedVSize);
}


}
