/***************************************************************************//**
\file          UdsClient.c
\author        huanghai
\mail          huanghai@auto-link.com
\version       0.01
\date          2016-10-24
\description   uds client code, request service throngh can and deal the response
*******************************************************************************/
#include "stdafx.h"
#include "NetworkLayer.h"
#include "UdsClient.h"
#include "RdWrData.h"


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


BYTE RspData[BUF_LEN];
UINT RspDlc;
BOOL GetRsp;
BYTE ReqSid;
BYTE RspSid;

/* uds user layer timer */
static UINT uds_timer[UDS_TIMER_CNT] = { 0 };

/**
* uds_timer_start - start uds timer
*
* void :
*
* returns:
*     void
*/
static void
uds_timer_start(BYTE num)
{
	if (num >= UDS_TIMER_CNT) return;

	if (num == UDS_TIMER_FSA)
		uds_timer[UDS_TIMER_FSA] = TIMEOUT_FSA;
	if (num == UDS_TIMER_S3server)
		uds_timer[UDS_TIMER_S3server] = TIMEOUT_S3server;
}

static void
uds_timer_stop(BYTE num)
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
static int
uds_timer_run(BYTE num)
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
static INT
uds_timer_chk(BYTE num)
{
	if (num >= UDS_TIMER_CNT) return 0;

	if (uds_timer[num] > 0)
		return 1;
	else
		return 0;
}


/**
* uds_dataff_indication - uds first frame indication callbacl
*
* @msg_dlc : first frame dlc
*
* returns:
*     void
*/
static void
uds_dataff_indication(WORD msg_dlc)
{
	uds_timer_stop(UDS_TIMER_S3server);
}

/**
* uds_data_indication - uds data request indication callback,
*
* @msg_buf  :
* @msg_dlc  :
* @n_result :
*
* returns:
*     void
*/

static void
uds_data_indication(BYTE msg_buf[], WORD msg_dlc, n_result_t n_result)
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
* uds_data_confirm - uds response confirm
*
* @n_result :
*
* returns:
*     void
*/
static void
uds_data_confirm(n_result_t n_result)
{

	uds_timer_start(UDS_TIMER_S3server);
}


/**
* uds_client_main - uds main loop, should be schedule every 1 ms
*
* @void  :
*
* returns:
*     void
*/
extern void
uds_client_main(void)
{
	network_main();

}

/**
* uds_client_init - uds user layer init
*
* @void  :
*
* returns:
*     0 - ok
*/
extern int
uds_client_init(void)
{
	nt_usdata_t usdata;

	usdata.ffindication = uds_dataff_indication;
	usdata.indication = uds_data_indication;
	usdata.confirm = uds_data_confirm;

	return netowrk_reg_usdata(usdata);
}



extern void
uds_client_request(BYTE SvcId, BYTE ReqBuf[], UINT ReqLen)
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


UINT uds_get_rsp(BYTE DataBuf[], UINT BufLen)
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