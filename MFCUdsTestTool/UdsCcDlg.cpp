// UdsCcDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCUdsTestTool.h"
#include "UdsCcDlg.h"
#include "afxdialogex.h"

#include "UdsClient.h"


// CUdsCcDlg 对话框

IMPLEMENT_DYNAMIC(CUdsCcDlg, CDialogEx)

CUdsCcDlg::CUdsCcDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_CC, pParent)
{

}

CUdsCcDlg::~CUdsCcDlg()
{
}

void CUdsCcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_CCSUB, m_combccsub);
	DDX_Control(pDX, IDC_COMBO_CCTYPE, m_combcctype);

	DDX_Text(pDX, IDC_EDIT_CCRST, m_CcResult);
}


BEGIN_MESSAGE_MAP(CUdsCcDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CC, &CUdsCcDlg::OnBnClickedButtonCc)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CUdsCcDlg 消息处理程序


BOOL CUdsCcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_combccsub.SetCurSel(3); //预置 关闭接收和发送
	m_combcctype.SetCurSel(1);//预置 一般报文

	m_CcResult = _T("");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CUdsCcDlg::OnBnClickedButtonCc()
{
	// TODO: 在此添加控件通知处理程序代码
	BYTE CmdBuf[BUF_LEN];
	UINT CmdLen = 2;

	
	CmdBuf[0] = 0x03;
	theApp.UdsClient.request(SID_10, CmdBuf, 1);
	Sleep(50);
	//Communication Control
	CmdBuf[0] = m_combccsub.GetCurSel();
	CmdBuf[1] = m_combcctype.GetCurSel();

	theApp.UdsClient.request(SID_28, CmdBuf, CmdLen);

	SetTimer(1, 200, NULL);
	SetTim = TRUE;
}


void CUdsCcDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	BYTE DataBuf[BUF_LEN];
	UINT readlen;
	if (SetTim == TRUE)
	{
		SetTim = FALSE;
		readlen = theApp.UdsClient.get_rsp(DataBuf, BUF_LEN);
		m_CcResult = _T("");
		if (readlen > 0)
			m_CcResult += _T("Success");
		else
			m_CcResult += _T("Failed");

		UpdateData(false);//更新数据
			
	}

	CDialogEx::OnTimer(nIDEvent);
}
