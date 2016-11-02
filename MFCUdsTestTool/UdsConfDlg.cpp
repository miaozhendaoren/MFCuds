// UdsConfDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCUdsTestTool.h"
#include "UdsConfDlg.h"
#include "afxdialogex.h"

#include "UdsUtil.h"


// CUdsConfDlg 对话框

IMPLEMENT_DYNAMIC(CUdsConfDlg, CDialogEx)

CUdsConfDlg::CUdsConfDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_UDSCONF, pParent)
{

}

CUdsConfDlg::~CUdsConfDlg()
{
}

void CUdsConfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_FUNCID, m_FuncId);
	DDX_Text(pDX, IDC_EDIT_PHYSID, m_PhysId);
	DDX_Text(pDX, IDC_EDIT_RSPID, m_RspnId);
	DDX_Text(pDX, IDC_EDIT_IDRST, m_IdResult);
}


BEGIN_MESSAGE_MAP(CUdsConfDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SAVEID, &CUdsConfDlg::OnBnClickedButtonSaveid)
END_MESSAGE_MAP()


// CUdsConfDlg 消息处理程序
BOOL CUdsConfDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_FuncId = FUNCID_STR;
	m_PhysId = PHYSID_STR;
	m_RspnId = RSPNID_STR;

	GetDlgItem(IDC_EDIT_FUNCID)->SetWindowText(m_FuncId);
	GetDlgItem(IDC_EDIT_PHYSID)->SetWindowText(m_PhysId);
	GetDlgItem(IDC_EDIT_RSPID)->SetWindowText(m_RspnId);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CUdsConfDlg::OnBnClickedButtonSaveid()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);//更新控件数据

	BYTE FuncId[10] = { 0 };
	BYTE PhysId[10] = { 0 };
	BYTE RspnId[10] = { 0 };
	BYTE temp_buf[50];
	LONG temp_len;

	temp_len = UdsUtil::str2char(m_FuncId, temp_buf) - 1;
	UdsUtil::str2HEX(temp_buf, FuncId);

	temp_len = UdsUtil::str2char(m_PhysId, temp_buf) - 1;
	UdsUtil::str2HEX(temp_buf, PhysId);

	temp_len = UdsUtil::str2char(m_RspnId, temp_buf) - 1;
	UdsUtil::str2HEX(temp_buf, RspnId);

	theApp.m_Fucid = (UINT)FuncId[0] << 8;
	theApp.m_Fucid |= (UINT)FuncId[1] << 0;

	theApp.m_Phyid = (UINT)PhysId[0] << 8;
	theApp.m_Phyid |= (UINT)PhysId[1] << 0;

	theApp.m_Rspid = (UINT)RspnId[0] << 8;
	theApp.m_Rspid |= (UINT)RspnId[1] << 0;

	m_IdResult = _T("");
	if (theApp.m_Fucid >= 0x800 || theApp.m_Phyid >= 0x800 || theApp.m_Rspid >= 0x800)
	{
		m_IdResult += _T("Fail,OutOfRange");
	}
	else
	{
		m_IdResult += _T("Success");
	}
	UpdateData(false);//更新控件数据
}

