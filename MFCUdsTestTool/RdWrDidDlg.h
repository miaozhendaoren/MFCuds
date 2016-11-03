#pragma once
#include "ColoredListCtrl.h"
#include <afxtempl.h>
#include <queue>

typedef struct
{
	UINT SID;
	UINT CmdLen; // The unit is millisecond
	BYTE CmdBuf[BUF_LEN];
}UdsCmd, *PUdsCmd;

#define GETRSP_MS 2000
#define TIMOUT_MS 100
#define GETRSP_CNT (GETRSP_MS/TIMOUT_MS)

// CReadDidDlg 对话框

class CRdWrDidDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRdWrDidDlg)

public:
	CRdWrDidDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRdWrDidDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RDWRDID };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CColoredListCtrl m_list;
	BOOL SetTim;
	CString m_WrResult;
public:

	BYTE m_Step;
	BOOL m_Rdwr;

    CArray<UdsCmd> m_CmdList;
	UdsCmd m_CmdNow;
	BOOL m_GetRsp;
	BYTE m_GetRspCnt;
	BYTE m_RspBuf[4];

	enum { EmRd = 0, EmWr = 1 };
	BYTE RdWr;
	int nItem, nSubItem;
	BOOL GetInput;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtRddid();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLvnItemchangedListRdid(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListRdid(NMHDR *pNMHDR, LRESULT *pResult);

	static UINT UdsRdwrThread(void *param);

	// shis function inserts the default values into the listControl
	void InsertItems();
	// this function will returns the item text depending on the item and SubItem Index
	CString GetItemText(HWND hWnd, int nItem, int nSubItem) const;
	afx_msg void OnNMClickListRdwrdid(NMHDR *pNMHDR, LRESULT *pResult);
	// This function set the text in the specified SubItem depending on the Row and Column values
	void SetCell(HWND hWnd1, CString value, int nRow, int nCol);

	INT uds_input_wdidlist(UINT did_n, CString str);
};


/* uds read/write data read-write mode */
typedef enum __UDS_RWDATA_RW__
{
	UDS_RWDATA_RDONLY = 0,
	UDS_RWDATA_RDWR,
	UDS_RWDATA_RDWR_WRONCE,
	UDS_RWDATA_RDWR_INBOOT
}uds_rwdata_rw;

/* uds read/write data read-write mode */
typedef enum __UDS_RWDATA_TYPE__
{
	UDS_RWDATA_HEX = 0,
	UDS_RWDATA_ASCII,
	UDS_RWDATA_BCD
}uds_rwdata_type;

/* uds read/write data read-write mode */
typedef enum __UDS_RWDATA_STORE__
{
	UDS_RWDATA_RAM = 0,
	UDS_RWDATA_DFLASH,
	UDS_RWDATA_EEPROM
}uds_rwdata_store;

/* uds read/write data typedef */
typedef struct __UDS_RWDATA_T__
{
	CString name;
	WORD did;    /* 0100 - EFFF and F010 - F0FF for vehicleManufacturerSpecific */
	BYTE *p_data;
	BYTE dlc;
	uds_rwdata_rw rw_mode;
	uds_rwdata_store rw_store;
}uds_rwdata_t;

#define RWDATA_CNT  11