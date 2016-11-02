#pragma once


// CUdsCcDlg 对话框

class CUdsCcDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUdsCcDlg)

public:
	CUdsCcDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUdsCcDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CC };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CComboBox m_combccsub;
	CComboBox m_combcctype;
	CString m_CcResult;
	BOOL SetTim;

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButtonCc();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
