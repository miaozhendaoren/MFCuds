/***************************************************************************//**
\file          RdWrData.h
\author        huanghai
\mail          huanghai@auto-link.com
\version       0
\date          2016-10-24
\description
*******************************************************************************/
#pragma once
#include "ColoredListCtrl.h"
#include "afxwin.h"

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


extern const uds_rwdata_t rwdata_list[RWDATA_CNT];

int uds_input_wdidlist(UINT did_n, CString str);
