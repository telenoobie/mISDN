/**
	@file
	@brief This file include other C files of function implementions to simplify the project file management. You can add your files (PLATFORMLIB/PROX_Plat_xxxx.C; PLATFORMLIB/xxxxRW_xxxx.C) here.
**/

#include "nxapi.h"

// platform support functions

#ifdef __WIN32__
	#ifdef __KERNEL__ // linux kernel
	#include "PLATFORMLIB/PROX_Plat_Win32_KNL.C"
	#else // __KERNEL__
		#ifdef _CVI_
		#include "PLATFORMLIB/PROX_Plat_WinEVBCVI.C"
		#else // _CVI_
		#include "PLATFORMLIB/PROX_Plat_Win32.C"
		#endif // _CVI_
	#endif // __KERNEL__
#endif // __WIN32__

#ifdef __LINUX__
	#ifdef __KERNEL__ // linux kernel
//	#include "PLATFORMLIB/PROX_Plat_Linux_KNL.C"
	#else // __KERNEL__
	#include "PLATFORMLIB/PROX_Plat_Linux.C"
	#endif // __KERNEL__
#endif // __LINUX__

// Common support functions
//#include "PROXCOMMON/NXSINCOS.C"
//#include "PROXCOMMON/NXMATH.C"

// N68x38x API functions, R/W functions
#if (defined(N681386) || defined(N682386) || defined(N681387) || defined(N682387))

#ifdef _CVI_
#include "PLATFORMLIB/N68X386RW_EVB.C"
#endif // _CVI_

//#include "N68X386/N68X386API.C"

#endif // (defined(N681386) || defined(N682386) || defined(N681387) || defined(N682387))

// W684386 API functions, R/W functions
#if defined(W684386)

#ifdef _CVI_
#include "PLATFORMLIB/W684386RW_EVB.C"
#endif // _CVI_

#include "W684386/W684386API.C"

#endif // (defined(W684386))

// W681388 API functions, R/W functions
#if defined(W681388)

#ifdef _CVI_
#include "PLATFORMLIB/W681388RW_EVB.C"
#endif // _CVI_

#include "W681388/W681388API.C"

#endif // defined(W681388)
