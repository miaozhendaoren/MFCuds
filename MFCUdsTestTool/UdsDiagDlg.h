#pragma once
#include "ColoredListCtrl.h"

// CUdsDiagDlg 对话框

typedef enum __OBD_DTC_NAME_T__
{
	DTC_FUEL_SENSOR_BELOW = 0,
	DTC_FUEL_SENSOR_ABOVE,
	DTC_BATT_VOLTAG_BELOW,
	DTC_BATT_VOLTAG_ABOVE,
	DTC_CAN_BUS_OFF,
	DTC_COM_LOST_EMS,
	DTC_COM_LOST_TCU,
	DTC_COM_LOST_ABS,
	DTC_COM_LOST_TPMS,
	DTC_COM_LOST_EPB,
	DTC_COM_LOST_ESP,
	DTC_COM_LOST_EPS,
	DTC_COM_LOST_BCM,
	DTC_COM_LOST_SRS,
	DTC_COM_LOST_DVD,
	DTC_COM_LOST_PDC,
	DTC_COM_LOST_ALS,
	DTC_COM_LOST_PEPS,
	DTC_COM_LOST_AC,
	DTC_COM_LOST_AVM,
	DTC_LIGHT_ERROR_BREAK,
	DTC_LIGHT_ERROR_EPC,
	DTC_LIGHT_ERROR_TCU,
	DTC_LIGHT_ERROR_AIR_BAG,
	DTC_LIGHT_ERROR_ESC,
	DTC_LIGHT_ERROR_ABS,
	DTC_LIGHT_ERROR_MIL,
	DTC_LIGHT_ERROR_EPB_FAULT,
	DTC_LIGHT_ERROR_ESC_OFF,
	OBD_DTC_CNT
}obd_dtc_name_t;

typedef struct __OBD_DTC_PARA_T__
{
	CString Name;
	CString DispCode;
	UINT  DtcCode;
	BYTE  DtcSt;
}obd_dtc_t;

class CUdsDiagDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUdsDiagDlg)

public:
	CUdsDiagDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUdsDiagDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DIAG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CColoredListCtrl m_List;
	BOOL SetTim;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSptdtc();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonClrdtc();
};
