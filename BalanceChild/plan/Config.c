/****************************************************
file     : config.c  
author   : 
copyright: 2013.9.05~2015.9.05
*****************************************************/
#include "Config.h"

//-----------------------内存管理函数---------------------------------------------
struct MY_MEM _My_mem;

void CreatBuffer(UINT8* src, UINT16 len)
{
	_My_mem._memBlock = src;		
	_My_mem._memSize = len;
	_My_mem._memLeft = len;
	_My_mem._memEmpty = 0;
}

void *My_malloc(UINT16 len)
{
	void* result = 0;
	if(_My_mem._memBlock != 0)
	{
		if(LOAD16(len)) len++;		//len不能为奇数
		if(_My_mem._memLeft >= len)
		{
			result = _My_mem._memBlock + _My_mem._memSize - _My_mem._memLeft;
			_My_mem._memLeft -= len;
		}
		else _My_mem._memEmpty = 1;
	}
	return result;
}

void My_memset(UINT8* src, UINT16 len, UINT8 value)
{
	UINT16 i;
	for(i = 0; i < len; i++) *(src + i) = value;
}

void My_free(void* p)
{
	if((UINT8*)p >= _My_mem._memBlock && (UINT8*)p < (_My_mem._memBlock + _My_mem._memSize))
	{
		_My_mem._memLeft = _My_mem._memSize - ((UINT8*)p - _My_mem._memBlock);
	}
}

struct MY_MEM Get_Mem_Info(void) {return _My_mem;}
//-----------------------End of 内存管理函数---------------------------------------
