#pragma once
/*****************************************************************************
\file          network_layer.h
\author        huanghai
\mail          huanghai@auto-link.com
\version       0
\date          2016-10-24
\description   uds network code, base on ISO 15765
*******************************************************************************/


/*******************************************************************************
Include Files
*******************************************************************************/
#include "afxwin.h"
#include "NetworkLayerPrivate.h"

typedef enum _N_TATYPE_T_
{
	N_TATYPE_NONE = 0,
	N_TATYPE_PHYSICAL,
	N_TATYPE_FUNCTIONAL
}n_tatype_t;

typedef enum _N_RESULT_
{
	N_OK = 0,
	N_TIMEOUT_A,
	N_TIMEOUT_Bs,
	N_TIMEOUT_Cr,
	N_WRONG_SN,
	N_INVALID_FS,
	N_UNEXP_PDU,
	N_WFT_OVRN,
	N_BUFFER_OVFLW,
	N_ERROR
}n_result_t;


/*******************************************************************************
external Varaibles
*******************************************************************************/



class CUdsNetwork
{
public:
	CUdsNetwork();
	~CUdsNetwork();

private:
	/*******************************************************************************
	Global Varaibles
	*******************************************************************************/
	network_layer_st nwl_st;

	BOOL g_wait_cf;
	BOOL g_wait_fc;

	UINT nt_timer[TIMER_CNT];

	BYTE g_rfc_stmin;    /* received flowcontrol SeparationTime */
	BYTE g_rfc_bs;       /* received flowcontrol block size */

	BYTE g_xcf_bc;       /* transmit consecutive frame block counter */
	BYTE g_xcf_sn;       /* transmit consecutive frame SequenceNumber */
	BYTE g_rcf_bc;       /* received frame block counter */
	BYTE g_rcf_sn;       /* received consecutive frame SequenceNumber */

									/* transmit buffer */
	BYTE remain_buf[UDS_FF_DL_MAX];
	WORD remain_len;
	WORD remain_pos;

	/* recieve buffer */
	BYTE recv_buf[UDS_FF_DL_MAX];
	WORD recv_len;
    WORD recv_fdl;  /* frame data len */

public:
	BYTE g_tatype;

private:
	void nt_timer_start(BYTE num);
	void nt_timer_start_wv(BYTE num, UINT value);
	void nt_timer_stop(BYTE num);
	int nt_timer_run(BYTE num);
	int nt_timer_chk(BYTE num);
	void clear_network(void);
	void recv_singleframe(BYTE frame_buf[], BYTE frame_dlc);
	int recv_firstframe(BYTE frame_buf[], BYTE frame_dlc);
	int recv_consecutiveframe(BYTE frame_buf[], BYTE frame_dlc);
	int recv_flowcontrolframe(BYTE frame_buf[], BYTE frame_dlc);
	void send_flowcontrol(BYTE flow_st);
	void send_singleframe(BYTE msg_buf[], WORD msg_dlc);
	int send_firstframe(BYTE msg_buf[], WORD msg_dlc);
	int send_consecutiveframe(BYTE msg_buf[], WORD msg_dlc, BYTE frame_sn);
	void send_multipleframe(BYTE msg_buf[], WORD msg_dlc);

protected:
	virtual void ZTai_UDS_Send(BYTE CanData[], BYTE CanDlc) = 0;
	virtual void N_USData_ffindication(WORD msg_dlc) = 0;
	virtual void N_USData_indication(BYTE msg_buf[], WORD msg_dlc, n_result_t n_result) = 0;
	virtual void N_USData_confirm(n_result_t n_result) = 0;

public:
    void network_main(void);
	void netowrk_recv_frame(BYTE func_addr, BYTE frame_buf[], BYTE frame_dlc);
	void netowrk_send_udsmsg(BYTE msg_buf[], WORD msg_dlc);
};

/****************EOF****************/
