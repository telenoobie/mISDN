/**
	@file
	@brief This file includes platform depended functions implementations for ProX API on Linux kernel level.

	@author qkdang@nuvoton.com
*/

#include "nxapi.h"

#include <linux/sched.h>
#include <linux/time.h>
#include <linux/jiffies.h>

NXFUNIMPSTR void nxDelayMS(int valms)
{
	schedule_timeout_uninterruptible(msecs_to_jiffies(valms));
}

NXFUNIMPSTR __u32 nxGetTickCount(void)
{
	struct timespec tvp = CURRENT_TIME;
	return ((tvp.tv_sec*1000) + (tvp.tv_nsec/1000000));
}

NXFUNIMPSTR void nxHardwareInit(__chip_inst pInst, __chip_chnl chnl)
{
	// nothing to do
}

NXFUNIMPSTR void nxAssertReset(__chip_inst pInst, __chip_chnl chnl)
{
	// nothing to do
}

NXFUNIMPSTR void nxReleaseReset(__chip_inst pInst, __chip_chnl chnl)
{
	// nothing to do
}

NXFUNIMPSTR __chip_chnl nxGetCommonChnl(__chip_inst pInst, __chip_chnl chnl)
{
#ifdef TDM400MODULE
	return (chnl/CHIP_CHANNELS(pInst, chnl))*CHIP_CHANNELS(pInst, chnl);
#elif defined(TDM2400MODULE)
	struct wctdm *wc = (struct wctdm *)pInst;
	switch(wc->modtype[chnl])
	{
	case MOD_TYPE_FXS2386:
	case MOD_TYPE_FXS2387:
		return wc->prox_cs_slot[chnl];//(chnl & (~0x01));
	case MOD_TYPE_FXS4386:
		return (chnl & (~0x03));
	default:
		return chnl;
	}
#else
	return 0;
#endif
}

NXFUNIMPSTR int nxIsWideBand(__chip_inst pInst, __chip_chnl chnl)
{
#if WIDE_BAND
	return 1;
#else
	return 0;
#endif
}

NXFUNIMPSTR int nxIsCommonChnl(__chip_inst pInst, __chip_chnl chnl)
{
#ifdef TDM400MODULE
	if((chnl%CHIP_CHANNELS(pInst, chnl)) == 0) return 1;
#elif defined(TDM2400MODULE)
	struct wctdm *wc = (struct wctdm *)pInst;
	switch(wc->modtype[chnl])
	{
	case MOD_TYPE_FXS2386:
	case MOD_TYPE_FXS2387:
		return (chnl == wc->prox_cs_slot[chnl]); // (!(chnl & 0x01));
	case MOD_TYPE_FXS4386:
		return (!(chnl & 0x03));
	default:
		return 1;
	}
#else
	if(chnl == 0) return 1;
#endif
	return 0;
}

NXFUNIMPSTR int nxGetChnlIndex(__chip_inst pInst, __chip_chnl chnl)
{
#ifdef TDM400MODULE
	return (chnl % CHIP_CHANNELS(pInst, chnl));
#elif defined(TDM2400MODULE)
	struct wctdm *wc = (struct wctdm *)pInst;
	switch(wc->modtype[chnl])
	{
	case MOD_TYPE_FXS2386:
	case MOD_TYPE_FXS2387:
		return (chnl & 0x01);
	case MOD_TYPE_FXS4386:
		return (chnl & 0x03);
	default:
		return 0;
	}
#else
	return chnl;
#endif
}

NXFUNIMPSTR void nxSaveCalibData(__chip_inst pInst, __chip_chnl chnl, unsigned char *dat, int len)
{
	// not implemented
}

NXFUNIMPSTR int nxLoadCalibData(__chip_inst pInst, __chip_chnl chnl, unsigned char *dat, int len)
{
	// not implemented
	return -1;
}

NXFUNIMPSTR ProXBATType nxGetBatType(__chip_inst pInst, __chip_chnl chnl)
{
	return BAT_INDUCTOR;
}

NXFUNIMPSTR int nxGetChipChannels(__chip_inst pInst, __chip_chnl chnl)
{
#ifdef TDM400MODULE
	return (chnl % CHIP_CHANNELS(pInst, chnl));
#elif defined(TDM2400MODULE)
	struct wctdm *wc = (struct wctdm *)pInst;
	switch(wc->modtype[chnl])
	{
	case MOD_TYPE_FXS2386:
	case MOD_TYPE_FXS2387:
		return 2;
	case MOD_TYPE_FXS4386:
		return 4;
	default:
		return 1;
	}
#else
	return CHIP_CHANNELS(pInst, chnl);
#endif
}
