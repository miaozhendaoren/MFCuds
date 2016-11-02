
// MFCUdsTestTool.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "UdsClient.h"

#define FUNCID 0x7DF
#define PHYSID 0x766
#define RSPNID 0x706

#define FUNCID_STR _T("0x7DF")
#define PHYSID_STR _T("0x766")
#define RSPNID_STR _T("0x706")

// CMFCUdsTestToolApp: 
// 有关此类的实现，请参阅 MFCUdsTestTool.cpp
//

class CMFCUdsTestToolApp : public CWinApp
{
public:
	CMFCUdsTestToolApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现
	UINT m_Fucid = FUNCID;
	UINT m_Phyid = PHYSID;
	UINT m_Rspid = RSPNID;

	UINT m_Bgnid;
	UINT m_Endid;
	BOOL m_FilterEn;
	UINT m_CanChnl;

	CUdsClient UdsClient;
	DECLARE_MESSAGE_MAP()
};

extern CMFCUdsTestToolApp theApp;