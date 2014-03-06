/**
	@file
	@brief This file includes sin/cos functions implementations for ProX API.

	@author qkdang@nuvoton.com
*/

// qkdang, 2010

#include "nxapi.h"
#include "nxsincos.h"

#if HAS_FLOAT_POINT

// return (2^20)*sin(2*PI*idx/16000)
NXFUNIMPSTR __u32 nxSin(int idx)
{
	// int N = 16000;
	int i;
	float a = 0.000392699;//sin(2.0f*3.1415926f/N);
	float c = 1.0f;
	float s = 0.0f;
	for(i=0; i<idx; ++i)
	{
		c = c - s*a;
		s = s + c*a;
	}
	return (__u32)(s*1048576);
}

// return (2^20)*cos(2*PI*idx/16000)
NXFUNIMPSTR __u32 nxCos(int idx)
{
	// int N = 16000;
	int i;
	float a = 0.000392699;//sin(2.0f*3.1415926f/N);
	float c = 1.0f;
	float s = 0.0f;
	for(i=0; i<idx; ++i)
	{
		c = c - s*a;
		s = s + c*a;
	}
	return (__u32)(s*1048576);
}

#else // HAS_FLOAT_POINT

// return (2^20)*sin(2*PI*idx/16000)
// param idx <= 4000
NXFUNIMPSTR __u32 nxSin(int idx)
{
	// N = 16000;
	int i;
	__u32 a = 412; // (1<<20)*sin(2.0f*3.1415926f/1600);
	__u32 c = (1<<20);
	__u32 s = 0;
	for(i=0; i<idx; ++i)
	{
		c = c - ((s*a)>>20);
		s = s + ((c*a)>>20);
	}
	return s;
}

// return (2^20)*cos(2*PI*idx/16000)
// param idx <= 4000
NXFUNIMPSTR __u32 nxCos(int idx)
{
	// N = 16000;
	int i;
	__u32 a = 412; // (1<<20)*sin(2.0f*3.1415926f/1600);
	__u32 c = (1<<20);
	__u32 s = 0;
	for(i=0; i<idx; ++i)
	{
		c = c - ((s*a)>>20);
		s = s + ((c*a)>>20);
	}
	return c;
}

#endif // HAS_FLOAT_POINT
