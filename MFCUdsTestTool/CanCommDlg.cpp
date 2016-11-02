// CanCommDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCUdsTestTool.h"
#include "MFCUdsTestToolDlg.h"
#include "CanCommDlg.h"
#include "afxdialogex.h"

#include "ControlCAN.h"

#include "UdsUtil.h"


INT nextrow = 0;
// CCanCommDlg 对话框

IMPLEMENT_DYNAMIC(CCanCommDlg, CDialogEx)

CCanCommDlg::CCanCommDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_COMM, pParent)
{
	m_CanRxEn = FALSE;
	m_CanExt = FALSE;
	m_CanRmt = FALSE;
}

CCanCommDlg::~CCanCommDlg()
{
}

void CCanCommDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_CANMSG, m_List);
	DDX_Check(pDX, IDC_CHECK_RECV, m_CanRxEn);

	DDX_Text(pDX, IDC_EDIT_CANTX, m_Editcantx);
	DDX_Text(pDX, IDC_EDIT_CANID, m_EditCanid);
}


BEGIN_MESSAGE_MAP(CCanCommDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_RECV, &CCanCommDlg::OnBnClickedCheckRecv)
	ON_BN_CLICKED(IDC_BUTTON_TX, &CCanCommDlg::OnBnClickedButtonTx)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CCanCommDlg::OnBnClickedButtonClear)
END_MESSAGE_MAP()


// CCanCommDlg 消息处理程序

BOOL CCanCommDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_EditCanid = _T("766");
	GetDlgItem(IDC_EDIT_CANID)->SetWindowText(m_EditCanid);



	m_List.InsertColumn(0, _T("Seq"));
	m_List.SetColumnWidth(0, 40);
	m_List.InsertColumn(1, _T("Time"));
	m_List.SetColumnWidth(1, 85);
	m_List.InsertColumn(2, _T("CANIndex"));
	m_List.SetColumnWidth(2, 60);
	m_List.InsertColumn(3, _T("Tx-Rx"));
	m_List.SetColumnWidth(3, 60);
	m_List.InsertColumn(4, _T(" ID "));
	m_List.SetColumnWidth(4, 60);
	m_List.InsertColumn(5, _T("Frame"));
	m_List.SetColumnWidth(5, 50);
	m_List.InsertColumn(6, _T("Type"));
	m_List.SetColumnWidth(6, 70);
	m_List.InsertColumn(7, _T("DLC"));
	m_List.SetColumnWidth(7, 30);
	m_List.InsertColumn(8, _T("Data"));
	m_List.SetColumnWidth(8, 160);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CCanCommDlg::OnBnClickedCheckRecv()
{
	// TODO: 在此添加控件通知处理程序代码
	/*
	UpdateData(TRUE);
	if (m_CanRxEn)
	{
	StopRecv = 0;
	//开启接收线程
	AfxBeginThread(ReceiveThread, 0);
	}
	else
	StopRecv = 1;
	*/
}

