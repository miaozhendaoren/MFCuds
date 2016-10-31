/***************************************************************************//**
\file          RdWrData.c
\author        huanghai
\mail          huanghai@auto-link.com
\version       0.01
\date          2016-10-24
\description   uds service spport code, include read/write data and routine control
*******************************************************************************/
#include "stdafx.h"
#include "RdWrData.h"
#include "UdsClient.h"
#include "UdsUtil.h"

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





int
uds_input_wdidlist(UINT did_n, CString str)
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

	memset(rwdata_list[did_n].p_data, 0,  rwdata_list[did_n].dlc);

	for (i = 0; i < datanum && i < rwdata_list[did_n].dlc; i++)
	{
		rwdata_list[did_n].p_data[i] = data_buf[i];
	}

	return 0;
}
