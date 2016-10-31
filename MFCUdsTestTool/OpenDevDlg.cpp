// OpenDevDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCUdsTestTool.h"
#include "OpenDevDlg.h"
#include "afxdialogex.h"
#include "ControlCAN.h"
#include "UdsUtil.h"

// COpenDevDlg 对话框

IMPLEMENT_DYNAMIC(COpenDevDlg, CDialogEx)

COpenDevDlg::COpenDevDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_OPENDEV, pParent)
{
	theApp.m_FilterEn = TRUE;
}

COpenDevDlg::~COpenDevDlg()
{
}

void COpenDevDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CANCHNL, m_combchnl);
	DDX_Control(pDX, IDC_COMBO_CANBAUD, m_combbaud);

	DDX_Text(pDX, IDC_EDIT_BGNID, m_EditBgnid);
	DDX_Text(pDX, IDC_EDIT_ENDID, m_EditEndid);

	DDX_Check(pDX, IDC_CHECK_FILT, theApp.m_FilterEn);
}


BEGIN_MESSAGE_MAP(COpenDevDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BT_OPENDEV, &COpenDevDlg::OnBnClickedBtOpendev)
END_MESSAGE_MAP()


// COpenDevDlg 消息处理程序


BOOL COpenDevDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_combchnl.SetCurSel(0);//预置CAN1
	m_combbaud.SetCurSel(2);//预置500K


	m_EditBgnid = _T("700");
	m_EditEndid = _T("7FF");
	GetDlgItem(IDC_EDIT_BGNID)->SetWindowText(m_EditBgnid);
	GetDlgItem(IDC_EDIT_ENDID)->SetWindowText(m_EditEndid);

	m_EditExBgnid.SubclassWindow(GetDlgItem(IDC_EDIT_BGNID)->m_hWnd);
	m_EditExEndid.SubclassWindow(GetDlgItem(IDC_EDIT_ENDID)->m_hWnd);

	m_EditExBgnid.SetLimitLen(3);
	m_EditExEndid.SetLimitLen(3);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void COpenDevDlg::OnBnClickedBtOpendev()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);//更新控件数据

	CString baudrate;//定义字符串
	CString databit;
	BYTE id_bgn[10] = { 0 };
	BYTE id_end[10] = { 0 };
	BYTE temp_buf[50];
	LONG temp_len;
	UINT filter_code;

	if (m_combchnl.GetCurSel() == 0)
		theApp.m_CanChnl = CANCH_CAN1;
	else
		theApp.m_CanChnl = CANCH_CAN2;

	m_combbaud.GetLBText(m_combbaud.GetCurSel(), baudrate);//取得所选的字符串，并存放在baudrate里面


	temp_len = UdsUtil::str2char(m_EditBgnid, temp_buf) - 1;
	UdsUtil::str2HEX(temp_buf, id_bgn);

	temp_len = UdsUtil::str2char(m_EditEndid, temp_buf) - 1;
	UdsUtil::str2HEX(temp_buf, id_end);


	if (theApp.m_FilterEn)
	{
		UINT temp_id;
		theApp.m_Bgnid = (UINT)id_bgn[0] << 8;
		theApp.m_Bgnid |= (UINT)id_bgn[1] << 0;

		theApp.m_Endid = (UINT)id_end[0] << 8;
		theApp.m_Endid |= (UINT)id_end[1] << 0;

		if (theApp.m_Endid < theApp.m_Bgnid)
		{
			temp_id = theApp.m_Bgnid;
			theApp.m_Bgnid = theApp.m_Endid;
			theApp.m_Endid = temp_id;
		}
	}
	else
	{
		theApp.m_Bgnid = 0x000;
		theApp.m_Endid = 0x7FF;
	}

	filter_code = 0;
	filter_code = (theApp.m_Bgnid & 0x00000F00) << 21;
	filter_code |= (theApp.m_Endid & 0x00000F00) << 5;

	DWORD Reserved = 0;
	//打开设备
	if (VCI_OpenDevice(VCI_USBCAN2, CAN_DEVINDEX, Reserved) != 1)
	{
		MessageBox(_T("open failed"));
		return;
	}
	VCI_INIT_CONFIG InitInfo[1];
	InitInfo->Timing0 = 0x00;
	InitInfo->Timing1 = 0x1C;
	InitInfo->Filter = 0;
	InitInfo->AccCode = filter_code;
	InitInfo->AccMask = (~filter_code);
	InitInfo->Mode = 0;
	//初始化CAN1
	if (VCI_InitCAN(VCI_USBCAN2, CAN_DEVINDEX, theApp.m_CanChnl, InitInfo) != 1)
	{
		MessageBox(_T("Init-CAN1 failed!"));
		return;
	}
	Sleep(100);
	//初始化CAN1
	if (VCI_StartCAN(VCI_USBCAN2, CAN_DEVINDEX, theApp.m_CanChnl) != 1)
	{
		MessageBox(_T("Start-CAN1 failed!"));
		return;
	}

	MessageBox(_T("Open successful!\n Start CAN OK!"));
	EndDialog(0);

}
