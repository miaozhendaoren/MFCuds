// ReadDidDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCUdsTestTool.h"
#include "RdWrDidDlg.h"
#include "afxdialogex.h"
#include "UdsClient.h"
#include "UdsUtil.h"

// Read/Write Did date
BYTE ASC_boot_ver[10];
BYTE uds_session;
BYTE ASC_sys_supplier_id[5];
BYTE ASC_soft_ver[10];
BYTE ASC_sys_name[10];
BYTE ASC_ecu_part_num[15];
BYTE BCD_manufacture_date[3];
BYTE HEX_ecu_sn[10];
BYTE ASC_VIN[17];
BYTE HEX_tester_sn[10];
BYTE BCD_program_date[3];


const uds_rwdata_t rwdata_list[RWDATA_CNT] =
{
	{ _T("BootVer"),    0xF183, ASC_boot_ver,         10, UDS_RWDATA_RDONLY,      UDS_RWDATA_DFLASH },
	{ _T("UdsSession"), 0xF186, &uds_session,         1,  UDS_RWDATA_RDONLY,      UDS_RWDATA_RAM },
	{ _T("EcuPartNUm"), 0xF187, ASC_ecu_part_num,     15, UDS_RWDATA_RDWR_INBOOT, UDS_RWDATA_EEPROM },
	{ _T("SupplierId"), 0xF18A, ASC_sys_supplier_id,  5,  UDS_RWDATA_RDONLY,      UDS_RWDATA_DFLASH },
	{ _T("ManuDate"),   0xF18B, BCD_manufacture_date, 3,  UDS_RWDATA_RDONLY,      UDS_RWDATA_EEPROM }, /* be writen after manufacture */
	{ _T("EcuSn"),      0xF18C, HEX_ecu_sn,           10, UDS_RWDATA_RDONLY,      UDS_RWDATA_EEPROM }, /* be writen after manufacture */
	{ _T("VIN"),        0xF190, ASC_VIN,              17, UDS_RWDATA_RDWR_WRONCE, UDS_RWDATA_EEPROM }, /* be writen after installment */
	{ _T("SoftVer"),    0xF195, ASC_soft_ver,         10, UDS_RWDATA_RDONLY,      UDS_RWDATA_DFLASH },
	{ _T("SysName"),    0xF197, ASC_sys_name,         10, UDS_RWDATA_RDONLY,      UDS_RWDATA_DFLASH },
	{ _T("TesterSn"),   0xF198, HEX_tester_sn,        10, UDS_RWDATA_RDWR_INBOOT, UDS_RWDATA_EEPROM }, /* update by tester after program */
	{ _T("ProgDate"),   0xF199, BCD_program_date,     3,  UDS_RWDATA_RDWR_INBOOT, UDS_RWDATA_EEPROM } /* update by tester after program */
};

// CReadDidDlg 对话框

IMPLEMENT_DYNAMIC(CRdWrDidDlg, CDialogEx)

CRdWrDidDlg::CRdWrDidDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_RDWRDID, pParent)
{

}

CRdWrDidDlg::~CRdWrDidDlg()
{
	m_CmdList.RemoveAll();
}

void CRdWrDidDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_RDWRDID, m_list);
	DDX_Text(pDX, IDC_EDIT_WRRST, m_WrResult);
}


BEGIN_MESSAGE_MAP(CRdWrDidDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BT_RDDID, &CRdWrDidDlg::OnBnClickedBtRddid)
	ON_WM_TIMER()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RDWRDID, &CRdWrDidDlg::OnLvnItemchangedListRdid)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RDWRDID, &CRdWrDidDlg::OnNMDblclkListRdid)
	ON_NOTIFY(NM_CLICK, IDC_LIST_RDWRDID, &CRdWrDidDlg::OnNMClickListRdwrdid)
END_MESSAGE_MAP()


// CReadDidDlg 消息处理程序


