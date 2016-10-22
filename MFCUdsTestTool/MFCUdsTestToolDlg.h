
// MFCUdsTestToolDlg.h : 头文件
//

#pragma once
#include "ColoredListCtrl.h"
#include "afxwin.h"

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
	CMenu m_Menu;
	CColoredListCtrl m_list;
	BOOL m_CanRxEn;

	CString m_Editcantx;
	CString m_EditCanid;

	BOOL m_CanExt;
	BOOL m_CanRmt;
public:
	afx_msg void OnMenuOpendev();

	afx_msg void OnBnClickedCheckRecv();

	//DECLARE_EVENTSINK_MAP()

	static UINT ReceiveThread(void *param);
	afx_msg void OnBnClickedButtonTx();
};
