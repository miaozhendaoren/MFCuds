#pragma once
#include "NetworkLayer.h"

#define BUF_LEN       1024

#define SID_10        (0x10) /* SessionControl */
#define SID_11        (0x11) /* ECUReset */
#define SID_14        (0x14) /* ClearDTC */
#define SID_18        (0x18) /* KWPReadDTC */
#define SID_19        (0x19) /* ReadDTC */
#define SID_22        (0x22) /* ReadID */
#define SID_27        (0x27) /* SecurityAccess */
#define SID_2E        (0x2E) /* WriteID */
#define SID_2F        (0x2F) /* InputOutputControlID */
#define SID_28        (0x28) /* CommunicationControl */
#define SID_31        (0x31) /* RoutineControl */
#define SID_3E        (0x3E) /* TesterPresent */
#define SID_85        (0x85) /* ControlDTCSetting */

#define SID_10_MIN_LEN      (0x02u)
#define SID_11_MIN_LEN      (0x02u)
#define SID_27_MIN_LEN      (0x02u)
#define SID_28_MIN_LEN      (0x03u)
#define SID_3E_MIN_LEN      (0x02u)
#define SID_85_MIN_LEN      (0x02u)
#define SID_22_MIN_LEN      (0x03u)
#define SID_2E_MIN_LEN      (0x04u)
#define SID_14_MIN_LEN      (0x04u) /* 3 Bytes DTC */
#define SID_19_MIN_LEN      (0x02u)
#define SID_2F_MIN_LEN      (0x04u)
#define SID_31_MIN_LEN      (0x04u)


#define TIMEOUT_FSA          (10000) /* 10s */
#define TIMEOUT_S3server     (5000)  /* 5000ms */
/* uds app layer timer */
typedef enum __UDS_TIMER_T__
{
	UDS_TIMER_FSA = 0,
	UDS_TIMER_S3server,
	UDS_TIMER_CNT
}uds_timer_t;


/* uds security access level */
typedef enum __UDS_SA_LV__
{
	UDS_SA_NON = 0,
	UDS_SA_LV1,
	UDS_SA_LV2,
}uds_sa_lv;



/*******************************************************************************
Function  Definition
*******************************************************************************/

class CUdsClient : public CUdsNetwork
{
public:
	CUdsClient();
	~CUdsClient();

private:
	BYTE RspData[BUF_LEN];
	UINT RspDlc;
	BOOL GetRsp;
	BYTE ReqSid;
	BYTE RspSid;

	/* uds user layer timer */
	UINT uds_timer[UDS_TIMER_CNT];

private:
	void uds_timer_start(BYTE num);
	void uds_timer_stop(BYTE num);
	int uds_timer_run(BYTE num);
	int uds_timer_chk(BYTE num);

protected:
	void ZTai_UDS_Send(BYTE CanData[], BYTE CanDlc);
	void N_USData_ffindication(WORD msg_dlc);
	void N_USData_indication(BYTE msg_buf[], WORD msg_dlc, n_result_t n_result);
	void N_USData_confirm(n_result_t n_result);
public:
    void main_loop(void);
	void request(BYTE SvcId, BYTE DidBuf[], UINT DidLen);
	UINT get_rsp(BYTE DataBuf[], UINT BufLen);
};
