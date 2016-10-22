#include "stdafx.h"
#include "UdsUtil.h"


UdsUtil::UdsUtil()
{
}


UdsUtil::~UdsUtil()
{
}


LONG UdsUtil::str2char(CString str, PBYTE ptr)

{

	if (ptr == NULL) return 0;

#ifdef _UNICODE

	LONG len;

	len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);

	WideCharToMultiByte(CP_ACP, 0, str, -1, (LPSTR)ptr, len + 1, NULL, NULL);

#else
	LONG len;

	len = str.GetAllocLength();

	sprintf(ptr, _T("%s"), str);

#endif

	return len;

}

LONG UdsUtil::str2HEX(PBYTE src_str, PBYTE dst_buf)
{

	BYTE hex;
	BYTE char_tmp;
	LONG i, len, dst_pos;

	len = strlen((char *)src_str);

	dst_pos = 0;

	if (len % 2)
	{
		for (i = 0; i < len; i++)
		{

			char_tmp = src_str[i];

			if (' ' == char_tmp || '\n' == char_tmp || '\t' == char_tmp || '\r' == char_tmp)
				break;

			if (char_tmp >= '0' && char_tmp <= '9')
				hex = char_tmp - '0';

			if (char_tmp >= 'a' && char_tmp <= 'z')
				hex = char_tmp - 'a' + 10;

			if (char_tmp >= 'A' && char_tmp <= 'Z')
				hex = char_tmp - 'A' + 10;

			dst_pos = (i + 1) / 2;

			if (i % 2)
			{
				dst_buf[dst_pos] = hex << 4;
			}
			else
			{
				dst_buf[dst_pos] |= hex;
			}
		}
	}
	else
	{
		for (i = 0; i < len; i++)
		{

			char_tmp = src_str[i];

			if (' ' == char_tmp || '\n' == char_tmp || '\t' == char_tmp || '\r' == char_tmp)
				break;

			if (char_tmp >= '0' && char_tmp <= '9')
				hex = char_tmp - '0';

			if (char_tmp >= 'a' && char_tmp <= 'z')
				hex = char_tmp - 'a' + 10;

			if (char_tmp >= 'A' && char_tmp <= 'Z')
				hex = char_tmp - 'A' + 10;

			dst_pos = i / 2;

			if (!(i % 2))
			{
				dst_buf[dst_pos] = hex << 4;
			}
			else
			{
				dst_buf[dst_pos] |= hex;
			}

		}
	}


	return dst_pos + 1;
}