BOOL CRdWrDidDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetTimer(1, TIMOUT_MS, NULL);
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES);

	m_list.InsertColumn(0, _T("Name"));
	m_list.SetColumnWidth(0, 60);
	m_list.InsertColumn(1, _T(" DID "));
	m_list.SetColumnWidth(1, 50);
	m_list.InsertColumn(2, _T("DLC"));
	m_list.SetColumnWidth(2, 50);
	m_list.InsertColumn(3, _T("Data"));
	m_list.SetColumnWidth(3, 160);

	UINT listrow = 0;
	UINT nowItem;

	CString str;
	CString str1;


	m_GetRspCnt = 0;

	nItem = -1;
	nSubItem = -1;
	m_GetRsp = FALSE;
	GetInput = FALSE;

	::ShowWindow(::GetDlgItem(m_hWnd, IDC_EDIT_RDWRDID), SW_HIDE);

	if (RdWr == EmRd)
	{
		for (listrow = 0; listrow < RWDATA_CNT; listrow++)
		{
			m_list.ItemTextColor[listrow] = 0;

			m_list.ItemColorFlag[listrow] = 0;

			nowItem = m_list.InsertItem(listrow, rwdata_list[listrow].name);

			str.Format(_T("0X%04X"), rwdata_list[listrow].did);

			m_list.SetItemText(nowItem, 1, str);	//DID

			str.Format(_T("%d"), rwdata_list[listrow].dlc);

			m_list.SetItemText(nowItem, 2, str);	//DID
		}
	}
	else
	{
		for (listrow = 0; listrow < RWDATA_CNT; listrow++)
		{
			if (rwdata_list[listrow].rw_mode == UDS_RWDATA_RDONLY)
				m_list.ItemTextColor[listrow] = 2;

			m_list.ItemColorFlag[listrow] = 0;

			nowItem = m_list.InsertItem(listrow, rwdata_list[listrow].name);

			str.Format(_T("0X%04X"), rwdata_list[listrow].did);

			m_list.SetItemText(nowItem, 1, str);	//DID

			str.Format(_T("%d"), rwdata_list[listrow].dlc);

			m_list.SetItemText(nowItem, 2, str);	//DID
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CRdWrDidDlg::OnBnClickedBtRddid()
{
	// TODO: 在此添加控件通知处理程序代码
	m_WrResult = _T("");
	m_WrResult += _T("Write");
	UpdateData(false);//更新控件数据
#if 0
	BYTE PreBuf[BUF_LEN];
	BYTE DidBuf[BUF_LEN];
	UINT DidLen;
	UINT listrow = 0;

	BYTE DataBuf[BUF_LEN];
	UINT readlen;

	PreBuf[0] = 0x03;
	theApp.UdsClient.request(SID_10, PreBuf, 1);
	Sleep(100);
	readlen = theApp.UdsClient.get_rsp(DataBuf, BUF_LEN);
	if (readlen == 0) return;

	if (RdWr == EmRd)
	{
		for (listrow = 0; listrow < RWDATA_CNT; listrow++)
		{
			DidLen = 0;
			if (m_list.GetCheck(listrow))
			{
				DidBuf[DidLen++] = (BYTE)(rwdata_list[listrow].did >> 8);
				DidBuf[DidLen++] = (BYTE)(rwdata_list[listrow].did >> 0);

				theApp.UdsClient.request(SID_22, DidBuf, DidLen);
				SetTimer(1, TIMOUT_MS, NULL);
				m_GetRsp = TRUE;
				break;
			}
		}
	}
	else
	{
		//Request seed
		PreBuf[0] = 0x01;
		theApp.UdsClient.request(SID_27, PreBuf, 1);
		Sleep(200);
		readlen = theApp.UdsClient.get_rsp(DataBuf, BUF_LEN);
		if (readlen != 6 || DataBuf[1] != 0x01) return;

		//Send key
		PreBuf[0] = 0x02;
		UdsUtil::KeyCalcu(&DataBuf[2], &PreBuf[1]);
		theApp.UdsClient.request(SID_27, PreBuf, 5);
		Sleep(100);
		readlen = theApp.UdsClient.get_rsp(DataBuf, BUF_LEN);
		if (readlen != 2 || DataBuf[1] != 0x02) return;

		for (listrow = 0; listrow < RWDATA_CNT; listrow++)
		{
			DidLen = 0;
			if (m_list.GetCheck(listrow))
			{
				DidBuf[DidLen++] = (BYTE)(rwdata_list[listrow].did >> 8);
				DidBuf[DidLen++] = (BYTE)(rwdata_list[listrow].did >> 0);

				int i;
				for (i = 0; i < rwdata_list[listrow].dlc; i++)
				{
					DidBuf[DidLen++] = rwdata_list[listrow].p_data[i];
				}

				theApp.UdsClient.request(SID_2E, DidBuf, DidLen);
				SetTimer(1, TIMOUT_MS, NULL);
				m_GetRsp = TRUE;
				break;
			}
		}
	}
#endif
	UdsCmd CmdNew;
	UINT DidLen;
	UINT listrow = 0;

	//Push Session request cmd
	CmdNew.SID = SID_10;
	CmdNew.CmdBuf[0] = 0x03;
	CmdNew.CmdLen = 1;
	m_CmdList.Add(CmdNew);

	if (RdWr == EmRd)
	{
		for (listrow = 0; listrow < RWDATA_CNT; listrow++)
		{
			DidLen = 0;
			CmdNew.SID = SID_22;
			if (m_list.GetCheck(listrow))
			{
				CmdNew.CmdBuf[DidLen++] = (BYTE)(rwdata_list[listrow].did >> 8);
				CmdNew.CmdBuf[DidLen++] = (BYTE)(rwdata_list[listrow].did >> 0);

				CmdNew.CmdLen = DidLen;
				m_CmdList.Add(CmdNew);
				break;
			}
		}
	}
	else
	{

		//Push request seed cmd
		CmdNew.SID = SID_27;
		CmdNew.CmdBuf[0] = 0x01;
		CmdNew.CmdLen = 1;
		m_CmdList.Add(CmdNew);

		//Push send key cmd
		CmdNew.SID = SID_27;
		CmdNew.CmdBuf[0] = 0x02;
		CmdNew.CmdLen = 5;
		m_CmdList.Add(CmdNew);

		for (listrow = 0; listrow < RWDATA_CNT; listrow++)
		{
			DidLen = 0;
			CmdNew.SID = SID_2E;
			if (m_list.GetCheck(listrow))
			{
				CmdNew.CmdBuf[DidLen++] = (BYTE)(rwdata_list[listrow].did >> 8);
				CmdNew.CmdBuf[DidLen++] = (BYTE)(rwdata_list[listrow].did >> 0);

				int i;
				for (i = 0; i < rwdata_list[listrow].dlc; i++)
				{
					CmdNew.CmdBuf[DidLen++] = rwdata_list[listrow].p_data[i];
				}

				CmdNew.CmdLen = DidLen;
				m_CmdList.Add(CmdNew);
				break;
			}
		}
	}
}


void CRdWrDidDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
#if 0
	BYTE DataBuf[BUF_LEN];
	UINT readlen;
	UINT remnpos;

	WORD curr_did;
	UINT listrow = 0;
	UINT did_n;
	BOOL find_did;
	CString str;
	CString str1;

	if (m_GetRsp == TRUE)
	{
		m_GetRsp = FALSE;
		readlen = theApp.UdsClient.get_rsp(DataBuf, BUF_LEN);

		if (RdWr == EmRd)
		{
			remnpos = 1;
			while (remnpos < readlen)
			{
				curr_did = DataBuf[remnpos + 1];
				curr_did |= ((WORD)DataBuf[remnpos]) << 8;

				find_did = FALSE;
				for (did_n = 0; did_n < RWDATA_CNT; did_n++)
				{
					if (rwdata_list[did_n].did == curr_did)
					{
						find_did = TRUE;
						remnpos += 2;
						UINT i;
						for (i = 0; i < rwdata_list[did_n].dlc; i++)
						{
							str1.Format(_T("%02X"), DataBuf[remnpos + i]);
							str += str1;
						}
						remnpos += rwdata_list[did_n].dlc;
						listrow = did_n;
						m_list.SetItemText(listrow, 3, str);
						break;
					}
				}

				if (find_did == FALSE)
					break;
			}
		}
		else
		{
			/* Always can't get response */
			if (readlen > 0)
				MessageBox(_T("write did get response\n"));
		}
	}
#endif
	BYTE DataBuf[BUF_LEN];
	UINT readlen;
	UINT remnpos;

	WORD curr_did;
	UINT listrow = 0;
	UINT did_n;

	CString str;
	CString str1;

	if (m_GetRsp == TRUE)
	{
		readlen = theApp.UdsClient.get_rsp(DataBuf, BUF_LEN);

		if (readlen > 0)
		{
			m_GetRsp = FALSE;

			switch (m_CmdNow.SID)
			{
				case SID_22:
				{
					remnpos = 1;
					while (remnpos < readlen)
					{
						curr_did = DataBuf[remnpos + 1];
						curr_did |= ((WORD)DataBuf[remnpos]) << 8;

						for (did_n = 0; did_n < RWDATA_CNT; did_n++)
						{
							if (rwdata_list[did_n].did == curr_did)
							{
		
								remnpos += 2;
								UINT i;
								for (i = 0; i < rwdata_list[did_n].dlc; i++)
								{
									str1.Format(_T("%02X"), DataBuf[remnpos + i]);
									str += str1;
								}
								remnpos += rwdata_list[did_n].dlc;
								listrow = did_n;
								m_list.SetItemText(listrow, 3, str);
								break;
							}
						}
						break;
					}
				}
				break;
				case SID_2E:
					m_RspBuf[0] = DataBuf[0];
					m_RspBuf[1] = DataBuf[1];
					m_WrResult = _T("");
					m_WrResult += _T("Success");
					UpdateData(false);//更新控件数据
				break;
				case SID_27:
				{
					if (readlen == 6 && DataBuf[1] == 0x01)
					{
						m_RspBuf[0] = DataBuf[2];
						m_RspBuf[1] = DataBuf[3];
						m_RspBuf[2] = DataBuf[4];
						m_RspBuf[3] = DataBuf[5];
					}
				}
				break;
				case SID_10:
				default:
				break;
			}

		}
		else
		{
			m_GetRspCnt++;
			if (m_GetRspCnt >= GETRSP_CNT)
			{
				m_GetRsp = FALSE;
			}
		}
	}
	else
	{
		INT CmdSize = m_CmdList.GetSize();
		if (CmdSize <= 0) return;
		m_CmdNow = m_CmdList[0];
		m_CmdList.RemoveAt(0, 1);
		if (m_CmdNow.SID == SID_27 && m_CmdNow.CmdBuf[0] == 0x02)
		{
			UdsUtil::KeyCalcu(m_RspBuf, &m_CmdNow.CmdBuf[1]);
		}
		if (m_CmdNow.SID == SID_2E)
			m_RspBuf[0] = 1;
		theApp.UdsClient.request(m_CmdNow.SID, m_CmdNow.CmdBuf, m_CmdNow.CmdLen);
		m_GetRsp = TRUE;
		m_GetRspCnt = 0;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CRdWrDidDlg::OnLvnItemchangedListRdid(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	if (m_list.GetCheck(pNMLV->iItem))
	{
		UINT item;
		for (item = 0; item < m_list.GetItemCount(); item++)
		{
			if (item == pNMLV->iItem)
				continue;
			m_list.SetCheck(item, FALSE);
		}
	}

	*pResult = 0;
}


void CRdWrDidDlg::OnNMDblclkListRdid(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	if (pNMItemActivate->iItem == -1) return;

	if (RdWr == EmWr)
	{   /*
		CString cstr;
		int nItem = m_list.GetNextItem(-1, LVNI_SELECTED);
		cstr.Format(_T("double click %d \n"), nItem);
		MessageBox(cstr);
		*/
		Invalidate();
		HWND hWnd1 = ::GetDlgItem(m_hWnd, IDC_LIST_RDWRDID);
		LPNMITEMACTIVATE pNMItemActivate = (LPNMITEMACTIVATE)pNMHDR;
		RECT rect;
		//get the row number
		nItem = pNMItemActivate->iItem;
		//get the column number
		nSubItem = pNMItemActivate->iSubItem;
		if (nSubItem != 3 || nItem == -1 || rwdata_list[nItem].rw_mode == UDS_RWDATA_RDONLY)
			return;
		GetInput = TRUE;
		//Retrieve the text of the selected subItem 
		//from the list
		CString str = GetItemText(hWnd1, nItem,
			nSubItem);

		RECT rect1, rect2;
		// this macro is used to retrieve the Rectanle 
		// of the selected SubItem
		ListView_GetSubItemRect(hWnd1, nItem,
			nSubItem, LVIR_BOUNDS, &rect);
		//Get the Rectange of the listControl
		::GetWindowRect(pNMItemActivate->hdr.hwndFrom, &rect1);
		//Get the Rectange of the Dialog
		::GetWindowRect(m_hWnd, &rect2);

		int x = rect1.left - rect2.left;
		int y = rect1.top - rect2.top;

		if (nItem != -1)
			::SetWindowPos(::GetDlgItem(m_hWnd, IDC_EDIT_RDWRDID),
				HWND_TOP, rect.left + x - 7, rect.top + 13,
				rect.right - rect.left,
				rect.bottom - rect.top, NULL);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_EDIT_RDWRDID), SW_SHOW);
		::SetFocus(::GetDlgItem(m_hWnd, IDC_EDIT_RDWRDID));
		//Draw a Rectangle around the SubItem
		//::Rectangle(::GetDC(pNMItemActivate->hdr.hwndFrom),
			//rect.left, rect.top - 1, rect.right, rect.bottom);
		//Set the listItem text in the EditBox
		::SetWindowText(::GetDlgItem(m_hWnd, IDC_EDIT_RDWRDID), str);
	}
	*pResult = 0;
}


// shis function inserts the default values into the listControl
void CRdWrDidDlg::InsertItems()
{
	HWND hWnd = ::GetDlgItem(m_hWnd, IDC_LIST_RDWRDID);

}


// this function will returns the item text depending on the item and SubItem Index
CString CRdWrDidDlg::GetItemText(HWND hWnd, int nItem, int nSubItem) const
{
	LVITEM lvi;
	memset(&lvi, 0, sizeof(LVITEM));
	lvi.iSubItem = nSubItem;
	CString str;
	int nLen = 128;
	int nRes;
	do
	{
		nLen *= 2;
		lvi.cchTextMax = nLen;
		lvi.pszText = str.GetBufferSetLength(nLen);
		nRes = (int)::SendMessage(hWnd,
			LVM_GETITEMTEXT, (WPARAM)nItem,
			(LPARAM)&lvi);
	} while (nRes == nLen - 1);
	str.ReleaseBuffer();
	return str;
}


void CRdWrDidDlg::OnNMClickListRdwrdid(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int mItem, mSubItem;

	//get the row number
	mItem = pNMItemActivate->iItem;
	//get the column number
	mSubItem = pNMItemActivate->iSubItem;

	if (GetInput == FALSE) return;
	GetInput = FALSE;


	CString str;
	//get the text from the EditBox
	GetDlgItemText(IDC_EDIT_RDWRDID, str);

	if (uds_input_wdidlist(nItem, str) < 0)
		return;

	//set the value in the listContorl with the
	//specified Item & SubItem values
	SetCell(::GetDlgItem(m_hWnd, IDC_LIST_RDWRDID),
		str, nItem, nSubItem);

	::SendDlgItemMessage(m_hWnd, IDC_EDIT_RDWRDID,
		WM_KILLFOCUS, 0, 0);
	::ShowWindow(::GetDlgItem(m_hWnd, IDC_EDIT_RDWRDID),
		SW_HIDE);

	*pResult = 0;
}


// This function set the text in the specified SubItem depending on the Row and Column values
void CRdWrDidDlg::SetCell(HWND hWnd1, CString value, int nRow, int nCol)
{
	TCHAR     szString[256];
	wsprintf(szString, value, 0);

	//Fill the LVITEM structure with the 
	//values given as parameters.
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nRow;
	lvItem.pszText = szString;
	lvItem.iSubItem = nCol;
	if (nCol >0)
		//set the value of listItem
		::SendMessage(hWnd1, LVM_SETITEM,
		(WPARAM)0, (WPARAM)&lvItem);
	else
		//Insert the value into List
		ListView_InsertItem(hWnd1, &lvItem);
}

INT CRdWrDidDlg::uds_input_wdidlist(UINT did_n, CString str)
{
	BYTE hex_str[32];
	BYTE hex_buf[50];
	BYTE data_buf[50];

	BYTE temp_char;
	BYTE temp_buf[50];
	LONG temp_len;

	int datanum = 0, newflag = 1, i;

	if (did_n >= RWDATA_CNT) return -1;

	temp_len = UdsUtil::str2char(str, temp_buf) - 1;
	newflag = 1;
	for (i = 0; i < temp_len; i++)
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
			}
		}
	}

	memset(rwdata_list[did_n].p_data, 0, rwdata_list[did_n].dlc);

	for (i = 0; i < datanum && i < rwdata_list[did_n].dlc; i++)
	{
		rwdata_list[did_n].p_data[i] = data_buf[i];
	}

	return 0;
}