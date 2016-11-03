
// MFCUdsTestToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCUdsTestTool.h"
#include "MFCUdsTestToolDlg.h"
#include "afxdialogex.h"
#include "ControlCAN.h"
#include "UdsUtil.h"
#include "UdsClient.h"

#include "OpenDevDlg.h"
#include "RdWrDidDlg.h"
#include "UdsDiagDlg.h"
#include "CanCommDlg.h"
#include "UdsConfDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
}

void CMFCUdsTestToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MAIN, m_Table);

	//DDX_Radio(pDX, IDC_RADIO_EXT, m_CanExt);
	//DDX_Radio(pDX, IDC_RADIO_RMT, m_CanRmt);
}

BEGIN_MESSAGE_MAP(CMFCUdsTestToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(IDC_BT_OPENDEV, &CMFCUdsTestToolDlg::OnBnClickedBtOpendev)
	ON_COMMAND(ID_MENU_OPENDEV, &CMFCUdsTestToolDlg::OnMenuOpendev)
	ON_COMMAND(ID_MENU_CLOSEDEV, &CMFCUdsTestToolDlg::OnMenuClosedev)
	ON_COMMAND(ID_MENU_RDID, &CMFCUdsTestToolDlg::OnMenuRdid)
	ON_COMMAND(ID_MENU_WDID, &CMFCUdsTestToolDlg::OnMenuWdid)
	ON_COMMAND(ID_MENU_RESET, &CMFCUdsTestToolDlg::OnMenuReset)
	ON_COMMAND(ID_MENU_DTCON, &CMFCUdsTestToolDlg::OnMenuDtcon)
	ON_COMMAND(ID_MENU_DTCOFF, &CMFCUdsTestToolDlg::OnMenuDtcoff)
	ON_COMMAND(ID_MENU_RDDTC, &CMFCUdsTestToolDlg::OnMenuRddtc)
	ON_COMMAND(ID_MENU_CRDTC, &CMFCUdsTestToolDlg::OnMenuCrdtc)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, &CMFCUdsTestToolDlg::OnTcnSelchangeTabMain)
	ON_COMMAND(ID_MENU_CC, &CMFCUdsTestToolDlg::OnMenuCc)
	ON_COMMAND(ID_MENU_UDSCONF, &CMFCUdsTestToolDlg::OnMenuUdsconf)
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

	//为Table control 增加页面
	m_Table.InsertItem(0, _T("通讯"));
	m_Table.InsertItem(1, _T("诊断"));

	//创建对话框
	m_CanComm.Create(IDD_DIALOG_COM, &m_Table);
	m_UdsDiag.Create(IDD_DIALOG_DIAG, &m_Table);


	//设定在Table内显示的范围
	CRect rc;
	m_Table.GetClientRect(rc);
	rc.top += 23;
	rc.bottom -= 0;
	rc.left += 0;
	rc.right -= 0;
	m_CanComm.MoveWindow(&rc);
	m_UdsDiag.MoveWindow(&rc);

	//把对话框对象指针保存起来
	pDialog[0] = &m_CanComm;
	pDialog[1] = &m_UdsDiag;
	//显示初始页面
	pDialog[0]->ShowWindow(SW_SHOW);
	pDialog[1]->ShowWindow(SW_HIDE);
	//保存当前选择
	m_CurSelTab = 0;

	//开启接收线程
	AfxBeginThread(ReceiveThread, 0);
	//开启UDS线程
	AfxBeginThread(UdsMainThread, &theApp.UdsClient);

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

void CMFCUdsTestToolDlg::OnBnClickedBtOpendev()
{
	// TODO: 在此添加控件通知处理程序代码
	COpenDevDlg  Dlg;
	Dlg.DoModal();
}

void CMFCUdsTestToolDlg::OnMenuOpendev()
{
	// TODO: 在此添加命令处理程序代码
	COpenDevDlg  Dlg;

	m_CanComm.PrintLog(0, _T(">>Open Can Device"));
	Dlg.DoModal();

	m_CanComm.PrintLog(0, _T("      Done"));
}

void CMFCUdsTestToolDlg::OnMenuClosedev()
{
	m_CanComm.PrintLog(0, _T(">>Close Can Device"));
	// TODO: 在此添加命令处理程序代码
	if (VCI_CloseDevice(VCI_USBCAN2, CAN_DEVINDEX) != 1)
	{
		m_CanComm.PrintLog(0, _T("      Fail"));
		return;

	}

	m_CanComm.PrintLog(0, _T("      Done"));
}

void CMFCUdsTestToolDlg::OnMenuRdid()
{
	// TODO: 在此添加命令处理程序代码
	CRdWrDidDlg  Dlg;
	Dlg.RdWr = Dlg.EmRd;
	Dlg.DoModal();
}



