// ReadDidDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCUdsTestTool.h"
#include "RdWrDidDlg.h"
#include "afxdialogex.h"
#include "RdWrData.h"
#include "UdsClient.h"

// CReadDidDlg 对话框

IMPLEMENT_DYNAMIC(CRdWrDidDlg, CDialogEx)

CRdWrDidDlg::CRdWrDidDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_RDWRDID, pParent)
{

}

CRdWrDidDlg::~CRdWrDidDlg()
{
}

void CRdWrDidDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_RDWRDID, m_list);
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


	nItem = -1;
	nSubItem = -1;
	SetTim = FALSE;

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
	BYTE SesBuf[BUF_LEN];
	BYTE DidBuf[BUF_LEN];
	UINT DidLen;
	UINT listrow = 0;

	DidLen = 0;

	if (RdWr == EmRd)
	{
		for (listrow = 0; listrow < RWDATA_CNT; listrow++)
		{
			if (m_list.GetCheck(listrow))
			{
				DidBuf[DidLen++] = (BYTE)(rwdata_list[listrow].did >> 8);
				DidBuf[DidLen++] = (BYTE)(rwdata_list[listrow].did >> 0);

				uds_client_request(SID_22, DidBuf, DidLen);
				SetTimer(1, 100, NULL);
				SetTim = TRUE;
				break;
			}
		}
	}
	else
	{
		for (listrow = 0; listrow < RWDATA_CNT; listrow++)
		{
			if (m_list.GetCheck(listrow))
			{
				DidBuf[DidLen++] = (BYTE)(rwdata_list[listrow].did >> 8);
				DidBuf[DidLen++] = (BYTE)(rwdata_list[listrow].did >> 0);

				int i;
				for (i = 0; i < rwdata_list[listrow].dlc; i++)
				{
					DidBuf[DidLen++] = rwdata_list[listrow].p_data[i];
				}
				SesBuf[0] = 0x03;
				uds_client_request(SID_10, SesBuf, 1);
				Sleep(50);
				uds_client_request(SID_2E, DidBuf, DidLen);
				SetTimer(1, 100, NULL);
				SetTim = TRUE;
				break;
			}
		}
	}
}


void CRdWrDidDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	BYTE DataBuf[BUF_LEN];
	UINT readlen;
	UINT remnpos;

	WORD curr_did;
	UINT listrow = 0;
	UINT did_n;
	BOOL find_did;
	CString str;
	CString str1;

	if (SetTim == TRUE)
	{
		readlen = uds_get_rsp(DataBuf, BUF_LEN);

		if (RdWr == EmRd)
		{
			remnpos = 1;
			while (remnpos <= readlen)
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
		SetTim = FALSE;
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
		if (nSubItem != 3 || nItem == -1)
			return;
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

	if ((nItem == mItem && nSubItem == mSubItem))
		return;

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
