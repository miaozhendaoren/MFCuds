// ColoredListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ColoredListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColoredListCtrl
CColoredListCtrl::CColoredListCtrl()
{
	m_colRow0 = RGB(200,200,250);
	m_colRow1 = RGB(230,247,247);
	m_colRow2 = RGB(192,192,192);
	m_colRow3 = RGB(0, 0, 0);
//	m_colRow1 = RGB(240,247,249);
//	m_colRow2 = RGB(229,232,239);
}

CColoredListCtrl::~CColoredListCtrl()
{
}


BEGIN_MESSAGE_MAP(CColoredListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CColoredListCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColoredListCtrl message handlers

void CColoredListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
  *pResult = 0;

  LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
  int iRow = lplvcd->nmcd.dwItemSpec;

  switch(lplvcd->nmcd.dwDrawStage)
  {
    case CDDS_PREPAINT :
    {
      *pResult = CDRF_NOTIFYITEMDRAW;
      return;
    }

    // Modify item text and or background
    case CDDS_ITEMPREPAINT:
    {
		if (ItemTextColor[iRow] == EmCol2)
		  lplvcd->clrText = m_colRow2;
		else
		  lplvcd->clrText = m_colRow3;
		// If you want the sub items the same as the item,
		// set *pResult to CDRF_NEWFONT

		switch (ItemColorFlag[iRow])
		{
		case EmCol0:
			lplvcd->clrTextBk = m_colRow0;
			break;
		case EmCol1:
			lplvcd->clrTextBk = m_colRow1;
			break;
		case EmCol2:
			lplvcd->clrTextBk = m_colRow2;
			break;
		default:
			break;
		}

      *pResult =CDRF_NOTIFYSUBITEMDRAW;
      return;
    }

    // Modify sub item text and/or background
    case CDDS_SUBITEM | CDDS_PREPAINT | CDDS_ITEM:
    {
	/*	//if(*(ItemColorFlag+nextrow)){
        if(ItemColorFlag[iRow]){
         lplvcd->clrTextBk = m_colRow2;
        }
        else{
          lplvcd->clrTextBk = m_colRow1;
        }
*/
        *pResult = CDRF_DODEFAULT;
        return;
    }
  }
  /*
 void CCoolList::OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult){//类型安全转换
NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
*pResult = 0;//指定列表项绘制前后发送消息
if(CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
{*pResult = CDRF_NOTIFYITEMDRAW;}
else if(CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
{//奇数行
	if(pLVCD->nmcd.dwItemSpec % 2)
		pLVCD->clrTextBk = RGB(255, 255, 128);//偶数行
	else 
		pLVCD->clrTextBk = RGB(128, 255, 255);//继续*pResult = CDRF_DODEFAULT;}}  
);//继续
*pResult = CDRF_DODEFAULT;}}

*/
}

BOOL CColoredListCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default

//return TRUE;

  CRect rect;
  CColoredListCtrl::GetClientRect(rect);


//  POINT mypoint;  
 
  CBrush brush1(m_colRow1);


 
 int chunk_height=GetCountPerPage();
 pDC->FillRect(&rect,&brush1);
/*
 for (int i=0;i<=chunk_height;i++)
 {
	GetItemPosition(i,&mypoint);
	rect.top=mypoint.y ;
	GetItemPosition(i+1,&mypoint);
	rect.bottom =mypoint.y;
	pDC->FillRect(&rect,i %2 ? &brush1 : &brush0);
 }*/

//  brush0.DeleteObject();
  brush1.DeleteObject();

  return FALSE;
}
