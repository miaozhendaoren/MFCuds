
// MFCUdsTestToolDlg.h : 头文件
//

#pragma once
#include "ColoredListCtrl.h"
#include "afxwin.h"
#include "CanCommDlg.h"
#include "UdsDiagDlg.h"
#include "UdsCcDlg.h"

#include "ControlCAN.h"

// CMFCUdsTestToolDlg 对话框
class CMFCUdsTestToolDlg : public CDialogEx
{

// 构造
public:
	CMFCUdsTestToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCUDSTESTTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CTabCtrl m_Table;
	CMenu m_Menu;
	CColoredListCtrl *m_List;

	int m_CurSelTab;
	CCanCommDlg m_CanComm;
	CUdsDiagDlg m_UdsDiag;
	CDialog* pDialog[2];  //用来保存对话框对象指针

public:
	static INT TransmitCanmsg(VCI_CAN_OBJ *SendObj);
	static UINT UdsMainThread(void *param);
	static UINT ReceiveThread(void *param);

	afx_msg void OnBnClickedBtOpendev();
	afx_msg void OnMenuOpendev();
	afx_msg void OnMenuClosedev();
	afx_msg void OnMenuRdid();
	//DECLARE_EVENTSINK_MAP()
	afx_msg void OnMenuWdid();
	afx_msg void OnMenuReset();
	afx_msg void OnMenuDtcon();
	afx_msg void OnMenuDtcoff();
	afx_msg void OnMenuRddtc();
	afx_msg void OnMenuCrdtc();
	afx_msg void OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMenuCc();
	afx_msg void OnMenuUdsconf();
};
