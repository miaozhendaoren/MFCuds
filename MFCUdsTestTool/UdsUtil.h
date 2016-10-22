#pragma once
class UdsUtil
{
public:
	UdsUtil();
	~UdsUtil();
public:
	static LONG str2HEX(PBYTE src_str, PBYTE dst_buf);
	static LONG str2char(CString str, PBYTE ptr);
};