void CCanCommDlg::OnBnClickedButtonTx()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);//更新控件数据

					 //从界面获取发送信息
	VCI_CAN_OBJ SendObj;
	BYTE hex_str[32];
	BYTE hex_buf[50];
	BYTE data_buf[50];
	BYTE can_id[10] = { 0 };

	BYTE temp_char;
	BYTE temp_buf[50];
	LONG temp_len;

	int datanum = 0, IDnum = 0, newflag = 1, i;
	int FrameFormat, FrameType;

	if (m_CanExt == 0)
		FrameFormat = FRMFMT_STD;
	else
		FrameFormat = FRMFMT_EXT;

	if (m_CanRmt == 0)
		FrameType = FRMTYP_DAT;
	else
		FrameType = FRMTYP_RMT;

	temp_len = UdsUtil::str2char(m_EditCanid, temp_buf) - 1;
	UdsUtil::str2HEX(temp_buf, can_id);

	temp_len = UdsUtil::str2char(m_Editcantx, temp_buf) - 1;
	newflag = 1;
	for (i = 0; i<temp_len; i++)
	{
		temp_char = temp_buf[i];
		if (temp_char == ' ')
		{
			newflag = 1;
		}
		else
		{
			if (newflag == 1)
			{
				newflag = 0;
				hex_str[0] = temp_char;
				hex_str[1] = 0;
				hex_str[2] = 0;
			}
			else
			{
				newflag = 1;
				hex_str[1] = temp_char;
				hex_str[2] = 0;
			}

			if (newflag == 1 || temp_buf[i + 1] == ' ')
			{
				UdsUtil::str2HEX(hex_str, hex_buf);
				data_buf[datanum++] = hex_buf[0];
				if (datanum >= 8)
					break;
			}
		}
	}
	SendObj.ExternFlag = FrameType;
	SendObj.DataLen = datanum;
	SendObj.RemoteFlag = FrameFormat;
	if (FrameFormat == 1)//if remote frame, data area is invalid
		for (i = 0; i<datanum; i++)
			data_buf[i] = 0;

	SendObj.ID = 0;
	SendObj.ID |= (UINT)can_id[0] << 8;
	SendObj.ID |= (UINT)can_id[1] << 0;


	for (i = 0; i<datanum; i++)
		SendObj.Data[i] = data_buf[i];
	/****************************************************************************/
	/******************************从界面获取发送信息完毕***********************/
	/****************************************************************************/

	//调用动态链接库发送函数
	INT flag = CMFCUdsTestToolDlg::TransmitCanmsg(&SendObj);//CAN message send
	if (flag < 1)
	{
		if (flag == -1)
			MessageBox(_T("failed- device not open\n"));
		else if (flag == 0)
			MessageBox(_T("send error\n"));
		return;

	}

}

void CCanCommDlg::OnBnClickedButtonClear()
{
	// TODO: 在此添加控件通知处理程序代码
	m_List.DeleteAllItems();
	UpdateData(FALSE);//更新数据
}

// Insert a Can msg to list
void CCanCommDlg::InsertItem(INT Dire, VCI_CAN_OBJ * pCanObj)
{
	CSize size;
	UINT JustnowItem;
	BYTE data;

	//发送信息列表显示
	CString strTime;
	SYSTEMTIME   systime;
	GetLocalTime(&systime);
	strTime.Format(_T("%02d:%02d:%02d:%03d"), systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);


	size.cx = 0;
	size.cy = 50;
	CString str;
	CString str1;

	if (nextrow >= 59999)
	{
		m_List.DeleteAllItems();
		nextrow = 0;
	}

	str.Format(_T("%d"), nextrow);

	if (Dire == 1)
	    m_List.ItemColorFlag[nextrow] = 1;
	else
		m_List.ItemColorFlag[nextrow] = 0;

	JustnowItem = m_List.InsertItem(nextrow, str);

	nextrow++;
	m_List.SetItemText(JustnowItem, 1, strTime);
	str.Format(_T("%d"), theApp.m_CanChnl);
	m_List.SetItemText(JustnowItem, 2, str);
	if (Dire == 1)
	    m_List.SetItemText(JustnowItem, 3, _T("Send"));
	else
		m_List.SetItemText(JustnowItem, 3, _T("Recv"));
	
	str = _T("");
	if ((pCanObj->RemoteFlag) == 1)
	{
		m_List.SetItemText(JustnowItem, 5, _T("Remote"));
	}
	else
	{
		m_List.SetItemText(JustnowItem, 5, _T("Data"));
	}

	if ((pCanObj->ExternFlag) == 1)
	{
		str1.Format(_T("%08X"), pCanObj->ID);
		m_List.SetItemText(JustnowItem, 4, str1);	//ID信息

		m_List.SetItemText(JustnowItem, 6, _T("Extended"));

	}
	else									//标准帧
	{
		str1.Format(_T("%04X"), pCanObj->ID);
		m_List.SetItemText(JustnowItem, 4, str1);	//ID信息
		m_List.SetItemText(JustnowItem, 6, _T("Standard"));
	}

	str.Format(_T("%d"), pCanObj->DataLen);
	m_List.SetItemText(JustnowItem, 7, str);

	INT i;
	str = _T("");
	for (i = 0; i<pCanObj->DataLen; i++)
	{
		str1.Format(_T("%02X"), pCanObj->Data[i]);
		str = (str + str1 + _T(" "));
	}
	m_List.SetItemText(JustnowItem, 8, str);
	m_List.Scroll(size);
	//发送信息列表显示完毕
}