void CMFCUdsTestToolDlg::OnMenuWdid()
{
	// TODO: 在此添加命令处理程序代码
	CRdWrDidDlg  Dlg;
	Dlg.RdWr = Dlg.EmWr;
	Dlg.DoModal();
}


void CMFCUdsTestToolDlg::OnMenuReset()
{
	// TODO: 在此添加命令处理程序代码
	BYTE RstBuf[BUF_LEN];
	UINT RstLen = 1;

	RstBuf[0] = 0x01;

	theApp.UdsClient.request(SID_11, RstBuf, RstLen);
}



void CMFCUdsTestToolDlg::OnMenuDtcon()
{
	// TODO: 在此添加命令处理程序代码
	BYTE CmdBuf[BUF_LEN];
	UINT CmdLen = 1;

	CmdBuf[0] = 0x01;

	theApp.UdsClient.request(SID_85, CmdBuf, CmdLen);
}


void CMFCUdsTestToolDlg::OnMenuDtcoff()
{
	// TODO: 在此添加命令处理程序代码
	BYTE CmdBuf[BUF_LEN];
	UINT CmdLen = 1;

	CmdBuf[0] = 0x02;

	theApp.UdsClient.request(SID_85, CmdBuf, CmdLen);
}


void CMFCUdsTestToolDlg::OnMenuRddtc()
{
	// TODO: 在此添加命令处理程序代码
	BYTE CmdBuf[BUF_LEN];
	UINT CmdLen = 1;

	//ReportSupportedDTC
	CmdBuf[0] = 0x0A;

	theApp.UdsClient.request(SID_19, CmdBuf, CmdLen);
}


void CMFCUdsTestToolDlg::OnMenuCrdtc()
{
	// TODO: 在此添加命令处理程序代码
	BYTE CmdBuf[BUF_LEN];
	UINT CmdLen = 3;

	//All Group
	CmdBuf[0] = 0xFF;
	CmdBuf[1] = 0xFF;
	CmdBuf[2] = 0xFF;

	theApp.UdsClient.request(SID_14, CmdBuf, CmdLen);
}


void CMFCUdsTestToolDlg::OnMenuCc()
{
	// TODO: 在此添加命令处理程序代码
	CUdsCcDlg  Dlg;
	Dlg.DoModal();
}


void CMFCUdsTestToolDlg::OnMenuUdsconf()
{
	// TODO: 在此添加命令处理程序代码
	CUdsConfDlg  Dlg;
	Dlg.DoModal();
}

void CMFCUdsTestToolDlg::OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//把当前的页面隐藏起来
	pDialog[m_CurSelTab]->ShowWindow(SW_HIDE);
	//得到新的页面索引
	m_CurSelTab = m_Table.GetCurSel();
	//把新的页面显示出来
	pDialog[m_CurSelTab]->ShowWindow(SW_SHOW);

	*pResult = 0;
}


INT CMFCUdsTestToolDlg::TransmitCanmsg(VCI_CAN_OBJ *SendObj)
{
	CMFCUdsTestToolDlg *dlg = (CMFCUdsTestToolDlg*)AfxGetApp()->GetMainWnd();

	INT flag;
	//调用动态链接库发送函数
	flag = VCI_Transmit(VCI_USBCAN2, CAN_DEVINDEX, theApp.m_CanChnl, SendObj, 1);//CAN message send

	if (flag < 1)
	{
		return flag;
	}

	dlg->m_CanComm.InsertItem(1, SendObj);
	return flag;
}

UINT CMFCUdsTestToolDlg::UdsMainThread(LPVOID v)
{
	CUdsClient *pObj = (CUdsClient *)v;
	while (1)
	{
		pObj->main_loop();
		Sleep(1);
	}
}

UINT CMFCUdsTestToolDlg::ReceiveThread(LPVOID v)
{
	CMFCUdsTestToolDlg *dlg = (CMFCUdsTestToolDlg*)AfxGetApp()->GetMainWnd();

	INT NumValue;
	INT num = 0;
	VCI_CAN_OBJ pCanObj[200];

	CString str, str1;

	CSize size;
	UINT JustnowItem;
	DWORD ReceivedID;

	size.cx = 0;
	size.cy = 50;

	while (1)
	{

		//调用动态链接库接收函数
		NumValue = VCI_Receive(VCI_USBCAN2, CAN_DEVINDEX, theApp.m_CanChnl, pCanObj, 200, 0);

		//接收信息列表显示
		CString strTime;
		SYSTEMTIME systime;
		GetLocalTime(&systime);
		strTime.Format(_T("%02d:%02d:%02d:%03d"), systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);

		for (num = 0; num<NumValue; num++)
		{

			ReceivedID = pCanObj[num].ID;

			if (ReceivedID == theApp.m_Rspid)
				theApp.UdsClient.netowrk_recv_frame(0, pCanObj[num].Data, pCanObj[num].DataLen);
			dlg->m_CanComm.InsertItem(0, pCanObj);
		}

		Sleep(5);
	}

	return 1;
}

