#pragma once


// CUdsConfDlg 对话框

class CUdsConfDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUdsConfDlg)

public:
	CUdsConfDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUdsConfDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_UDSCONF };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CString m_FuncId;
	CString m_PhysId;
	CString m_RspnId;
	CString m_IdResult;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSaveid();

};
