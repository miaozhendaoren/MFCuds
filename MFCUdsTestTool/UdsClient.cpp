/***************************************************************************//**
\file          UdsClient.c
\author        huanghai
\mail          huanghai@auto-link.com
\version       0.01
\date          2016-10-24
\description   uds client code, request service throngh can and deal the response
*******************************************************************************/
#include "stdafx.h"
#include "UdsClient.h"
#include "ControlCAN.h"
#include "MFCUdsTestTool.h"
#include "MFCUdsTestToolDlg.h"


/*******************************************************************************
Type declaration
*******************************************************************************/
typedef struct __UDS_SERVICE_T__
{
	BYTE uds_sid;
	BYTE uds_min_len;
	void(*uds_client)  (BYTE *, WORD);
	BOOL std_spt;   /* default session support */
	BOOL ext_spt;   /* extended session support */
	BOOL fun_spt;   /* function address support */
	BOOL ssp_spt;   /* subfunction suppress PosRsp bit support */
	uds_sa_lv uds_sa; /* security access */
}uds_client_t;


/*******************************************************************************
Function  declaration
*******************************************************************************/


CUdsClient::CUdsClient() : CUdsNetwork()
{
}


CUdsClient::~CUdsClient()
{
}


/**
* uds_timer_start - start uds timer
*
* void :
*
* returns:
*     void
*/
void CUdsClient::uds_timer_start(BYTE num)
{
	if (num >= UDS_TIMER_CNT) return;

	if (num == UDS_TIMER_FSA)
		uds_timer[UDS_TIMER_FSA] = TIMEOUT_FSA;
	if (num == UDS_TIMER_S3server)
		uds_timer[UDS_TIMER_S3server] = TIMEOUT_S3server;
}

void CUdsClient::uds_timer_stop(BYTE num)
{
	if (num >= UDS_TIMER_CNT) return;

	uds_timer[num] = 0;
}

/**
* uds_timer_run - run a uds timer, should be invoked per 1ms
*
* void :
*
* returns:
*     0 - timer is not running, 1 - timer is running, -1 - a timeout occur
*/
int CUdsClient::uds_timer_run(BYTE num)
{
	if (num >= UDS_TIMER_CNT) return 0;

	if (uds_timer[num] == 0)
	{
		return 0;
	}
	else if (uds_timer[num] == 1)
	{
		uds_timer[num] = 0;
		return -1;
	}
	else
	{
		/* if (uds_timer[num] > 1) */
		uds_timer[num]--;
		return 1;
	}

}

/**
* uds_timer_chk - check a uds timer and stop it
*
* num :
*
* returns:
*     0 - timer is not running, 1 - timer is running,
*/
INT CUdsClient::uds_timer_chk(BYTE num)
{
	if (num >= UDS_TIMER_CNT) return 0;

	if (uds_timer[num] > 0)
		return 1;
	else
		return 0;
}


/**
* N_USData_ffindication - uds first frame indication callbacl
*
* @msg_dlc : first frame dlc
*
* returns:
*     void
*/
void CUdsClient::N_USData_ffindication(WORD msg_dlc)
{
	uds_timer_stop(UDS_TIMER_S3server);
}

/**
* N_USData_indication - uds data request indication callback,
*
* @msg_buf  :
* @msg_dlc  :
* @n_result :
*
* returns:
*     void
*/

void CUdsClient::N_USData_indication(BYTE msg_buf[], WORD msg_dlc, n_result_t n_result)
{
	uds_timer_stop(UDS_TIMER_S3server);

	if (n_result != N_OK)
	{
		uds_timer_start(UDS_TIMER_S3server);
		return;
	}

	RspSid = msg_buf[0] - 0x40;
	if (RspSid != ReqSid)
		return;

	RspDlc = msg_dlc;
	UINT i;
	for (i = 0; i < RspDlc; i++)
		RspData[i] = msg_buf[i];
	GetRsp = TRUE;
}


/**
* N_USData_confirm - uds response confirm
*
* @n_result :
*
* returns:
*     void
*/
void CUdsClient::N_USData_confirm(n_result_t n_result)
{

	uds_timer_start(UDS_TIMER_S3server);
}

/*******************************************************************************
Function  Definition - common
*******************************************************************************/

void CUdsClient::ZTai_UDS_Send(BYTE CanData[], BYTE CanDlc)
{

	VCI_CAN_OBJ SendObj[1];

	int FrameFormat, FrameType;
	UINT i;

	FrameFormat = FRMFMT_STD;
	FrameType = FRMTYP_DAT;


	SendObj->ExternFlag = FrameType;
	SendObj->DataLen = CanDlc;
	SendObj->RemoteFlag = FrameFormat;
	if (FrameFormat == 1)//if remote frame, data area is invalid
	for (i = 0; i<CanDlc; i++)
	CanData[i] = 0;

	SendObj->ID = theApp.m_Phyid;

	for (i = 0; i<CanDlc; i++)
	SendObj->Data[i] = CanData[i];

	CMFCUdsTestToolDlg::TransmitCanmsg(SendObj);

}
/**
* main_loop - uds main loop, should be schedule every 1 ms
*
* @void  :
*
* returns:
*     void
*/
void CUdsClient::main_loop(void)
{
	network_main();
}



/**
* sv_request - uds service request
*
* @void  :
*
* returns:
*     void
*/
void CUdsClient::request(BYTE SvcId, BYTE ReqBuf[], UINT ReqLen)
{
	BYTE cmd_buf[BUF_LEN];

	GetRsp = FALSE;
	ReqSid = SvcId;
	cmd_buf[0] = SvcId;

	UINT i;
	for (i = 0; i < ReqLen; i++)
	{
		cmd_buf[1 + i] = ReqBuf[i];
	}

	netowrk_send_udsmsg(cmd_buf, ReqLen+1);
}


UINT CUdsClient::get_rsp(BYTE DataBuf[], UINT BufLen)
{
	/*
	DWORD Ticks;
	Ticks = GetTickCount();

	while (GetRsp == FALSE)
	{
		if ((GetTickCount() - Ticks) >= 2000)
			break;
		Sleep(200);
	}
	*/

	if (GetRsp == TRUE)
	{
		UINT i;
		for (i = 0; i < RspDlc && i < BufLen; i++)
			DataBuf[i] = RspData[i];

		GetRsp = FALSE;
		return i;
	}
	else
	{
		return 0;
	}
}
