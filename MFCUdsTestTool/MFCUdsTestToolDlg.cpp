
// MFCUdsTestToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCUdsTestTool.h"
#include "MFCUdsTestToolDlg.h"
#include "afxdialogex.h"
#include "OpenDevDlg.h"
#include "ControlCAN.h"
#include "UdsUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned long nextrow;
UINT StopRecv = 0;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCUdsTestToolDlg 对话框



CMFCUdsTestToolDlg::CMFCUdsTestToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCUDSTESTTOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_CanRxEn = FALSE;
	m_CanExt = FALSE;
	m_CanRmt = FALSE;
}

void CMFCUdsTestToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CANMSG, m_list);
	DDX_Check(pDX, IDC_CHECK_RECV, m_CanRxEn);

	DDX_Text(pDX, IDC_EDIT_CANTX, m_Editcantx);
	DDX_Text(pDX, IDC_EDIT_CANID, m_EditCanid);

	//DDX_Radio(pDX, IDC_RADIO_EXT, m_CanExt);
	//DDX_Radio(pDX, IDC_RADIO_RMT, m_CanRmt);
}

BEGIN_MESSAGE_MAP(CMFCUdsTestToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_MENU_OPENDEV, &CMFCUdsTestToolDlg::OnMenuOpendev)
	ON_BN_CLICKED(IDC_CHECK_RECV, &CMFCUdsTestToolDlg::OnBnClickedCheckRecv)
	ON_BN_CLICKED(IDC_BUTTON_TX, &CMFCUdsTestToolDlg::OnBnClickedButtonTx)
END_MESSAGE_MAP()


// CMFCUdsTestToolDlg 消息处理程序

BOOL CMFCUdsTestToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//添加Menu
	m_Menu.LoadMenu(IDR_MENU1);
	SetMenu(&m_Menu);

	m_EditCanid = _T("766");
	GetDlgItem(IDC_EDIT_CANID)->SetWindowText(m_EditCanid);



	m_list.InsertColumn(0, _T("Seq"));
	m_list.SetColumnWidth(0, 40);
	m_list.InsertColumn(1, _T("Time"));
	m_list.SetColumnWidth(1, 85);
	m_list.InsertColumn(2, _T("CANIndex"));
	m_list.SetColumnWidth(2, 60);
	m_list.InsertColumn(3, _T("Tx-Rx"));
	m_list.SetColumnWidth(3, 60);
	m_list.InsertColumn(4, _T(" ID "));
	m_list.SetColumnWidth(4, 60);
	m_list.InsertColumn(5, _T("Frame"));
	m_list.SetColumnWidth(5, 50);
	m_list.InsertColumn(6, _T("Type"));
	m_list.SetColumnWidth(6, 70);
	m_list.InsertColumn(7, _T("DLC"));
	m_list.SetColumnWidth(7, 30);
	m_list.InsertColumn(8, _T("Data"));
	m_list.SetColumnWidth(8, 160);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCUdsTestToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCUdsTestToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCUdsTestToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCUdsTestToolDlg::OnMenuOpendev()
{
	// TODO: 在此添加命令处理程序代码
	COpenDevDlg  Dlg;
	Dlg.DoModal();
}


void CMFCUdsTestToolDlg::OnBnClickedCheckRecv()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_CanRxEn)
	{
		StopRecv = 0;
		//开启接收线程
		AfxBeginThread(ReceiveThread, 0);
	}
	else
		StopRecv = 1;
}



