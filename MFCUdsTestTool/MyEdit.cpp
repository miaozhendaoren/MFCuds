// MyEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCUdsTestTool.h"
#include "MyEdit.h"


// CMyEdit

IMPLEMENT_DYNAMIC(CMyEdit, CEdit)

CMyEdit::CMyEdit()
{

}

CMyEdit::~CMyEdit()
{
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()



// CMyEdit 消息处理程序




void CMyEdit::SetLimitLen(unsigned int nLimitLen)
{
	SetLimitText(nLimitLen);
}


void CMyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!(nChar >= _T('0') && nChar <= _T('9') || nChar >= _T('A') && nChar <= _T('F') || nChar >= _T('a') && nChar <= _T('f') || nChar == VK_BACK || nChar == VK_SPACE))
	{
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}
