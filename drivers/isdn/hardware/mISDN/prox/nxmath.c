/**
	@file
	@brief This file includes some basic math functions implementations for ProX API.

	@author qkdang@nuvoton.com
*/

// qkdang, 2010

#include "nxapi.h"
#include "nxmath.h"

// bisection
NXFUNIMPSTR __i32 nxSqrt(__i32 val)
{
    __i32 ret=0;
    __i32 s;
    __i32 ret_sq;
	val *= 100;
	ret_sq = -val - 1;
    for(s=30; s>=0; s-=2)
	{
        __i32 b;
        ret += ret;
        b = ret_sq + ((2*ret + 1)<<s);
        if(b<0)
		{
            ret_sq=b;
            ret++;
        }
    }
	ret += 5;
    return ret/10;
}