UINT CMFCUdsTestToolDlg::ReceiveThread(LPVOID v)
{
	CMFCUdsTestToolDlg *dlg = (CMFCUdsTestToolDlg*)AfxGetApp()->GetMainWnd();
	int k = 0;
	int NumValue;
	int i;
	VCI_CAN_OBJ pCanObj[200];

	CString strbuf[200], str1;
	int num = 0;

	CSize size;
	unsigned int JustnowItem;
	DWORD ReceivedID;

	size.cx = 0;
	size.cy = 50;
	CString str;
	int Len = 0;
	while (1)
	{

		//调用动态链接看接收函数
		NumValue = VCI_Receive(VCI_USBCAN2, CAN_DEVINDEX, theApp.m_CanChnl, pCanObj, 200, 0);
		//接收信息列表显示
		k++;
		CString strTime;
		SYSTEMTIME   systime;
		GetLocalTime(&systime);
		strTime.Format(_T("%02d:%02d:%02d:%03d"), systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);

		for (num = 0; num<NumValue; num++)
		{
			/*
			ReceivedID = pCanObj[num].ID;
			if (theApp.m_FilterEn)
			{
			if (ReceivedID < theApp.m_Bgnid || ReceivedID > theApp.m_Endid)
			break;
			}
			*/

			if (nextrow == 59999)
			{
				dlg->m_list.DeleteAllItems();
				nextrow = 0;
			}

			dlg->m_list.ItemColorFlag[nextrow] = 0;

			str.Format(_T("%d"), nextrow);
			JustnowItem = dlg->m_list.InsertItem(nextrow, str);
			nextrow++;

			dlg->m_list.SetItemText(JustnowItem, 1, strTime);

			str.Format(_T("%d"), theApp.m_CanChnl);
			dlg->m_list.SetItemText(JustnowItem, 2, str);

			dlg->m_list.SetItemText(JustnowItem, 3, _T("Receive"));
			str = _T("");

			if ((pCanObj[num].RemoteFlag) == 1)
			{
				dlg->m_list.SetItemText(JustnowItem, 5, _T("Remote"));
			}
			else
			{
				dlg->m_list.SetItemText(JustnowItem, 5, _T("Data"));

			}
			if ((pCanObj[num].ExternFlag) == 1)
			{
				ReceivedID = pCanObj[num].ID;
				str1.Format(_T("%08X"), ReceivedID);
				dlg->m_list.SetItemText(JustnowItem, 4, str1);	//ID信息	

				dlg->m_list.SetItemText(JustnowItem, 6, _T("Extended"));

			}
			else									//标准帧
			{
				ReceivedID = pCanObj[num].ID;
				str1.Format(_T("%04X"), ReceivedID);
				dlg->m_list.SetItemText(JustnowItem, 4, str1);	//ID信息	
				dlg->m_list.SetItemText(JustnowItem, 6, _T("Standard"));
			}
			str.Format(_T("%d"), pCanObj[num].DataLen);	//长度信息
			dlg->m_list.SetItemText(JustnowItem, 7, str);

			str = _T("");
			for (i = 0; i<(pCanObj[num].DataLen); i++)	//数据信息
			{
				str1.Format(_T("%02X "), pCanObj[num].Data[i]);
				str += str1;
			}

			dlg->m_list.SetItemText(JustnowItem, 8, str);
			dlg->m_list.Scroll(size);
			//接收信息列表显示完毕
		}

		Sleep(5);

		if (StopRecv == 1)
			return 0;
	}

	return 1;
}

void CMFCUdsTestToolDlg::OnBnClickedButtonTx()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);//更新控件数据

					 //从界面获取发送信息
	VCI_CAN_OBJ sendbuf[1];
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

	newflag = 1;

	temp_len = UdsUtil::str2char(m_Editcantx, temp_buf) - 1;

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
	sendbuf->ExternFlag = FrameType;
	sendbuf->DataLen = datanum;
	sendbuf->RemoteFlag = FrameFormat;
	if (FrameFormat == 1)//if remote frame, data area is invalid
		for (i = 0; i<datanum; i++)
			data_buf[i] = 0;

	sendbuf->ID = 0;
	sendbuf->ID |= (UINT)can_id[0] << 8;
	sendbuf->ID |= (UINT)can_id[1] << 0;


	for (i = 0; i<datanum; i++)
		sendbuf->Data[i] = data_buf[i];
	/****************************************************************************/
	/******************************从界面获取发送信息完毕***********************/
	/****************************************************************************/
	int flag;


	//调用动态链接库发送函数
	flag = VCI_Transmit(VCI_USBCAN2, CAN_DEVINDEX, theApp.m_CanChnl, sendbuf, 1);//CAN message send
	if (flag<1)
	{
		if (flag == -1)
			MessageBox(_T("failed- device not open\n"));
		else if (flag == 0)
			MessageBox(_T("send error\n"));
		return;

	}

	CSize size;
	unsigned int JustnowItem;
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
	str.Format(_T("%d"), nextrow);
	m_list.ItemColorFlag[nextrow] = 1;
	JustnowItem = m_list.InsertItem(nextrow, str);

	nextrow++;
	m_list.SetItemText(JustnowItem, 1, strTime);
	str.Format(_T("%d"), theApp.m_CanChnl);
	m_list.SetItemText(JustnowItem, 2, str);
	m_list.SetItemText(JustnowItem, 3, _T("Send"));
	str = _T("");

	if ((sendbuf->RemoteFlag) == 1)
	{
		m_list.SetItemText(JustnowItem, 5, _T("Remote"));
	}
	else
	{
		m_list.SetItemText(JustnowItem, 5, _T("Data"));
	}
	if ((sendbuf->ExternFlag) == 1)
	{
		for (i = 0; i<4; i++)
		{
			data = can_id[i];
			str1.Format(_T("%02X"), data);
			str += str1;
		}
		m_list.SetItemText(JustnowItem, 4, str);
		m_list.SetItemText(JustnowItem, 6, _T("Extended"));
	}
	else
	{
		for (i = 0; i<2; i++)
		{

			data = can_id[i];
			str1.Format(_T("%02X"), data);
			str += str1;
		}
		m_list.SetItemText(JustnowItem, 4, str);

		m_list.SetItemText(JustnowItem, 6, _T("Standard"));
	}
	str.Format(_T("%d"), sendbuf->DataLen);
	m_list.SetItemText(JustnowItem, 7, str);

	str = _T("");
	for (i = 0; i<sendbuf->DataLen; i++)
	{
		data = sendbuf->Data[i];
		str1.Format(_T("%02X"), data);
		str = (str + str1 + _T(" "));
	}
	m_list.SetItemText(JustnowItem, 8, str);
	m_list.Scroll(size);
	//发送信息列表显示完毕
}
