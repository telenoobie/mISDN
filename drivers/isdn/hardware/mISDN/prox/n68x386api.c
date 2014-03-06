/**
	@file
	@brief API functions implementations for N68x38x.
**/

// qkdang

#include "nxapi.h"
#include "n68x386api.h"
#include "n68x386Regs.h"

#include "n68x386rw.h"
#include "nxsincos.h"
#include "nxmath.h"

#include "n68x386RingParam.h"
#include "n68x386ToneParam.h"
#include "n68x386DTMFGenParam.h"

#define DELAY_MS(arg) nxDelayMS(arg)

#define MABS(arg1, arg2) (((arg1) >= (arg2))?((arg1) - (arg2)):((arg2) - (arg1)))

NXFUNIMPSTR int N68X386HWReset(__chip_inst pInst, __chip_chnl chnl)
{
	int itmo;
	int iRet = -1;
#if USE_HARDWARE_INIT
	nxHardwareInit(pInst, chnl);
	nxAssertReset(pInst, chnl);
	// some delay: 100uS
	for(itmo=0; itmo<10; itmo++)
	{
		nxAssertReset(pInst, chnl);
	}
	nxReleaseReset(pInst, chnl);
#endif // USE_HARDWARE_INIT
	for(itmo=0; itmo<30; itmo++)
	{
		if((n68x386ReadReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R04_PLLS) & 0x01))
		{
			iRet = 1;
			break;
		}
		else
		{
			DELAY_MS(100);
		}
	}
	return iRet;
}

#if DYN_CHIP_CHANNELS

#if WIDE_BAND
#if N681622
	#include "./inc/init_1622/Ch1Init_Inductor1_16k.c"
	#include "./inc/init_1622/Ch1Init_Inductor2_16k.c"
	#include "./inc/init_1622/Ch2Init_Inductor2_16k.c"
#else // N681622
	#include "./inc/init/Ch1Init_Inductor1_16k.c"
	#include "./inc/init/Ch1Init_Inductor2_16k.c"
	#include "./inc/init/Ch2Init_Inductor2_16k.c"
#endif // N681622
#else //WIDE_BAND
#if N681622
	#include "./inc/init_1622/Ch1Init_Inductor1_8k.c"
	#include "./inc/init_1622/Ch1Init_Inductor2_8k.c"
	#include "./inc/init_1622/Ch2Init_Inductor2_8k.c"
#else // N681622
	#include "./inc/init/Ch1Init_Inductor1_8k.c"
	#include "./inc/init/Ch1Init_Inductor2_8k.c"
	#include "./inc/init/Ch2Init_Inductor2_8k.c"
#endif // N681622
#endif // WIDE_BAND

#else // DYN_CHIP_CHANNELS

#if WIDE_BAND
#if N681622
	#if (CHIP_CHANNELS(0, 0) == 1)
		#include "./inc/init_1622/Ch1Init_Inductor1_16k.c"
	#elif (CHIP_CHANNELS(0, 0) == 2)
		#include "./inc/init_1622/Ch1Init_Inductor2_16k.c"
		#include "./inc/init_1622/Ch2Init_Inductor2_16k.c"
	#endif // CHIP_CHANNELS
#else // N681622
	#if (CHIP_CHANNELS(0, 0) == 1)
		#include "./inc/init/Ch1Init_Inductor1_16k.c"
	#elif (CHIP_CHANNELS(0, 0) == 2)
		#include "./inc/init/Ch1Init_Inductor2_16k.c"
		#include "./inc/init/Ch2Init_Inductor2_16k.c"
	#endif
#endif // N681622
#else //WIDE_BAND
#if N681622
	#if (CHIP_CHANNELS(0, 0) == 1)
		#include "./inc/init_1622/Ch1Init_Inductor1_8k.c"
	#elif (CHIP_CHANNELS(0, 0) == 2)
		#include "./inc/init_1622/Ch1Init_Inductor2_8k.c"
		#include "./inc/init_1622/Ch2Init_Inductor2_8k.c"
	#endif // CHIP_CHANNELS
#else // N681622
	#if (CHIP_CHANNELS(0, 0) == 1)
		#include "./inc/init/Ch1Init_Inductor1_8k.c"
	#elif (CHIP_CHANNELS(0, 0) == 2)
		#include "./inc/init/Ch1Init_Inductor2_8k.c"
		#include "./inc/init/Ch2Init_Inductor2_8k.c"
	#endif
#endif // N681622
#endif // WIDE_BAND

#endif // else DYN_CHIP_CHANNELS

NXFUNIMPSTR int N68X386Init(__chip_inst pInst, __chip_chnl chnl)
{
	int iRet = -1;
	ProXBATType btp = nxGetBatType(pInst, chnl);
	switch(btp)
	{
	case BAT_INDUCTOR:
	default:
		//if(nxIsWideBand(pInst, chnl))
#if WIDE_BAND
		{
#if DYN_CHIP_CHANNELS
			switch(nxGetChnlIndex(pInst, chnl))
			{
			case 0:
				if(CHIP_CHANNELS(pInst, chnl) == 1)
				{
					iRet = n68x386WriteRegs(pInst, chnl, 0, X1Ch1Init_Inductor_16k, 0xF0);
				}
				else
				{
					iRet = n68x386WriteRegs(pInst, chnl, 0, X2Ch1Init_Inductor_16k, 0xF0);
				}
				break;
			case 1:
				if(CHIP_CHANNELS(pInst, chnl) == 2)
				{
					iRet = n68x386WriteRegs(pInst, chnl, 0, X2Ch2Init_Inductor_16k, 0xF0);
				}
				break;
			}
#else // DYN_CHIP_CHANNELS
			switch(nxGetChnlIndex(pInst, chnl))
			{
			case 0:
#if (CHIP_CHANNELS(0, 0) == 1)
				iRet = n68x386WriteRegs(pInst, chnl, 0, X1Ch1Init_Inductor_16k, 0xF0);
#else
				iRet = n68x386WriteRegs(pInst, chnl, 0, X2Ch1Init_Inductor_16k, 0xF0);
#endif
				break;
#if (CHIP_CHANNELS(0, 0) == 2)
			case 1:
				iRet = n68x386WriteRegs(pInst, chnl, 0, X2Ch2Init_Inductor_16k, 0xF0);
				break;
#endif
			}
#endif // DYN_CHIP_CHANNELS
		}
		//else
#else // WIDE_BAND
		{
#if DYN_CHIP_CHANNELS
			switch(nxGetChnlIndex(pInst, chnl))
			{
			case 0:
				if(CHIP_CHANNELS(pInst, chnl) == 1)
				{
					iRet = n68x386WriteRegs(pInst, chnl, 0, X1Ch1Init_Inductor_8k, 0xF0);
				}
				else
				{
					iRet = n68x386WriteRegs(pInst, chnl, 0, X2Ch1Init_Inductor_8k, 0xF0);
				}
				break;
			case 1:
				if(CHIP_CHANNELS(pInst, chnl) == 2)
				{
					iRet = n68x386WriteRegs(pInst, chnl, 0, X2Ch2Init_Inductor_8k, 0xF0);
				}
				break;
			}
#else // DYN_CHIP_CHANNELS
			switch(nxGetChnlIndex(pInst, chnl))
			{
			case 0:
#if (CHIP_CHANNELS(0, 0) == 1)
				iRet = n68x386WriteRegs(pInst, chnl, 0, X1Ch1Init_Inductor_8k, 0xF0);
#else
				iRet = n68x386WriteRegs(pInst, chnl, 0, X2Ch1Init_Inductor_8k, 0xF0);
#endif
				break;
#if (CHIP_CHANNELS(0, 0) == 2)
			case 1:
				iRet = n68x386WriteRegs(pInst, chnl, 0, X2Ch2Init_Inductor_8k, 0xF0);
				break;
#endif
			}
#endif // else DYN_CHIP_CHANNELS
		}
#endif // WIDE_BAND
		break;
	}
	return iRet;
}

NXFUNIMPSTR int N68X386BATOn(__chip_inst pInst, __chip_chnl chnl, int onoff)
{
	int vtmp;
	vtmp = n68x386ReadReg(pInst, chnl, NX386_R22_PONC);
	if(vtmp == -1)
	{
		return -1;
	}
	if(onoff)
	{
		vtmp &= ~0x01;
	}
	else
	{
		vtmp |= 0x01;
	}
	return n68x386WriteReg(pInst, chnl, NX386_R22_PONC, vtmp);
}

#define AC_DELAY 10

#if (!N681622)
static void nx386CurrentCalib(__chip_inst pInst, __chip_chnl chnl, int RingNTip)
{
	unsigned char tempReg, min = 0;
	int looper;
	unsigned int temp, minval, Q1I;

	n68x386WriteReg(pInst, chnl, NX386_R43_LAMC, 0x00); // Disable all LAMC 0x43 Automatic transition
	n68x386WriteReg(pInst, chnl, NX386_R44_LSC, 0x0E); // Calibration mode

	switch(RingNTip)
	{
	case 0:    // Trim QT3 current to be set
		tempReg = n68x386ReadReg(pInst, chnl, NX386_RB0_CAL2);
		tempReg &= 0x0F;
		minval = 0xFFF;
		for(looper=15; looper>=0; looper--)
		{
			n68x386WriteReg(pInst, chnl, NX386_RB0_CAL2, (looper<<4)|tempReg);
			DELAY_MS(AC_DELAY);
			temp = n68x386ReadShort(pInst, chnl, NX386_R85_QT3IC);
			Q1I = n68x386ReadShort(pInst, chnl, NX386_R87_QT1IC);
			if(MABS(Q1I, temp) < minval)
			{
				minval = MABS(Q1I, temp);
				min = looper;
			}
		}
		n68x386WriteReg(pInst, chnl, NX386_RB0_CAL2, (min<<4)|tempReg); // //ring current channel three finished
		break;
	case 1:  // Ring Current QR3 current to be set
		tempReg = n68x386ReadReg(pInst, chnl, NX386_RB1_CAL3);
		tempReg &= 0xF0;
		minval = 0xFFF;
		for(looper=15; looper>=0; looper--)
		{
			n68x386WriteReg(pInst, chnl, NX386_RB1_CAL3, looper|tempReg);
			DELAY_MS(AC_DELAY);
			temp = n68x386ReadShort(pInst, chnl, NX386_R86_QR3IC);
			Q1I = n68x386ReadShort(pInst, chnl, NX386_R89_QR1IC);
			if(MABS(Q1I, temp) < minval)
			{
				minval =MABS(Q1I, temp);
				min=looper;
			}
		}
		n68x386WriteReg(pInst, chnl, NX386_RB1_CAL3, min|tempReg);
		break;
	} //switch tr close
}

static void nx386VBATCalib(__chip_inst pInst, __chip_chnl chnl)
{
	unsigned char looper;
	unsigned char target;
	unsigned int QR3, QT3, QR1, QT1;

	//VBAT calibration starts
	n68x386WriteReg(pInst, chnl, NX386_RB0_CAL2, 0xF7);
	n68x386WriteReg(pInst, chnl, NX386_RB1_CAL3, 0x7F);

	target = n68x386ReadReg(pInst, chnl, NX386_RAF_CAL1);
	target &= 0xF0;

	for(looper=0; looper<16; looper++)
	{
		n68x386WriteReg(pInst, chnl, NX386_RAF_CAL1, looper|target);
		DELAY_MS(AC_DELAY);
		QT1 = n68x386ReadShort(pInst, chnl, NX386_R87_QT1IC);
		QR1 = n68x386ReadShort(pInst, chnl, NX386_R89_QR1IC);
		QT3 = n68x386ReadShort(pInst, chnl, NX386_R85_QT3IC);
		QR3 = n68x386ReadShort(pInst, chnl, NX386_R86_QR3IC);
		if(((QR3-QR1) > 32) && ((QT3-QT1)> 32)) break;
	}
}

#endif // !N681622

static void nx386CoarseCalib(__chip_inst pInst, __chip_chnl chnl)
{
	unsigned char minval, temp, vbg, looper;
	unsigned char min = 0;

	n68x386WriteReg(pInst, chnl, NX386_RAC_CC, 0x08); // enable bandgap cal switch

	minval = 0xff;
	for(looper=0; looper<8; looper++)
	{
		n68x386WriteReg(pInst, chnl, NX386_RAC_CC, 0x08|looper);
		DELAY_MS(AC_DELAY);
		temp = n68x386ReadReg(pInst, chnl, NX386_R99_TEMP); // read temp sense REG_TEMP=0x99 
		vbg = n68x386ReadReg(pInst, chnl, NX386_R9A_VBGAP);	// read VBG REG_VBGAP=0x9A
		if(MABS(temp, vbg) < minval)
		{
			minval = MABS(temp, vbg);
			min = looper;
		}
	}
	//if(min < 4) // don't use 1,2,or 3
	//{
	//	min = 0;
	//}
	n68x386WriteReg(pInst, chnl, NX386_RAC_CC, min & ~0x08); // Coarse trim finished
}

NXFUNIMPSTR int N68X386Caliberate(__chip_inst pInst, __chip_chnl chnl)
{
	int iRet = 1;
	
	int olsc, olamc, oxbtc;
	
	olsc = n68x386ReadReg(pInst, chnl, NX386_R44_LSC);
	olamc = n68x386ReadReg(pInst, chnl, NX386_R43_LAMC);
	oxbtc = n68x386ReadReg(pInst, chnl, NX386_R5F_XBTC);

	if((olsc < 0) || (olamc < 0) || (oxbtc < 0))
	{
		return -1;
	}

	n68x386WriteReg(pInst, chnl, NX386_R44_LSC, 0x09); // SET Idle Mode
	n68x386WriteReg(pInst, chnl, NX386_R43_LAMC, 0x07); // SET Auto Mode

	DELAY_MS(10);

	// check hookon status
	if(0x11 == n68x386ReadReg(pInst, chnl, NX386_R44_LSC))
	{
		iRet = -2;
		goto xerr;
	}

	// prepare
	n68x386WriteReg(pInst, chnl, NX386_R44_LSC, 0x09);
	DELAY_MS(50);
	n68x386WriteReg(pInst, chnl, NX386_R44_LSC, 0x02);
	DELAY_MS(50);
	if(n68x386ReadReg(pInst, chnl, NX386_R44_LSC) != 0x22)
	{
		iRet = -3;
		goto xerr;
	}

	// calibrate
	n68x386WriteReg(pInst, chnl, NX386_R5F_XBTC, 0x20); // CLR Bypass

	if(nxIsCommonChnl(pInst, chnl))
	{
		nx386CoarseCalib(pInst, chnl);
	}

#if (!N681622)
	DELAY_MS(200);
	nx386VBATCalib(pInst, chnl);
    nx386CurrentCalib(pInst, chnl, 0); // tip 0	
    nx386CurrentCalib(pInst, chnl, 1); // ring 1
#endif // !N681622

xerr:
	// restore
	n68x386WriteReg(pInst, chnl, NX386_R5F_XBTC, oxbtc); // CLR Bypass
	n68x386WriteReg(pInst, chnl, NX386_R44_LSC, olsc); // SET Idle Mode
	n68x386WriteReg(pInst, chnl, NX386_R43_LAMC, olamc); // SET Auto Mode

	return iRet;
}

static void nx386SetupOSC(__chip_inst pInst, __chip_chnl chnl, unsigned int osc1, unsigned int osic1, unsigned int osat1, unsigned int osit1,
					unsigned int osc2, unsigned int osic2, unsigned int osat2, unsigned int osit2)
{
	// setup tone 1
	unsigned int vt;
	int vr;
	vt = osc1;
	n68x386WriteReg(pInst, chnl, NX386_RC6_OS1CL, vt & 0x0FF);
	n68x386WriteReg(pInst, chnl, NX386_RC7_OS1CH, ((vt & 0x0FF00) >> 8) & 0x0FF);
	vt = osic1;
	n68x386WriteReg(pInst, chnl, NX386_RC2_OS1ICL, vt & 0x0FF);
	n68x386WriteReg(pInst, chnl, NX386_RC3_OS1ICH, ((vt & 0x0FF00) >> 8) & 0x0FF);
	vt = osat1;
	n68x386WriteReg(pInst, chnl, NX386_RCA_OS1ATL, vt & 0x0FF);
	n68x386WriteReg(pInst, chnl, NX386_RCB_OS1ATH, ((vt & 0x0FF00) >> 8) & 0x0FF);
	vt = osit1;
	n68x386WriteReg(pInst, chnl, NX386_RCE_OS1ITL, vt & 0x0FF);
	n68x386WriteReg(pInst, chnl, NX386_RCF_OS1ITH, ((vt & 0x0FF00) >> 8) & 0x0FF);

	// setup tone 2
	vt = osc2;
	n68x386WriteReg(pInst, chnl, NX386_RC8_OS2CL, (vt & 0xFF));
	n68x386WriteReg(pInst, chnl, NX386_RC9_OS2CH, ((vt & 0xFF00) >> 8) & 0x0FF);
	vr = n68x386ReadReg(pInst, chnl, NX386_RDC_ROFFS);
	if(vr >= 0)
	{
		vr &= 0x3F;
		n68x386WriteReg(pInst, chnl, NX386_RDC_ROFFS, vr);
	}
	vt = osic2;
	n68x386WriteReg(pInst, chnl, NX386_RC4_OS2ICL, vt & 0x0FF);
	n68x386WriteReg(pInst, chnl, NX386_RC5_OS2ICH, ((vt & 0x0FF00) >> 8) & 0x0FF);
	vt = osat2;
	n68x386WriteReg(pInst, chnl, NX386_RCC_OS2ATL, vt & 0x0FF);
	n68x386WriteReg(pInst, chnl, NX386_RCD_OS2ATH, ((vt & 0x0FF00) >> 8) & 0x0FF);
	vt = osit2;
	n68x386WriteReg(pInst, chnl, NX386_RD0_OS2ITL, vt & 0x0FF);
	n68x386WriteReg(pInst, chnl, NX386_RD1_OS2ITH, ((vt & 0x0FF00) >> 8) & 0x0FF);

	n68x386WriteReg(pInst, chnl, NX386_RAD_OS2RPD, 0x00);
}

static void nx386StartOSC(__chip_inst pInst, __chip_chnl chnl, int en1, int en2)
{
	// start
	// tx path
	/*
	int vt = n68x386ReadReg(pInst, chnl, NX386_RC1_RMPC);
	if(vt >= 0)
	{
		vt |= 0x08;
		n68x386WriteReg(pInst, chnl, NX386_RC1_RMPC, vt);
	}
	*/
	int vt = 0;
	n68x386WriteReg(pInst, chnl, NX386_RC1_RMPC, vt);
	// en
	vt = 0x08 | 0x04;
	if(en1)
	{
		vt |= 0x01;
	}
	if(en2)
	{
		vt |= 0x02;
	}
	n68x386WriteReg(pInst, chnl, NX386_RC0_OSNC, vt);
};

NXFUNIMPSTR int N68X386SetupRingtone(__chip_inst pInst, __chip_chnl chnl, enum tagRingRegion xregion)
{
	int idx;
	int on_time;

	idx = xregion - enumFirstValue;
	on_time = RingtoneValues_x386[idx].OSC2_OnTime;

	nx386SetupOSC(pInst, chnl,
		0, 0,
		0, 0,
		RingtoneValues_x386[idx].OSC2_FREQ, RingtoneValues_x386[idx].OSC2_AMP,
		on_time, RingtoneValues_x386[idx].OSC2_OffTime);
	// VBH
	N68X386SetVBHV(pInst, chnl, (RingtoneValues_x386[idx].Vpeak + 10)*1000);
	// RTDC
	n68x386WriteReg(pInst, chnl, NX386_R51_RTDFCLD, RingtoneValues_x386[idx].RTDC & 0x0FF);
	n68x386WriteReg(pInst, chnl, NX386_R52_DCHD, 
		(n68x386ReadReg(pInst, chnl, NX386_R52_DCHD) & 0x0F) | ((RingtoneValues_x386[idx].RTDC & 0x0F00) >> 4));
	// RTDB
	n68x386WriteReg(pInst, chnl, NX386_R48_RTDBA, RingtoneValues_x386[idx].RTDB);
	n68x386WriteReg(pInst, chnl, NX386_R68_RTDBD, RingtoneValues_x386[idx].RTDB);

	on_time = on_time * 1000 / (10*0x0320);
	return on_time;
}

NXFUNIMPSTR int N68X386SetupRing(__chip_inst pInst, __chip_chnl chnl, int freq, __u32 vpeak, __u32 msecOn, __u32 msecOff,
					__u32 dcv, __u32 usecOffset)
{
	__u32 ft, apeak;
	__u32 vt32;
	int vt;

	vt = 0;
	n68x386WriteReg(pInst, chnl, NX386_RC1_RMPC, vt);

	ft = nxCos(2*freq); // (2^20)*cos(2*PI*idx/16000)
	ft >>= 3; // we want (2^17)*cos(2*PI*freq/8000)
	vt32 = ft;
	n68x386WriteReg(pInst, chnl, NX386_RC8_OS2CL, (int)((vt32 >> 2) & 0x0FF));
	n68x386WriteReg(pInst, chnl, NX386_RC9_OS2CH, (int)((((vt32 >> 2) & 0x0FF00) >> 8) & 0x0FF));

	vt = 0;
	vt32 &= 0x03;
	// dc offset
	vt |= (vt32 << 6);
	vt |= dcv/1484;
	n68x386WriteReg(pInst, chnl, NX386_RDC_ROFFS, vt);

	if(vpeak > 93000) vpeak = 93000;

	apeak = vpeak;
	ft = nxSin(2*freq); // (2^20)*sin(2*PI*idx/16000)
	ft >>= 5; // we want (2^15)*sin(2*PI*freq/8000)
	apeak *= ft;
	apeak /= 96000;

	n68x386WriteReg(pInst, chnl, NX386_RC4_OS2ICL, (int)(apeak & 0x0FF));
	n68x386WriteReg(pInst, chnl, NX386_RC5_OS2ICH, (int)(((apeak & 0x0FF00) >> 8) & 0x0FF));

	if(msecOn > 8000) msecOn = 8000;
	if(msecOff > 8000) msecOff = 8000;

	ft = msecOn;
	ft <<= 3; // ft *= 8 : ft /= 0.125
	n68x386WriteReg(pInst, chnl, NX386_RCC_OS2ATL, (int)(ft & 0x0FF));
	n68x386WriteReg(pInst, chnl, NX386_RCD_OS2ATH, (int)(((ft & 0x0FF00) >> 8) & 0x0FF));
	ft = msecOff;
	ft <<= 3; // ft *= 8 : ft /= 0.125
	n68x386WriteReg(pInst, chnl, NX386_RD0_OS2ITL, (int)(ft & 0x0FF));
	n68x386WriteReg(pInst, chnl, NX386_RD1_OS2ITH, (int)(((ft & 0x0FF00) >> 8) & 0x0FF));

	// phase offset
	if(usecOffset > 32000)usecOffset = 32000;
	ft = usecOffset;
	ft /= 125;
	n68x386WriteReg(pInst, chnl, NX386_RAD_OS2RPD, (int)ft);

	return 1;
}

NXFUNIMPSTR int N68X386SetRingActiveTime(__chip_inst pInst, __chip_chnl chnl, __u32 msecOn)
{
	__u32 ft;

	if(msecOn > 8000) msecOn = 8000;

	ft = msecOn;
	ft <<= 3; // ft *= 8 : ft /= 0.125
	n68x386WriteReg(pInst, chnl, NX386_RCC_OS2ATL, (int)(ft & 0x0FF));
	n68x386WriteReg(pInst, chnl, NX386_RCD_OS2ATH, (int)(((ft & 0x0FF00) >> 8) & 0x0FF));
	
	return 0;
}

NXFUNIMPSTR int N68X386SetRingInactiveTime(__chip_inst pInst, __chip_chnl chnl, __u32 msecOff)
{
	__u32 ft;

	if(msecOff > 8000) msecOff = 8000;

	ft = msecOff;
	ft <<= 3; // ft *= 8 : ft /= 0.125
	n68x386WriteReg(pInst, chnl, NX386_RD0_OS2ITL, (int)(ft & 0x0FF));
	n68x386WriteReg(pInst, chnl, NX386_RD1_OS2ITH, (int)(((ft & 0x0FF00) >> 8) & 0x0FF));
	
	return 0;
}

NXFUNIMPSTR int N68X386SetupTrapRing(__chip_inst pInst, __chip_chnl chnl, int freq, __u32 vpeak, __u32 msecOn, __u32 msecOff,
					__u32 dcv, __u32 usecOffset, int crest_factor100)
{
	__u32 trise, ft;
	__u32 vt32;
	int vt;
	//unsigned int ut;

	if(crest_factor100 < 100)crest_factor100 = 100;

	trise = crest_factor100; // /100
	trise *= trise; // /10000
	trise = 4000 - (40000000/trise); // /4000
	trise *= 375; // /1000*4000
	trise /= freq;
	trise /= 100; // now trise has to /40000

	vt = (0x80|0x40);
	n68x386WriteReg(pInst, chnl, NX386_RC1_RMPC, vt);

	if(vpeak > 93000) vpeak = 93000;

	ft = vpeak;
	ft *= 32768;
	ft /= 96; // /1000
	//ft /= 8000; //
	//ft *= 40000;
	ft *= 5;
	ft /= trise;
	ft /= 1000;
	vt32 = ft;
	n68x386WriteReg(pInst, chnl, NX386_RC8_OS2CL, (int)((vt32 >> 2) & 0x0FF));
	n68x386WriteReg(pInst, chnl, NX386_RC9_OS2CH, (int)((((vt32 >> 2) & 0x0FF00) >> 8) & 0x0FF));

	vt = 0;
	// vt32 &= 0x03;
	// dc offset
	// vt |= (vt32 << 6);
	vt |= dcv/1484;
	n68x386WriteReg(pInst, chnl, NX386_RDC_ROFFS, vt);

	ft = trise/5;
	n68x386WriteReg(pInst, chnl, NX386_RC4_OS2ICL, (int)(ft & 0x0FF));

	ft = 40000/freq;
	ft /= 2;
	ft -= trise;
	ft -= trise;
	ft /= 5;
	n68x386WriteReg(pInst, chnl, NX386_RC5_OS2ICH, (int)(ft & 0x0FF));

	if(msecOn > 8000) msecOn = 8000;
	if(msecOff > 8000) msecOff = 8000;

	ft = msecOn;
	ft <<= 3; // ft *= 8 : ft /= 0.125
	n68x386WriteReg(pInst, chnl, NX386_RCC_OS2ATL, (int)(ft & 0x0FF));
	n68x386WriteReg(pInst, chnl, NX386_RCD_OS2ATH, (int)(((ft & 0x0FF00) >> 8) & 0x0FF));
	ft = msecOff;
	ft <<= 3; // ft *= 8 : ft /= 0.125
	n68x386WriteReg(pInst, chnl, NX386_RD0_OS2ITL, (int)(ft & 0x0FF));
	n68x386WriteReg(pInst, chnl, NX386_RD1_OS2ITH, (int)(((ft & 0x0FF00) >> 8) & 0x0FF));

	// phase offset
	if(usecOffset > 32000)usecOffset = 32000;
	ft = usecOffset;
	ft /= 125;
	n68x386WriteReg(pInst, chnl, NX386_RAD_OS2RPD, (int)ft);

	return 1;
}

NXFUNIMPSTR int N68X386SetRingPhase(__chip_inst pInst, __chip_chnl chnl, int uSec, int zcEna)
{
	int vt;
	__u32 vt32 = uSec;
	vt32 /= 125;
	n68x386WriteReg(pInst, chnl, NX386_RAD_OS2RPD, (int)vt32);
	vt = n68x386ReadReg(pInst, chnl, NX386_RC0_OSNC);
	if(vt >= 0)
	{
		if(zcEna)
		{
			vt |= 0x20;
		}
		else
		{
			vt &= ~0x20;
		}
		return n68x386WriteReg(pInst, chnl, NX386_RC0_OSNC, vt);
	}
	return -1;
}

NXFUNIMPSTR int N68X386SetVBHV(__chip_inst pInst, __chip_chnl chnl, __u32 valmV)
{
	int vt;
	vt = n68x386ReadReg(pInst, chnl, NX386_R4E_VBHVC);
	if(vt >= 0)
	{
		if(valmV > 93500)valmV = 93500;
		vt &= (0x80 | 0x40);
		vt |= (valmV/1484);
		n68x386WriteReg(pInst, chnl, NX386_R4E_VBHVC, vt);
		return 1;
	}
	return -1;
}

NXFUNIMPSTR int N68X386SetVCMR(__chip_inst pInst, __chip_chnl chnl, __u32 valmV)
{
	int vt;
	vt = n68x386ReadReg(pInst, chnl, NX386_R42_VCMRC);
	if(vt >= 0)
	{
		if(valmV > 93500)valmV = 93500;
		vt &= 0x00;
		vt |= (valmV/1484);
		n68x386WriteReg(pInst, chnl, NX386_R42_VCMRC, vt);
		return 1;
	}
	return -1;
}

NXFUNIMPSTR int N68X386SetVROFF(__chip_inst pInst, __chip_chnl chnl, __u32 valmV)
{
	int vt;
	vt = n68x386ReadReg(pInst, chnl, NX386_RDC_ROFFS);
	if(vt >= 0)
	{
		if(valmV > 47500)valmV = 47500;
		vt &= 0xE0;
		vt |= (valmV/1484);
		n68x386WriteReg(pInst, chnl, NX386_RDC_ROFFS, vt);
		return 1;
	}
	return -1;
}

NXFUNIMPSTR int N68X386SetUnbalancedRing(__chip_inst pInst, __chip_chnl chnl, int ena, int onTip)
{
	return -1;
}

NXFUNIMPSTR int N68X386SetupRingTripDetect(__chip_inst pInst, __chip_chnl chnl, int currentLim, int freqLP, int intena)
{
	__u32 ft;
	int vt, v;
	ft = currentLim*1000;
	ft /= 1270;
	vt = (int)ft;
	n68x386WriteReg(pInst, chnl, NX386_R55_RTTA, vt);
	n68x386WriteReg(pInst, chnl, NX386_R67_RTTD, vt);

	ft = freqLP;
	ft = 1000*1000/ft;
	ft /= 1250;
	ft /= 4;
	vt = (int)ft;
	n68x386WriteReg(pInst, chnl, NX386_R48_RTDBA, vt);
	n68x386WriteReg(pInst, chnl, NX386_R68_RTDBD, vt);

	ft = 6434*freqLP;
	ft /= 800;
	ft = 1024 - ft;
	ft *= 4; // 2^12;
	vt = (int)ft;
	//n68x386WriteReg(pInst, chnl, NX386_R65_RTDFCLD, (vt & 0x0FF));
	//n68x386WriteReg(pInst, chnl, NX386_R66_DCHD, ((vt & 0x0F00) >> 8));
	n68x386WriteReg(pInst, chnl, NX386_R51_RTDFCLD, (vt & 0x0FF));
	v = n68x386ReadReg(pInst, chnl, NX386_R52_DCHD);
	if(v >= 0)
	{
		v &= 0x0F;
		v |= ((vt & 0x0F00) >> 4);
		n68x386WriteReg(pInst, chnl, NX386_R52_DCHD, v);
	}
	return 0;
}

NXFUNIMPSTR int N68X386SetupRingTripCurrent(__chip_inst pInst, __chip_chnl chnl, int currentLim)
{
	__u32 ft;
	int vt;
	ft = currentLim*1000;
	ft /= 1270;
	vt = (int)ft;
	n68x386WriteReg(pInst, chnl, NX386_R55_RTTA, vt);
	n68x386WriteReg(pInst, chnl, NX386_R67_RTTD, vt);
	return 0;
}

NXFUNIMPSTR int N68X386Ring(__chip_inst pInst, __chip_chnl chnl, int en, int intena, int inteni)
{
	int v = n68x386ReadReg(pInst, chnl, NX386_R29_IE2);
	if(v >= 0)
	{
		v &= 0x080;
		if(intena)
		{
			v |= 0x10;
		}
		if(inteni)
		{
			v |= 0x20;
		}
		n68x386WriteReg(pInst, chnl, NX386_R29_IE2, v);
	}
	v = n68x386ReadReg(pInst, chnl, NX386_R43_LAMC);
	if(v >= 0)
	{
		if(!(v & 0x02)) v = -1;
	}
	if(v == -1) // manual
	{
		if(en)
		{
			N68X386SetLineStatus(pInst, chnl, LS_RINGING);
			// NX386_RC1_RMPC
			v = n68x386ReadReg(pInst, chnl, NX386_RC1_RMPC);
			if(v >= 0)
			{
				v &= 0xC0;
				v |= 0x20;
				n68x386WriteReg(pInst, chnl, NX386_RC1_RMPC, v);
			}
			// en NX386_RC0_OSNC
			v = 0x08 | 0x04;
			v |= 0x02;
			n68x386WriteReg(pInst, chnl, NX386_RC0_OSNC, v);
		}
		else
		{
			// disen NX386_RC0_OSNC
			v = 0x08 | 0x04;
			n68x386WriteReg(pInst, chnl, NX386_RC0_OSNC, v);
			// NX386_RC1_RMPC
			v = n68x386ReadReg(pInst, chnl, NX386_RC1_RMPC);
			if(v >= 0)
			{
				v &= 0xC0;
				n68x386WriteReg(pInst, chnl, NX386_RC1_RMPC, v);
			}
			//N68X386SetLineStatusTo(pInst, chnl, LS_FORWARD_ONHOOKTRANS);
//			N68X386SetLineStatusTo(pInst, chnl, LS_FORWARD_IDLE);
			// Jolly's PATCH
			N68X386SetLineStatus(pInst, chnl, LS_FORWARD_IDLE);
		}
	}
	else // auto
	{
		if(en)
		{
			N68X386SetLineStatus(pInst, chnl, LS_RINGING);
		}
		else
		{
			//N68X386SetLineStatusTo(pInst, chnl, LS_FORWARD_ONHOOKTRANS);
//			N68X386SetLineStatusTo(pInst, chnl, LS_FORWARD_IDLE);
			// Jolly's PATCH
			N68X386SetLineStatus(pInst, chnl, LS_FORWARD_IDLE);
		}
	}

	return 1;
}

NXFUNIMPSTR int N68X386GetRTLCStatus(__chip_inst pInst, __chip_chnl chnl)
{
	int v = n68x386ReadReg(pInst, chnl, NX386_R46_RTDSLC);
	if(v >= 0)
	{
		return (v & 0x01F);
	}
	return v;
}

NXFUNIMPSTR int N68X386GetLineStatus(__chip_inst pInst, __chip_chnl chnl)
{
	int v = n68x386ReadReg(pInst, chnl, NX386_R44_LSC);
	if(v >= 0)
	{
		return ((v & 0xF0) >> 4);
	}
	return v;
}

NXFUNIMPSTR int N68X386SetLineStatus(__chip_inst pInst, __chip_chnl chnl, NXLINESTATUS st)
{
	return n68x386WriteReg(pInst, chnl, NX386_R44_LSC, st & 0x0F);
}

NXFUNIMPSTR int N68X386SetLineStatusTo(__chip_inst pInst, __chip_chnl chnl, NXLINESTATUS st)
{
	int v;
	int iTimeo = 3;
	st &= 0x0F;
	v = n68x386WriteReg(pInst, chnl, NX386_R44_LSC, st);
	if(v >= 0)
	{
		st <<= 4;
		while(iTimeo --)
		{
			v = n68x386ReadReg(pInst, chnl, NX386_R44_LSC);
			if(v >= 0)
			{
				if((v & 0xF0) == st)
				{
					break;
				}
			}
			else
			{
				return v;
			}
			DELAY_MS(50);
		}
		if(iTimeo)
		{
			return ((st >> 4) + 1);
		}
		return 0;
	}
	return v;
}

NXFUNIMPSTR int N68X386IsLinePositive(__chip_inst pInst, __chip_chnl chnl)
{
	int vt;
	vt = n68x386ReadReg(pInst, chnl, NX386_R91_POLC);
	if(vt < 0)
	{
		return vt;
	}
	return (!(vt & 0x01));
}

#ifndef PROXADCDACGAINTABLE
#define PROXADCDACGAINTABLE 1

#if GAIN_STEP_0DOT5
// -41dB ~ 6dB; 1024*10(xdb/20)
static const unsigned short ADCDACGAIN[] =
{
	9, 10, 10, 11, 11, 12, 13, 14, 14, 15,
	16, 17, 18, 19, 20, 22, 23, 24, 26, 27,
	29, 31, 32, 34, 36, 38, 41, 43, 46, 48,
	51, 54, 58, 61, 65, 68, 72, 77, 81, 86,
	91, 97, 102, 108, 115, 122, 129, 137, 145, 153,
	162, 172, 182, 193, 204, 216, 229, 243, 257, 272,
	289, 306, 324, 343, 363, 385, 408, 432, 457, 485,
	513, 544, 576, 610, 646, 684, 725, 768, 813, 862,
	913, 967, 1024, 1085, 1149, 1217, 1289, 1366, 1446,
	1532, 1623, 1719, 1821, 1929, 2043
};
#else // GAIN_STEP_0DOT5
// -41dB ~ 6dB; 1024*10(xdb/20)
static const unsigned short ADCDACGAIN[] =
{
	9, 10, 11, 13, 14, 16, 18, 20, 23, 26,
	29, 32, 36, 41, 46, 51,	58, 65, 72, 81,
	91, 102, 115, 129, 145, 162, 182, 204, 229, 257,
	289, 234, 363, 408, 457, 513, 576, 646, 725, 813,
	913, 1024, 1149, 1289, 1446, 1623, 1821, 2043
};
#endif // GAIN_STEP_0DOT5

#endif // PROXADCDACGAINTABLE

NXFUNIMPSTR int N68X386SetTxDACGain(__chip_inst pInst, __chip_chnl chnl, int valdB)
{
	int vt;
	vt = n68x386ReadReg(pInst, chnl, NX386_RDF_DGH);
	if(vt < 0)
	{
		return vt;
	}

#if GAIN_STEP_0DOT5
	if(valdB < -410) valdB = -410;
	if(valdB > 60) valdB = 60;
	valdB += 410;
	valdB /= 5;
#else // GAIN_STEP_0DOT5
	if(valdB < -41) valdB = -41;
	if(valdB > 6) valdB = 6;
	valdB += 41;
#endif // GAIN_STEP_0DOT5

	n68x386WriteReg(pInst, chnl, NX386_RDE_DACL, (ADCDACGAIN[valdB] & 0x0FF));
	vt &= 0x0F;
	vt |= ((ADCDACGAIN[valdB] >> 4) & 0x0F0);

	n68x386WriteReg(pInst, chnl, NX386_RDF_DGH, vt);

	return 0;
}

NXFUNIMPSTR int N68X386SetRxADCGain(__chip_inst pInst, __chip_chnl chnl, int valdB)
{
	int vt;
	vt = n68x386ReadReg(pInst, chnl, NX386_RDF_DGH);
	if(vt < 0)
	{
		return vt;
	}

#if GAIN_STEP_0DOT5
	if(valdB < -410) valdB = -410;
	if(valdB > 60) valdB = 60;
	valdB += 410;
	valdB /= 5;
#else // GAIN_STEP_0DOT5
	if(valdB < -41) valdB = -41;
	if(valdB > 6) valdB = 6;
	valdB += 41;
#endif // GAIN_STEP_0DOT5

	n68x386WriteReg(pInst, chnl, NX386_RDD_ADCL, (ADCDACGAIN[valdB] & 0x0FF));
	vt &= 0xF0;
	vt |= ((ADCDACGAIN[valdB] >> 8) & 0x0F);

	n68x386WriteReg(pInst, chnl, NX386_RDF_DGH, vt);

	return 0;
}

typedef struct tagIMZValue
{
	unsigned char regx41; /* value of reg 0x41, hb/gain, for US PBX, Korea, Taiwan and Standard line it should be 0x5B, others 0x0F */
	unsigned char regxA8; /* value of reg 0xA8, IM1, for Standard, it is 0x01, others 0x00 */
	unsigned char regxF4;
	unsigned char regxFB; /* value of reg 0xFB, value FF for no opt */
	unsigned char regxDD; /* Low Byte of ADC Gain */
	unsigned char regxDE; /* Low Byte of DAC Gain */
	unsigned char regxDF; /* High Half Bytes of DAC and ADC Gain */
	unsigned char depthF3_H; /* 276: High Byte count of values to write to FIFO 0xF3 */
	unsigned char depthF3_L; /* 276: Low Byte count of values to write to FIFO 0xF3 */
	unsigned char regxF3[1];
}imzValue;

#if CONFIG_IMZ_Japan_CO
	#include "./inc/imz/IMF02.c"
#endif
#if CONFIG_IMZ_Bellcore
	#include "./inc/imz/IMF03.c"
#endif
#if CONFIG_IMZ_CTR21
	#include "./inc/imz/IMF04.c"
#endif
#if CONFIG_IMZ_China_CO
	#include "./inc/imz/IMF05.c"
#endif
#if CONFIG_IMZ_China_PBX
	#include "./inc/imz/IMF06.c"
#endif
#if CONFIG_IMZ_Japan_PBX
	#include "./inc/imz/IMF07.c"
#endif
#if CONFIG_IMZ_India_NewZealand
	#include "./inc/imz/IMF08.c"
#endif
#if CONFIG_IMZ_Germany_Legacy
	#include "./inc/imz/IMF09.c"
#endif
#if CONFIG_IMZ_UK_Legacy
	#include "./inc/imz/IMF10.c"
#endif
#if CONFIG_IMZ_Australia
	#include "./inc/imz/IMF11.c"
#endif
#if CONFIG_IMZ_Variation
	#include "./inc/imz/IMF12.c"
#endif

static const unsigned char * imzRegionValues[] =
{
	0, 0,
#if CONFIG_IMZ_Japan_CO
	vIMF02,
#endif
#if CONFIG_IMZ_Bellcore
	vIMF03,
#endif
#if CONFIG_IMZ_CTR21
	vIMF04,
#endif
#if CONFIG_IMZ_China_CO
	vIMF05,
#endif
#if CONFIG_IMZ_China_PBX
	vIMF06,
#endif
#if CONFIG_IMZ_Japan_PBX
	vIMF07,
#endif
#if CONFIG_IMZ_India_NewZealand
	vIMF08,
#endif
#if CONFIG_IMZ_Germany_Legacy
	vIMF09,
#endif
#if CONFIG_IMZ_UK_Legacy
	vIMF10,
#endif
#if CONFIG_IMZ_Australia
	vIMF11,
#endif
#if CONFIG_IMZ_Variation
	vIMF12,
#endif
};

#if MULTICHNL_SEPERATE_IMZ
NXFUNIMPSTR int N68X386SetLineImpendence(__chip_inst pInst, __chip_chnl chnl, enum tagIMZRegion xregion)
{
	imzValue *pvt0;
#if (CHIP_CHANNELS(0, 0) != 1)
	imzValue *pvt1;
	__chip_chnl chnl1;
#endif
	unsigned short cnt;
	unsigned short idx;

	if(xregion < 0 || xregion >= IMZ_Max)
	{
		return -1;
	}

	n68x386WriteReg(pInst, chnl, NX386_RA9_IM2C, 0);
	n68x386WriteReg(pInst, chnl, NX386_RA8_IM1C, xregion);

	n68x386WriteReg(pInst, chnl, 0xF5, 0x00);

	pvt0 = (imzValue*)imzRegionValues[xregion];
#if (CHIP_CHANNELS(0, 0) != 1)
	chnl1 = nxGetNextChnl(pInst, chnl);
	pvt1 = (imzValue*)imzRegionValues[nxGetIMZRegion(pInst, chnl1)];
#endif
	if(pvt0)
	{
		n68x386WriteReg(pInst, chnl, 0x41, pvt0->regx41);
		n68x386WriteReg(pInst, chnl, 0xA8, pvt0->regxA8);
		n68x386WriteReg(pInst, chnl, 0xF4, pvt0->regxF4);
		if(pvt0->regxFB != 0x0FF)
		{
			n68x386WriteReg(pInst, chnl, 0xFB, pvt0->regxFB);
		}
		n68x386WriteReg(pInst, chnl, 0xDD, pvt0->regxDD);
		n68x386WriteReg(pInst, chnl, 0xDE, pvt0->regxDE);
		n68x386WriteReg(pInst, chnl, 0xDF, pvt0->regxDF);

		cnt = pvt0->depthF3_H;
		cnt <<= 8;
		cnt |= pvt0->depthF3_L;

		n68x386WriteReg(pInst, chnl, 0xF5, 0x11);

#if (CHIP_CHANNELS(0, 0) == 1)
		for(idx=0; idx<cnt; idx++)
		{
			n68x386WriteReg(pInst, chnl, 0xF3, pvt0->regxF3[idx]);
		}
#else
		if(nxIsCommonChnl(chnl))
		{
			// write chnl, then another channel
			cnt = 138;
			for(idx=0; idx<cnt; idx++)
			{
				n68x386WriteReg(pInst, chnl, 0xF3, pvt0->regxF3[idx]);
			}
			if(pvt1)
			{
				// write the other channel
				cnt = 138;
				for(idx=0; idx<cnt; idx++)
				{
					n68x386WriteReg(pInst, chnl, 0xF3, pvt1->regxF3[idx]);
				}
			}
			else
			{
				// repeat
				cnt = 138;
				for(idx=0; idx<cnt; idx++)
				{
					n68x386WriteReg(pInst, chnl, 0xF3, pvt0->regxF3[idx]);
				}
			}
		}
		else
		{
			// write another chnl, then this channel
			n68x386WriteReg(pInst, chnl1, 0xF5, 0x11);
			if(pvt1)
			{
				// write the other channel
				cnt = 138;
				for(idx=0; idx<cnt; idx++)
				{
					n68x386WriteReg(pInst, chnl1, 0xF3, pvt1->regxF3[idx]);
				}
			}
			else
			{
				// repeat
				cnt = 138;
				for(idx=0; idx<cnt; idx++)
				{
					n68x386WriteReg(pInst, chnl1, 0xF3, pvt0->regxF3[idx]);
				}
			}
			cnt = 138;
			for(idx=0; idx<cnt; idx++)
			{
				n68x386WriteReg(pInst, chnl1, 0xF3, pvt0->regxF3[idx]);
			}
			if(pvt1)
			{
				n68x386WriteReg(pInst, chnl1, 0xF5, 0x04);
			}
			else
			{
				n68x386WriteReg(pInst, chnl1, 0xF5, 0x00);
			}
		}
#endif
		n68x386WriteReg(pInst, chnl, 0xF5, 0x04);
	}
	else
	{
		n68x386WriteReg(pInst, chnl, NX386_R41_HBC, 0x5B);
	}

	// store setings to context
	//
	
	return 1;
}
#else // MULTICHNL_SEPERATE_IMZ
NXFUNIMPSTR int N68X386SetLineImpendence(__chip_inst pInst, __chip_chnl chnl, enum tagIMZRegion xregion)
{
	imzValue *pvt;
	unsigned short cnt;
	unsigned short idx;

	if(xregion < 0 || xregion >= IMZ_Max)
	{
		return -1;
	}

	n68x386WriteReg(pInst, chnl, NX386_RA9_IM2C, 0);
	n68x386WriteReg(pInst, chnl, NX386_RA8_IM1C, xregion);

	n68x386WriteReg(pInst, chnl, 0xF5, 0x00);

	pvt = (imzValue*)imzRegionValues[xregion];
	if(pvt)
	{
		n68x386WriteReg(pInst, chnl, 0x41, pvt->regx41);
		n68x386WriteReg(pInst, chnl, 0xA8, pvt->regxA8);
		n68x386WriteReg(pInst, chnl, 0xF4, pvt->regxF4);
		if(pvt->regxFB != 0x0FF)
		{
			n68x386WriteReg(pInst, chnl, 0xFB, pvt->regxFB);
		}
		n68x386WriteReg(pInst, chnl, 0xDD, pvt->regxDD);
		n68x386WriteReg(pInst, chnl, 0xDE, pvt->regxDE);
		n68x386WriteReg(pInst, chnl, 0xDF, pvt->regxDF);

		cnt = pvt->depthF3_H;
		cnt <<= 8;
		cnt |= pvt->depthF3_L;

		n68x386WriteReg(pInst, chnl, 0xF5, 0x11);
#if (CHIP_CHANNELS(0, 0) == 1)
		for(idx=0; idx<cnt; idx++)
		{
			n68x386WriteReg(pInst, chnl, 0xF3, pvt->regxF3[idx]);
		}
#else
		cnt = 138;
		for(idx=0; idx<cnt; idx++)
		{
			n68x386WriteReg(pInst, chnl, 0xF3, pvt->regxF3[idx]);
		}
		// repeat
		cnt = 138;
		for(idx=0; idx<cnt; idx++)
		{
			n68x386WriteReg(pInst, chnl, 0xF3, pvt->regxF3[idx]);
		}
#endif
		n68x386WriteReg(pInst, chnl, 0xF5, 0x04);
	}
	else
	{
		n68x386WriteReg(pInst, chnl, NX386_R41_HBC, 0x5B);
	}
	return 1;
}
#endif // MULTICHNL_SEPERATE_IMZ

NXFUNIMPSTR int N68X386SetVGM(__chip_inst pInst, __chip_chnl chnl, __u32 valmV)
{
	int vt;
	vt = n68x386ReadReg(pInst, chnl, NX386_R4D_GMVC);
	if(vt >= 0)
	{
		if(valmV > 93500)valmV = 93500;
		vt &= 0xC0;
		vt |= (valmV/1484);
		n68x386WriteReg(pInst, chnl, NX386_R4D_GMVC, vt);
		return 1;
	}
	return -1;
}

NXFUNIMPSTR int N68X386SetVOV(__chip_inst pInst, __chip_chnl chnl, __u32 valmV)
{
	int vt;
	vt = n68x386ReadReg(pInst, chnl, NX386_R56_VOVC);
	if(vt >= 0)
	{
		if(valmV > 22600)valmV = 22600;
		vt &= 0xF0;
		vt |= (valmV/1484);
		n68x386WriteReg(pInst, chnl, NX386_R56_VOVC, vt);
		return 1;
	}
	return -1;
}

NXFUNIMPSTR int N68X386SetVOH(__chip_inst pInst, __chip_chnl chnl, __u32 valmV)
{
	int vt;
	vt = n68x386ReadReg(pInst, chnl, NX386_R4C_OHVC);
	if(vt >= 0)
	{
		if(valmV > 93500)valmV = 93500;
		vt &= 0xC0;
		vt |= (valmV/1484);
		n68x386WriteReg(pInst, chnl, NX386_R4C_OHVC, vt);
		return 1;
	}
	return -1;
}

NXFUNIMPSTR int N68X386SetVBLV(__chip_inst pInst, __chip_chnl chnl, __u32 valmV)
{
	int vt;
	vt = n68x386ReadReg(pInst, chnl, NX386_R4F_VBLVC);
	if(vt >= 0)
	{
		if(valmV > 93500)valmV = 93500;
		vt &= 0xC0;
		vt |= (valmV/1484);
		n68x386WriteReg(pInst, chnl, NX386_R4F_VBLVC, vt);
		return 1;
	}
	return -1;
}

NXFUNIMPSTR int N68X386SetVoltageTrack(__chip_inst pInst, __chip_chnl chnl, int ena)
{
	int vt;
	vt = n68x386ReadReg(pInst, chnl, NX386_R56_VOVC);
	if(vt >= 0)
	{
		vt &= ~0x10;
		if(ena)
		{
			vt |= 0x10;
		}
		n68x386WriteReg(pInst, chnl, NX386_R56_VOVC, vt);
		return 1;
	}
	return -1;
}

NXFUNIMPSTR int N68X386SetLoopClosureDetect(__chip_inst pInst, __chip_chnl chnl, int CurOrVolt, __i32 valmAmV, int debouncemS)
{
	__i32 iTmp;

	if(debouncemS < 0) debouncemS = 0;
	if(debouncemS > 159) debouncemS = 159;
	iTmp = debouncemS;
	iTmp *= 1000;
	iTmp /= 1250;
	n68x386WriteReg(pInst, chnl, NX386_R47_LCDB, iTmp);

	if(CurOrVolt)
	{
		// current
		if(valmAmV < 0) valmAmV = 0;
		if(valmAmV > 80) valmAmV = 80;
		iTmp = valmAmV;
		iTmp *= 1000;
		iTmp /= 1270;
		n68x386WriteReg(pInst, chnl, NX386_R53_LCT, iTmp);

		iTmp = n68x386ReadReg(pInst, chnl, NX386_R46_RTDSLC);
		if(iTmp >= 0)
		{
			iTmp &= ~0x020;
			n68x386WriteReg(pInst, chnl, NX386_R46_RTDSLC, iTmp);
		}
	}
	else
	{
		// voltage
		if(valmAmV < 0) valmAmV = 0;
		if(valmAmV > 93500) valmAmV = 93500;
		valmAmV /= 1484;
		n68x386WriteReg(pInst, chnl, NX386_R53_LCT, valmAmV);

		iTmp = n68x386ReadReg(pInst, chnl, NX386_R46_RTDSLC);
		if(iTmp >= 0)
		{
			iTmp |= 0x020;
			n68x386WriteReg(pInst, chnl, NX386_R46_RTDSLC, iTmp);
		}
	}

	return 0;
}

NXFUNIMPSTR int N68X386SetMaxLoopCurrent(__chip_inst pInst, __chip_chnl chnl, int valmA)
{
	int v = n68x386ReadReg(pInst, chnl, NX386_R45_LCLC);
	if(v >= 0)
	{
		if(valmA < 20) valmA = 20;
		if(valmA > 41) valmA = 41;
		valmA -= 20;
		valmA /= 3;
		v &= ~0x07;
		v |= valmA;
		return n68x386WriteReg(pInst, chnl, NX386_R45_LCLC, v);
	}
	return v;
}

NXFUNIMPSTR int N68X386GetTemperature(__chip_inst pInst, __chip_chnl chnl)
{
	int v = n68x386ReadReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R99_TEMP);
	if(v >= 0)
	{
		return (v - 67);
	}
	return -128;
}

static const unsigned short bclk_values_x386[] = 
{
	256, 512, 768, 1000, 1024, 1152, 1536, 1544, 2000, 2048, 4000, 4096, 8000, 8192,
};
NXFUNIMPSTR int N68X386GetBCLK(__chip_inst pInst, __chip_chnl chnl)
{
	int v = n68x386ReadReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R04_PLLS);
	if(v >= 0)
	{
		v &= 0x01F;
		if(v & 0x01)
		{
			v >>= 1;
			if(v < sizeof(bclk_values_x386)/sizeof(bclk_values_x386[0]))
			{
				return bclk_values_x386[v];
			}
		}
		return 0; // PLL not locked
	}
	return v;
}

NXFUNIMPSTR __i32 N68X386GetBATVoltage(__chip_inst pInst, __chip_chnl chnl)
{
	__i32 v = n68x386ReadReg(pInst, chnl, NX386_R80_BATVC);
	if(v >= 0)
	{
		return v*376;
	}
	return v;
}

NXFUNIMPSTR __i32 N68X386GetTipVoltage(__chip_inst pInst, __chip_chnl chnl)
{
	__i32 v = n68x386ReadReg(pInst, chnl, NX386_R81_TIPV);
	if(v >= 0)
	{
		return v*374;
	}
	return v;
}

NXFUNIMPSTR __i32 N68X386GetRingVoltage(__chip_inst pInst, __chip_chnl chnl)
{
	__i32 v = n68x386ReadReg(pInst, chnl, NX386_R82_RINGV);
	if(v >= 0)
	{
		return v*374;
	}
	return v;
}

NXFUNIMPSTR __i32 N68X386GetLoopVoltage(__chip_inst pInst, __chip_chnl chnl)
{
	__i32 v = n68x386ReadReg(pInst, chnl, NX386_R8D_LPVC);
	if(v >= 0)
	{
		return v*374;
	}
	return v;
}

NXFUNIMPSTR __i32 N68X386GetLoopCurrent(__chip_inst pInst, __chip_chnl chnl)
{
	__i32 v = n68x386ReadReg(pInst, chnl, NX386_R90_LPIC);
	if(v >= 0)
	{
		return v*303;
	}
	return v;
}

NXFUNIMPSTR int N68X386SetPCM(__chip_inst pInst, __chip_chnl chnl, int opts, int *regbuf)
{
	int v = n68x386ReadReg(pInst, chnl, NX386_R00_PCMC);
	if(regbuf)
	{
		v = (*regbuf);
	}
	if(v < 0)
	{
		return v;
	}
	if(opts & PROX_PCM_DATA2BCLK)
	{
		v |= 0x10;
	}
	if(opts & PROX_PCM_DATA1BCLK)
	{
		v &= ~0x10;
	}
	if(opts & PROX_PCM_ALAW)
	{
		v &= 0x01F;
	}
	if(opts & PROX_PCM_ULAW)
	{
		v &= 0x01F;
		v |= 0x40;
	}
	if(opts & PROX_PCM_LINEAR)
	{
		v &= 0x01F;
		v |= 0xA0;
	}
	if(opts & PROX_PCM_DISABLE)
	{
		v &= ~0x01;
	}
	if(opts & PROX_PCM_ENABLE)
	{
		v |= 0x01;
	}
	if(regbuf)
	{
		(*regbuf) = v;
	}
	return n68x386WriteReg(pInst, chnl, NX386_R00_PCMC, v);
}

NXFUNIMPSTR int N68X386SetTxTimeSlot(__chip_inst pInst, __chip_chnl chnl, unsigned short valBits)
{
	int v = n68x386ReadReg(pInst, chnl, NX386_R03_TCHC);
	if(v < 0)
	{
		return v;
	}
	if(valBits > 1024) valBits = 1024;

	n68x386WriteReg(pInst, chnl, NX386_R01_TTLC, (valBits & 0x0FF));
	n68x386WriteReg(pInst, chnl, NX386_R08_TTLWB, (valBits & 0x0FF));

	valBits >>= 8;
	v &= ~0x33;
	v |= valBits;
	valBits <<= 4;
	v |= valBits;
	n68x386WriteReg(pInst, chnl, NX386_R03_TCHC, (v & 0x0FF));

	return 0;
}

NXFUNIMPSTR int N68X386SetRxTimeSlot(__chip_inst pInst, __chip_chnl chnl, unsigned short valBits)
{
	int v = n68x386ReadReg(pInst, chnl, NX386_R03_TCHC);
	if(v < 0)
	{
		return v;
	}
	if(valBits > 1024) valBits = 1024;

	n68x386WriteReg(pInst, chnl, NX386_R02_RTLC, (valBits & 0x0FF));
	n68x386WriteReg(pInst, chnl, NX386_R09_RTLWB, (valBits & 0x0FF));

	valBits >>= 6;
	valBits &= 0x0C;
	v &= ~0xCC;
	v |= valBits;
	valBits <<= 4;
	v |= valBits;

	n68x386WriteReg(pInst, chnl, NX386_R03_TCHC, (v & 0x0FF));

	return 0;
}

NXFUNIMPSTR int N68X386SetRxAPG(__chip_inst pInst, __chip_chnl chnl, enum tagAPGValue val)
{
	int v = n68x386ReadReg(pInst, chnl, NX386_R40_ACGC);
	if(v < 0)
	{
		return v;
	}
	
	val &= 0x03;

	return n68x386WriteReg(pInst, chnl, NX386_R40_ACGC, 0x0FF & ((v & ~0x0C) | (val << 2)));
}

NXFUNIMPSTR int N68X386SetTxAPG(__chip_inst pInst, __chip_chnl chnl, enum tagAPGValue val)
{
	int v = n68x386ReadReg(pInst, chnl, NX386_R40_ACGC);
	if(v < 0)
	{
		return v;
	}

	val &= 0x03;

	return n68x386WriteReg(pInst, chnl, NX386_R40_ACGC, 0x0FF & ((v & ~0x03) | (val << 0)));
}

NXFUNIMPSTR int N68X386GetInterrupts(__chip_inst pInst, __chip_chnl chnl)
{
	int vInts = 0;
	int vt;
	int vi;

	vt = n68x386ReadReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R24_INTVL);
	if(vt < 0) return -1;

	switch(nxGetChnlIndex(pInst, chnl))
	{
	case 0:
		vt &= 0x07;
		break;
	default:
		vt &= 0x70;
		vt >>= 4;
	}

	if(vt > 0)
	{
		if(vt & 0x01)
		{
			vi = n68x386ReadReg(pInst, chnl, NX386_R26_INT1);
			if(vi > 0)
			{
				n68x386WriteReg(pInst, chnl, NX386_R26_INT1, vi);
				if(vi & 0x0FC)
				{
					vInts |= PROX_INT_PA;
				}
				if(vi & 0x001)
				{
					vInts |= PROX_INT_RT;
				}
				if(vi & 0x002)
				{
					vInts |= PROX_INT_LC;
				}
			}
		}
		if(vt & 0x02)
		{
			vi = n68x386ReadReg(pInst, chnl, NX386_R28_INT2);
			if(vi > 0)
			{
				n68x386WriteReg(pInst, chnl, NX386_R28_INT2, vi);
				if(vi & 0x001)
				{
					vInts |= PROX_INT_T1A;
				}
				if(vi & 0x002)
				{
					vInts |= PROX_INT_T1I;
				}
				if(vi & 0x004)
				{
					vInts |= PROX_INT_T2A;
				}
				if(vi & 0x008)
				{
					vInts |= PROX_INT_T2I;
				}
				if(vi & 0x010)
				{
					vInts |= PROX_INT_RA;
				}
				if(vi & 0x020)
				{
					vInts |= PROX_INT_RI;
				}
				if(vi & 0x040)
				{
					vInts |= PROX_INT_DTMF;
				}
				if(vi & 0x080)
				{
					vInts |= PROX_INT_FSK;
				}
			}
		}
		if(vt & 0x04)
		{
			vi = n68x386ReadReg(pInst, chnl, NX386_R2A_INT3);
			if(vi > 0)
			{
				n68x386WriteReg(pInst, chnl, NX386_R2A_INT3, vi);
				if(vi & 0x001)
				{
					vInts |= PROX_INT_TMP;
				}
				if(vi & 0x008)
				{
					vInts |= PROX_INT_GKD;
				}
			}
		}
		return vInts;
	}

	return vt;
}

NXFUNIMPSTR int N68X386IntEnable(__chip_inst pInst, __chip_chnl chnl, int ints)
{
	int iRet = 0;
	int vt;

	if(ints & (PROX_INT_PA|PROX_INT_RT|PROX_INT_LC))
	{
		vt = n68x386ReadReg(pInst, chnl, NX386_R27_IE1);
		if(vt >= 0)
		{
			if(ints & PROX_INT_PA)
			{
				vt |= 0x0FC;
			}
			if(ints & PROX_INT_RT)
			{
				vt |= 0x001;
			}
			if(ints & PROX_INT_LC)
			{
				vt |= 0x002;
			}
			n68x386WriteReg(pInst, chnl, NX386_R27_IE1, vt);
		}
		else
		{
			iRet = -1;
		}
	}

	if(ints & (PROX_INT_T1A|PROX_INT_T1I|PROX_INT_T2A|PROX_INT_T2I|
				PROX_INT_RA|PROX_INT_RI|PROX_INT_DTMF|PROX_INT_FSK))
	{
		vt = n68x386ReadReg(pInst, chnl, NX386_R29_IE2);
		if(vt >= 0)
		{
			if(ints & PROX_INT_T1A)
			{
				vt |= 0x001;
			}
			if(ints & PROX_INT_T1I)
			{
				vt |= 0x002;
			}
			if(ints & PROX_INT_T2A)
			{
				vt |= 0x004;
			}
			if(ints & PROX_INT_T2I)
			{
				vt |= 0x008;
			}
			if(ints & PROX_INT_RA)
			{
				vt |= 0x010;
			}
			if(ints & PROX_INT_RI)
			{
				vt |= 0x020;
			}
			if(ints & PROX_INT_DTMF)
			{
				vt |= 0x040;
			}
			if(ints & PROX_INT_FSK)
			{
				vt |= 0x080;
			}
			n68x386WriteReg(pInst, chnl, NX386_R29_IE2, vt);
		}
		else
		{
			iRet = -1;
		}
	}

	if(ints & (PROX_INT_TMP|PROX_INT_GKD))
	{
		vt = n68x386ReadReg(pInst, chnl, NX386_R2B_IE3);
		if(vt >= 0)
		{
			if(ints & PROX_INT_TMP)
			{
				vt |= 0x001;
			}
			if(ints & PROX_INT_GKD)
			{
				vt |= 0x008;
			}
			n68x386WriteReg(pInst, chnl, NX386_R2B_IE3, vt);
		}
		else
		{
			iRet = -1;
		}
	}

	return iRet;
}

NXFUNIMPSTR int N68X386IntDisable(__chip_inst pInst, __chip_chnl chnl, int ints)
{
	int iRet = 0;
	int vt;

	if(ints & (PROX_INT_PA|PROX_INT_RT|PROX_INT_LC))
	{
		vt = n68x386ReadReg(pInst, chnl, NX386_R27_IE1);
		if(vt >= 0)
		{
			if(ints & PROX_INT_PA)
			{
				vt &= ~0x0FC;
			}
			if(ints & PROX_INT_RT)
			{
				vt &= ~0x001;
			}
			if(ints & PROX_INT_LC)
			{
				vt &= ~0x002;
			}
			n68x386WriteReg(pInst, chnl, NX386_R27_IE1, vt);
		}
		else
		{
			iRet = -1;
		}
	}

	if(ints & (PROX_INT_T1A|PROX_INT_T1I|PROX_INT_T2A|PROX_INT_T2I|
				PROX_INT_RA|PROX_INT_RI|PROX_INT_DTMF|PROX_INT_FSK))
	{
		vt = n68x386ReadReg(pInst, chnl, NX386_R29_IE2);
		if(vt >= 0)
		{
			if(ints & PROX_INT_T1A)
			{
				vt &= ~0x001;
			}
			if(ints & PROX_INT_T1I)
			{
				vt &= ~0x002;
			}
			if(ints & PROX_INT_T2A)
			{
				vt &= ~0x004;
			}
			if(ints & PROX_INT_T2I)
			{
				vt &= ~0x008;
			}
			if(ints & PROX_INT_RA)
			{
				vt &= ~0x010;
			}
			if(ints & PROX_INT_RI)
			{
				vt &= ~0x020;
			}
			if(ints & PROX_INT_DTMF)
			{
				vt &= ~0x040;
			}
			if(ints & PROX_INT_FSK)
			{
				vt &= ~0x080;
			}
			n68x386WriteReg(pInst, chnl, NX386_R29_IE2, vt);
		}
		else
		{
			iRet = -1;
		}
	}

	if(ints & (PROX_INT_TMP|PROX_INT_GKD))
	{
		vt = n68x386ReadReg(pInst, chnl, NX386_R2B_IE3);
		if(vt >= 0)
		{
			if(ints & PROX_INT_TMP)
			{
				vt &= ~0x001;
			}
			if(ints & PROX_INT_GKD)
			{
				vt &= ~0x008;
			}
			n68x386WriteReg(pInst, chnl, NX386_R2B_IE3, vt);
		}
		else
		{
			iRet = -1;
		}
	}

	return iRet;
}

NXFUNIMPSTR int N68X386GenDialTone(__chip_inst pInst, __chip_chnl chnl, enum tagDialtoneRegion xregion)
{
	int idx = xregion;
	idx -= enumFirstValue;

	nx386SetupOSC(pInst, chnl,
		DialtoneValues_x386[idx].OSC1_FREQ, DialtoneValues_x386[idx].OSC1_AMP,
		DialtoneValues_x386[idx].OSC1_OnTime, DialtoneValues_x386[idx].OSC1_OffTime,
		DialtoneValues_x386[idx].OSC2_FREQ, DialtoneValues_x386[idx].OSC2_AMP,
		DialtoneValues_x386[idx].OSC2_OnTime, DialtoneValues_x386[idx].OSC2_OffTime);
	nx386StartOSC(pInst, chnl, DialtoneValues_x386[idx].OSC1_OnTime, DialtoneValues_x386[idx].OSC2_OnTime);
	return 0;
}

NXFUNIMPSTR int N68X386GenRingbackTone(__chip_inst pInst, __chip_chnl chnl, enum tagRingbacktoneRegion xregion)
{
	int idx = xregion;
	idx -= enumFirstValue;

	nx386SetupOSC(pInst, chnl,
		RingbacktoneValues_x386[idx].OSC1_FREQ, RingbacktoneValues_x386[idx].OSC1_AMP,
		RingbacktoneValues_x386[idx].OSC1_OnTime, RingbacktoneValues_x386[idx].OSC1_OffTime,
		RingbacktoneValues_x386[idx].OSC2_FREQ, RingbacktoneValues_x386[idx].OSC2_AMP,
		RingbacktoneValues_x386[idx].OSC2_OnTime, RingbacktoneValues_x386[idx].OSC2_OffTime);
	nx386StartOSC(pInst, chnl, RingbacktoneValues_x386[idx].OSC1_OnTime, RingbacktoneValues_x386[idx].OSC2_OnTime);
	return 0;
}

NXFUNIMPSTR int N68X386GenBusyTone(__chip_inst pInst, __chip_chnl chnl, enum tagBusytoneRegion xregion)
{
	int idx = xregion;
	idx -= enumFirstValue;

	nx386SetupOSC(pInst, chnl,
		BusytoneValues_x386[idx].OSC1_FREQ, BusytoneValues_x386[idx].OSC1_AMP,
		BusytoneValues_x386[idx].OSC1_OnTime, BusytoneValues_x386[idx].OSC1_OffTime,
		BusytoneValues_x386[idx].OSC2_FREQ, BusytoneValues_x386[idx].OSC2_AMP,
		BusytoneValues_x386[idx].OSC2_OnTime, BusytoneValues_x386[idx].OSC2_OffTime);
	nx386StartOSC(pInst, chnl, BusytoneValues_x386[idx].OSC1_OnTime, BusytoneValues_x386[idx].OSC2_OnTime);
	return 0;
}

NXFUNIMPSTR int N68X386DTMFGen(__chip_inst pInst, __chip_chnl chnl, unsigned char xdigit, unsigned int msActive, unsigned int msInactive, int db)
{
	int vt = 0x20 | 0x40;
	// Jolly's PATCH
	unsigned short amp_row, amp_col;
	n68x386WriteReg(pInst, chnl, NX386_RC0_OSNC, vt);

	//if(msActive < 0) msActive = 0;
	if(msActive > 8000) msActive = 8000;
	//if(msInactive < 0) msInactive = 0;
	if(msInactive > 8000) msInactive = 8000;
	if(!msActive) return 0;

	if((xdigit >= '0') && (xdigit <= '9'))
	{
		xdigit -= '0';
	}
	else
	if((xdigit >= 'A') && (xdigit <= 'D'))
	{
		xdigit = 10 + xdigit - 'A';
	}
	else
	if(xdigit == '*')
	{
		xdigit = 14;
	}
	else
	if(xdigit == '#')
	{
		xdigit = 15;
	}
	else
	if((xdigit >= 'a') && (xdigit <= 'd'))
	{
		xdigit = 10 + xdigit - 'a';
	}
	else
	{
		// Error digit
		return -1;
	}

	msActive *= 8;
	msInactive *= 8;

	// Jolly's PATCH
	amp_row = awAmplitudeRow_x386[asDigit[xdigit].row];
	amp_col = awAmplitudeColumn_x386[asDigit[xdigit].col];
	if (db > 2)
		db = 2; /* amplitude would overflow */
	if (db < 0) {
		db = 0 - db;
		amp_row >>= db;
		amp_col >>= db;
	} else {
		amp_row <<= db;
		amp_col <<= db;
	}

	nx386SetupOSC(pInst, chnl,
		awFreqRow_x386[asDigit[xdigit].row], amp_row,
		msActive, msInactive,
		awFreqColumn_x386[asDigit[xdigit].col], amp_col,
		msActive, msInactive);

	n68x386WriteReg(pInst, chnl, NX386_RAD_OS2RPD, 0x00);

	vt = 0;
	n68x386WriteReg(pInst, chnl, NX386_RC1_RMPC, vt);
	// en
	vt = 0x08 | 0x04 | 0x01 | 0x02;
	return n68x386WriteReg(pInst, chnl, NX386_RC0_OSNC, vt);
}

NXFUNIMPSTR int N68X386ToneGen(__chip_inst pInst, __chip_chnl chnl, unsigned int osc1, unsigned int osic1,
					unsigned int osc2, unsigned int osic2, unsigned int msActive, unsigned int msInactive)
{
	int vt = 0x20 | 0x40;
	n68x386WriteReg(pInst, chnl, NX386_RC0_OSNC, vt);

	//if(msActive < 0) msActive = 0;
	if(msActive > 8000) msActive = 8000;
	//if(msInactive < 0) msInactive = 0;
	if(msInactive > 8000) msInactive = 8000;
	if(!msActive) return 0;

	msActive *= 8;
	msInactive *= 8;

	nx386SetupOSC(pInst, chnl,
		osc1, osic1, msActive, msInactive,
		osc2, osic2, msActive, msInactive);

	n68x386WriteReg(pInst, chnl, NX386_RAD_OS2RPD, 0x00);

	vt = 0;
	n68x386WriteReg(pInst, chnl, NX386_RC1_RMPC, vt);
	// en
	vt = 0x08 | 0x04 | 0x01 | 0x02;
	return n68x386WriteReg(pInst, chnl, NX386_RC0_OSNC, vt);
}

NXFUNIMPSTR int N68X386StopTones(__chip_inst pInst, __chip_chnl chnl)
{
	int vt = 0x08 | 0x04;
	return n68x386WriteReg(pInst, chnl, NX386_RC0_OSNC, vt);
}

NXFUNIMPSTR int N68X386FSKGenSetup(__chip_inst pInst, __chip_chnl chnl, int opts)
{
	int vt = n68x386ReadReg(pInst, chnl, NX386_R10_FSKC);
	if(vt < 0)
	{
		return vt;
	}
	if(opts & PROX_FSKG_ENABLE)
	{
		vt |= 0x01;
	}
	if(opts & PROX_FSKG_DISABLE)
	{
		vt &= ~0x01;
	}
	if(opts & PROX_FSKG_ITUTV23)
	{
		vt |= 0x02;
	}
	if(opts & PROX_FSKG_BELL202)
	{
		vt &= ~0x02;
	}
	if(opts & PROX_FSKG_1STOPBIT)
	{
		vt &= ~0x04;
	}
	if(opts & PROX_FSKG_2STOPBIT)
	{
		vt |= 0x04;
	}
	if(opts & PROX_FSKG_START)
	{
		vt |= 0x08;
	}
	if(opts & PROX_FSKG_STOP)
	{
		vt &= ~0x08;
	}
	if(opts & PROX_FSKG_PARITY_EVEN)
	{
		vt &= ~0x20;
		vt |= 0x40;
	}
	if(opts & PROX_FSKG_PARITY_ODD)
	{
		vt |= 0x20;
		vt |= 0x40;
	}
	if(opts & PROX_FSKG_PARITY_NONE)
	{
		vt &= ~0x40;
	}
	if(opts & PROX_FSKG_MODE_RAW)
	{
		vt &= ~0x80;
	}
	if(opts & PROX_FSKG_MODE_PACKED)
	{
		vt |= 0x80;
	}
	if(opts & PROX_FSKG_POL_NORMAL)
	{
		vt &= ~0x20;
	}
	if(opts & PROX_FSKG_POL_INVERT)
	{
		vt |= 0x20;
	}
	return n68x386WriteReg(pInst, chnl, NX386_R10_FSKC, vt);
}

#ifndef PROXFSKGENGAINVALUE
#define PROXFSKGENGAINVALUE 1
// -24 to 0
static const unsigned char fskGainLevels[] = 
{
	1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 4, 4, 5, 5, 6, 7, 7, 8, 9, 10, 12, 13, 15
};
#endif // PROXFSKGENGAINVALUE

NXFUNIMPSTR int N68X386FSKGenSetLevel(__chip_inst pInst, __chip_chnl chnl, int dBVal)
{
	if(dBVal < -24)dBVal = -24;
	if(dBVal > 0)dBVal = 0;
	return n68x386WriteReg(pInst, chnl, NX386_R13_FSKLCR, fskGainLevels[dBVal + 24]);
}

NXFUNIMPSTR int N68X386FSKGenWrite(__chip_inst pInst, __chip_chnl chnl, unsigned char *dat, int cnt)
{
	int w = 0;
	int vt;
	while(cnt)
	{
		vt = n68x386ReadReg(pInst, chnl, NX386_R12_FSKS);
		if(vt < 0) return vt;
		if(vt & 0x04) return w;
		n68x386WriteReg(pInst, chnl, NX386_R11_FSKTD, (*dat));
		cnt --;
		dat ++;
		w ++;
	}
	return w;
}

NXFUNIMPSTR int N68X386FSKGenIsBusy(__chip_inst pInst, __chip_chnl chnl)
{
	int vt = n68x386ReadReg(pInst, chnl, NX386_R12_FSKS);
	if(vt < 0) return vt;
	return (!(vt & 0x01));
}

NXFUNIMPSTR int N68X386DTMFDSetInput(__chip_inst pInst, __chip_chnl chnl, int adcOrdac)
{
	int vt = n68x386ReadReg(pInst, chnl, NX386_R30_DTMFC);
	if(vt < 0) return vt;
	if(adcOrdac)
	{
		vt &= ~0x40;
	}
	else
	{
		vt |= 0x40;
	}
	return n68x386WriteReg(pInst, chnl, NX386_R30_DTMFC, vt);
}

NXFUNIMPSTR int N68X386DTMFDSetFreqError(__chip_inst pInst, __chip_chnl chnl, int tolerancelevel)
{
	int vt = n68x386ReadReg(pInst, chnl, NX386_R30_DTMFC);
	if(vt < 0) return vt;
	vt &= ~0x30;
	vt |= ((tolerancelevel & 0x03) << 4);
	return n68x386WriteReg(pInst, chnl, NX386_R30_DTMFC, vt);
}

NXFUNIMPSTR int N68X386DTMFDSetDecSpeed(__chip_inst pInst, __chip_chnl chnl, int speedlevel)
{
	int vt = n68x386ReadReg(pInst, chnl, NX386_R30_DTMFC);
	if(vt < 0) return vt;
	vt &= 0xF0;
	vt |= ((speedlevel & 0x0F));
	return n68x386WriteReg(pInst, chnl, NX386_R30_DTMFC, vt);
}

NXFUNIMPSTR int N68X386DTMFDSetSigLevel(__chip_inst pInst, __chip_chnl chnl, int dBVal)
{
	__i32 v;

#if GAIN_STEP_0DOT5
	if(dBVal < -410) dBVal = -410;
	if(dBVal > 60) dBVal = 60;
	dBVal += 410;
	dBVal /= 5;
#else // GAIN_STEP_0DOT5
	if(dBVal < -41) dBVal = -41;
	if(dBVal > 6) dBVal = 6;
	dBVal += 41;
#endif // GAIN_STEP_0DOT5

	v = ADCDACGAIN[dBVal];
	v *= 32;

	n68x386WriteReg(pInst, chnl, NX386_R34_DTMFTH, ((v >> 8) & 0x0FF));
	n68x386WriteReg(pInst, chnl, NX386_R35_DTMFTL, (v & 0x0FF));
	return 1;
}

NXFUNIMPSTR int N68X386DTMFDSetTimes(__chip_inst pInst, __chip_chnl chnl, int msPreActive, int msActive, int msInactive)
{
	if(msPreActive < 0) msPreActive = 0;
	if(msPreActive > 127) msPreActive = 127;
	if(msActive < 0) msActive = 0;
	if(msActive > 127) msActive = 127;
	if(msInactive < 0) msInactive = 0;
	if(msInactive > 127) msInactive = 127;
	n68x386WriteReg(pInst, chnl, NX386_R36_DTMFPT, msPreActive*2);
	n68x386WriteReg(pInst, chnl, NX386_R37_DTMFAT, msInactive*2);
	n68x386WriteReg(pInst, chnl, NX386_R38_DTMFDT, msActive*2);
	return 1;
}

#ifndef PROXDTMFVTOC
#define PROXDTMFVTOC 1
static const char dtmf_char[] =
{
	'D', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '*', '#', 'A', 'B', 'C'
};
#endif // PROXDTMFVTOC
NXFUNIMPSTR int N68X386DTMFDGetData(__chip_inst pInst, __chip_chnl chnl)
{
	int vt = n68x386ReadReg(pInst, chnl, NX386_R3A_DTMFD);
	if(vt < 0) return vt;
	if(vt & 0x80)
	{
		return dtmf_char[vt & 0x0F];
	}
	return 0;
}

NXFUNIMPSTR int N68X386DTMFDEnable(__chip_inst pInst, __chip_chnl chnl, int ena)
{
	int vt = n68x386ReadReg(pInst, chnl, NX386_R30_DTMFC);
	if(vt < 0) return vt;
	if(ena)
	{
		vt |= 0x80;
	}
	else
	{
		vt &= ~0x80;
	}
	return n68x386WriteReg(pInst, chnl, NX386_R30_DTMFC, vt);
}

NXFUNIMPSTR int N68X386DiagDLoop0(__chip_inst pInst, __chip_chnl chnl, int ena)
{
	int vt = n68x386ReadReg(pInst, chnl, NX386_R21_LBC);
	if(vt < 0) return vt;
	if(ena)
	{
		vt |= 0x01;
	}
	else
	{
		vt &= ~0x01;
	}
	return n68x386WriteReg(pInst, chnl, NX386_R21_LBC, vt);
}

NXFUNIMPSTR int N68X386DiagDLoop1(__chip_inst pInst, __chip_chnl chnl, int ena)
{
	int vt = n68x386ReadReg(pInst, chnl, NX386_R21_LBC);
	if(vt < 0) return vt;
	if(ena)
	{
		vt |= 0x02;
	}
	else
	{
		vt &= ~0x02;
	}
	return n68x386WriteReg(pInst, chnl, NX386_R21_LBC, vt);
}

NXFUNIMPSTR int N68X386DiagDLoop2(__chip_inst pInst, __chip_chnl chnl, int ena)
{
	int vt = n68x386ReadReg(pInst, chnl, NX386_R21_LBC);
	if(vt < 0) return vt;
	if(ena)
	{
		vt |= 0x04;
	}
	else
	{
		vt &= ~0x04;
	}
	return n68x386WriteReg(pInst, chnl, NX386_R21_LBC, vt);
}

NXFUNIMPSTR int N68X386DiagALoop0(__chip_inst pInst, __chip_chnl chnl, int ena)
{
	int vt = n68x386ReadReg(pInst, chnl, NX386_R21_LBC);
	if(vt < 0) return vt;
	if(ena)
	{
		vt |= 0x08;
	}
	else
	{
		vt &= ~0x08;
	}
	return n68x386WriteReg(pInst, chnl, NX386_R21_LBC, vt);
}

NXFUNIMPSTR int N68X386DiagALoop1(__chip_inst pInst, __chip_chnl chnl, int ena)
{
	int vt = n68x386ReadReg(pInst, chnl, NX386_R21_LBC);
	if(vt < 0) return vt;
	if(ena)
	{
		vt |= 0x10;
	}
	else
	{
		vt &= ~0x10;
	}
	return n68x386WriteReg(pInst, chnl, NX386_R21_LBC, vt);
}

NXFUNIMPSTR int N68X386DiagXConfig(__chip_inst pInst, __chip_chnl chnl, __u16 opts)
{
	int v = n68x386ReadReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R15_DIAGXC0);
	if(v < 0)
	{
		return v;
	}
	switch(nxGetChnlIndex(pInst, chnl))
	{
	case 0:
		v &= ~0x02;
		break;
	case 1:
		v |= 0x02;
		break;
	}
	if(opts & DIAGX_ENABLE)
	{
		v |= 0x01;
	}
	if(opts & DIAGX_DISABLE)
	{
		v &= ~0x01;
	}
	if(opts & DIAGX_FIFO_ENA)
	{
		v |= 0x08;
	}
	if(opts & DIAGX_FIFO_DIS)
	{
		v &= ~0x08;
	}
	if(opts & DIAGX_FIFOSRC_ADC)
	{
		v |= 0x80;
	}
	if(opts & DIAGX_FIFOSRC_SEL)
	{
		v &= ~0x80;
	}
	if(opts & DIAGX_ACLPF_ENA)
	{
		v |= 0x20;
	}
	if(opts & DIAGX_ACLPF_DIS)
	{
		v &= ~0x20;
	}
	if(opts & DIAGX_ACHPF_ENA)
	{
		v |= 0x40;
	}
	if(opts & DIAGX_ACHPF_DIS)
	{
		v &= ~0x40;
	}
	if(opts & DIAGX_DCLPF_ENA)
	{
		v |= 0x10;
	}
	if(opts & DIAGX_DCLPF_DIS)
	{
		v &= ~0x10;
	}
	if(opts & DIAGX_DATA_SIGNED)
	{
		v |= 0x04;
	}
	if(opts & DIAGX_DATA_USIGNED)
	{
		v &= ~0x04;
	}
	n68x386WriteReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R15_DIAGXC0, v);
	v = n68x386ReadReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R1A_DIAGXC5);
	if(v < 0)
	{
		return v;
	}
	v &= 0xBF;
	return n68x386WriteReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R1A_DIAGXC5, v);
}

NXFUNIMPSTR int N68X386DiagXSetACSrc(__chip_inst pInst, __chip_chnl chnl, enum tagDIAGXSrc src)
{
	int v = n68x386ReadReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R16_DIAGXC1);
	if(v < 0)
	{
		return v;
	}
	v &= 0x08F;
	v |= ((unsigned char)src) << 4;
	return n68x386WriteReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R16_DIAGXC1, v);
}

NXFUNIMPSTR int N68X386DiagXSetDCSrc(__chip_inst pInst, __chip_chnl chnl, enum tagDIAGXSrc src)
{
	int v = n68x386ReadReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R16_DIAGXC1);
	if(v < 0)
	{
		return v;
	}
	v &= 0x0F8;
	v |= ((unsigned char)src);
	return n68x386WriteReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R16_DIAGXC1, v);
}

NXFUNIMPSTR int N68X386DiagXTMSetSrc(__chip_inst pInst, __chip_chnl chnl, enum tagDIAGXSrc src)
{
	int v = n68x386ReadReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R18_DIAGXC3);
	if(v < 0)
	{
		return v;
	}
	v &= 0x08F;
	v |= ((unsigned char)src) << 4;
	return n68x386WriteReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R18_DIAGXC3, v);
}

NXFUNIMPSTR int N68X386DiagXTMSetHighThreshold(__chip_inst pInst, __chip_chnl chnl, __u16 val)
{
	int v = n68x386ReadReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R18_DIAGXC3);
	if(v < 0)
	{
		return v;
	}
	val &= 0x0FFF;
	n68x386WriteReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R17_DIAGXC2, (val & 0x0FF));
	v &= 0xF0;
	v |= (val >> 8);
	return n68x386WriteReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R18_DIAGXC3, v);
}

NXFUNIMPSTR int N68X386DiagXTMSetLowThreshold(__chip_inst pInst, __chip_chnl chnl, __u16 val)
{
	int v = n68x386ReadReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R1A_DIAGXC5);
	if(v < 0)
	{
		return v;
	}
	val &= 0x0FFF;
	n68x386WriteReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R19_DIAGXC4, (val & 0x0FF));
	v &= 0xF0;
	v |= (val >> 8);
	return n68x386WriteReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R1A_DIAGXC5, v);
}

NXFUNIMPSTR int N68X386DiagXTMEnable(__chip_inst pInst, __chip_chnl chnl, int ena)
{
	int vt = n68x386ReadReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R1C_DIAGXC7);
	if(vt < 0) return vt;
	if(ena)
	{
		vt |= 0x80;
	}
	else
	{
		vt &= ~0x80;
	}
	return n68x386WriteReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R1C_DIAGXC7, vt);
}

NXFUNIMPSTR int N68X386DiagXTMGetValue(__chip_inst pInst, __chip_chnl chnl)
{
	int vt = n68x386ReadReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R1B_DIAGXC6);
	int vt2 = n68x386ReadReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R1C_DIAGXC7);
	if((vt < 0) || (vt2 < 0)) return -1;
	return (((vt2 & 0x0F) << 8) | vt);
}

NXFUNIMPSTR int N68X386DiagXIsFifo0Done(__chip_inst pInst, __chip_chnl chnl)
{
	int vt = n68x386ReadReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R26_INT1);
	if(vt < 0) return 0;
	if(vt & 0x01)
	{
		n68x386WriteReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R26_INT1, 0x01);
		return 1;
	}
	return 0;
}

NXFUNIMPSTR int N68X386DiagXIsFifo1Done(__chip_inst pInst, __chip_chnl chnl)
{
	int vt = n68x386ReadReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R26_INT1);
	if(vt < 0) return 0;
	if(vt & 0x02)
	{
		n68x386WriteReg(pInst, nxGetCommonChnl(pInst, chnl), NX386_R26_INT1, 0x02);
		return 1;
	}
	return 0;
}

#ifndef FORLIMITDC
#define NXABS(arg) ((arg >= 0)?arg:-arg)
#define SAMPLES 50
#define HAZLIMITAC  50000
#define HAZLIMITDC  135000
#define FORLIMITAC  10000
#define FORLIMITDC  6000
#endif
NXFUNIMPSTR int N68X386OpenVoltageTest(__chip_inst pInst, __chip_chnl chnl, __i32 *acmVloop, __i32 *dcmVloop, __i32 *acmVring, __i32 *dcmVring, __i32 *acmVtip, __i32 *dcmVtip, int aForeignVolt)
{
	__i32 VacLoop, VdcLoop, VacRing, VdcRing, VacTip, VdcTip;		// in milivolts 
	__i32 TipVoltage, RingVoltage, LoopVoltage, num;
	__i32 TipVoltageSum, RingVoltageSum, LoopVoltageSum;  
	__i32 sampledTipVoltage[SAMPLES], sampledRingVoltage[SAMPLES], sampledLoopVoltage[SAMPLES]; 

	int v;

	TipVoltageSum = RingVoltageSum = LoopVoltageSum = 0;

	//The open state is used to prevent line driver from affecting tip/ring.
	if(N68X386SetLineStatusTo(pInst, chnl, LS_OPEN) < 0) return -1;
	
	if(aForeignVolt)
	{
		// DC off
		if(N68X386BATOn(pInst, chnl, 0) < 0)
		{
			v = -1;
			goto eexit;
		}
	}
	else
	{
		// DC on
		if(N68X386BATOn(pInst, chnl, 1) < 0)
		{
			v = -1;
			goto eexit;
		}
	}

	// waiting 100 ms for capacitance settling.
	DELAY_MS(100); 

	// aquires and sums samples of line voltage.
	// now Vtip will almost the same with Vring because of the Vscm sence circuit
	for(num=0; num<SAMPLES; num++)
	{
		v = N68X386GetTipVoltage(pInst, chnl);
		if(v >= 0)
		{
			TipVoltage = v;
		}
		else
		{
			goto eexit;
		}
		v = N68X386GetRingVoltage(pInst, chnl);
		if(v >= 0)
		{
			RingVoltage = v;
		}
		else
		{
			goto eexit;
		}
		v = N68X386GetLoopVoltage(pInst, chnl);
		if(v >= 0)
		{
			LoopVoltage = v;
		}
		else
		{
			goto eexit;
		}

		sampledTipVoltage[num] = TipVoltage;
		TipVoltageSum += TipVoltage;

		sampledRingVoltage[num] = RingVoltage;
		RingVoltageSum += RingVoltage;

		sampledLoopVoltage[num] = LoopVoltage;
		LoopVoltageSum += LoopVoltage;
		
		DELAY_MS(1); // If your SPI speed is fast enough, please add delay here
	}

	// The samples are then scaled and averaged for the dc value.  If different number of samples are taken, it is possible
	// to not be get different values due to AC frequencies on the line.  
	VdcTip =  TipVoltageSum/SAMPLES;
	VdcRing = RingVoltageSum/SAMPLES;
	VdcLoop = LoopVoltageSum/SAMPLES;

	if(dcmVtip)
	{
		(*dcmVtip) = VdcTip;
	}
	if(dcmVring)
	{
		(*dcmVring) = VdcRing;
	}
	if(dcmVloop)
	{
		(*dcmVloop) = VdcLoop;
	}

	TipVoltageSum = 0;
	RingVoltageSum = 0;
	LoopVoltageSum = 0;

	//The dc offset is then subtracted from the samples to remove the dc component.  Then by squareing the numbers, you can 
	//determine the AC rms value.
	for(num=0; num<SAMPLES; num++)
	{
		sampledTipVoltage[num] = sampledTipVoltage[num] - (VdcTip);
		sampledTipVoltage[num] = (sampledTipVoltage[num]*sampledTipVoltage[num])/SAMPLES;
		TipVoltageSum += sampledTipVoltage[num];

		sampledRingVoltage[num] = sampledRingVoltage[num] - (VdcRing);
		sampledRingVoltage[num] = (sampledRingVoltage[num]*sampledRingVoltage[num])/SAMPLES;
		RingVoltageSum += sampledRingVoltage[num];

		sampledLoopVoltage[num] = sampledLoopVoltage[num] - (VdcLoop);
		sampledLoopVoltage[num] = (sampledLoopVoltage[num]*sampledLoopVoltage[num])/SAMPLES;
		LoopVoltageSum += sampledLoopVoltage[num];
	}

	//calculate and output AC rms values
	VacTip = nxSqrt(TipVoltageSum);
	VacRing = nxSqrt(RingVoltageSum);
	VacLoop = nxSqrt(LoopVoltageSum);

	if(acmVtip)
	{
		(*acmVtip) = VacTip;
	}
	if(acmVring)
	{
		(*acmVring) = VacRing;
	}
	if(acmVloop)
	{
		(*acmVloop) = VacLoop;
	}

	v = 0;

	if(aForeignVolt)
	{
		if(((VacTip>FORLIMITAC) || (VacRing>FORLIMITAC) || (VacLoop>FORLIMITAC) 
			|| (VdcTip>FORLIMITDC) || (VdcRing>FORLIMITDC)) || ((NXABS((int)VdcLoop)>FORLIMITDC)))
		{
			v = 1;
		}
	}
	else
	{
		if((VacTip>HAZLIMITAC || VacRing>HAZLIMITAC || VacLoop>HAZLIMITAC)
			|| (VdcTip>HAZLIMITDC) || (VdcRing>HAZLIMITDC) || ((NXABS((int)VdcLoop)>HAZLIMITDC)))
		{
			v = 1;
		}
	}

eexit:

	N68X386BATOn(pInst, chnl, 1);
	N68X386SetLineStatusTo(pInst, chnl, LS_FORWARD_IDLE);

	return v;
}

#define LOWREN_CONST1_X386 30
#define LOWREN_CONST2_X386 600
NXFUNIMPSTR int N68X386LowRENTest(__chip_inst pInst, __chip_chnl chnl, __i32 *bellowDot175, __i32 *calibConst)
{
	int i;
	int cnt;
	int iRet = -1;
	__i32 v;
	int olamc;
	int oohv;
	int oogm;
	int ovov;

	if(bellowDot175)(*bellowDot175) = 0;
	if(calibConst)(*calibConst) = 0;

	olamc = n68x386ReadReg(pInst, chnl, NX386_R43_LAMC);
	oohv = n68x386ReadReg(pInst, chnl, NX386_R4C_OHVC);
	oogm = n68x386ReadReg(pInst, chnl, NX386_R4D_GMVC);
	ovov =  n68x386ReadReg(pInst, chnl, NX386_R56_VOVC);
	if(olamc < 0 || oohv < 0 || oogm < 0 || ovov < 0)return -1;

	n68x386WriteReg(pInst, chnl, NX386_R43_LAMC, 0x06);
	n68x386WriteReg(pInst, chnl, NX386_R4C_OHVC, 0x32);
	n68x386WriteReg(pInst, chnl, NX386_R4D_GMVC, 0x00);
	n68x386WriteReg(pInst, chnl, NX386_R56_VOVC, 0x00);

	cnt = 0;
	for(i=0; i<10; i++)
	{
		if(N68X386SetLineStatusTo(pInst, chnl, LS_FORWARD_IDLE) < 0)
		{
			goto xerr;
		}
		do
		{
			v = N68X386GetLoopVoltage(pInst, chnl);
		}while(v <= 65000); // wait for 65V
		DELAY_MS(10);
		if(N68X386SetLineStatusTo(pInst, chnl, LS_OPEN) < 0)
		{
			goto xerr;
		}
		do
		{
			v = N68X386GetLoopVoltage(pInst, chnl);
		}while(v > 65000); // wait for 65V
		// here you have to lock the machine to make sure the bellow loop has const time
		do
		{
			v = N68X386GetLoopVoltage(pInst, chnl);
			cnt ++;
		}while(v > 60000); // wait for 60V
	}

	if(calibConst)(*calibConst) = cnt;

	if(cnt < LOWREN_CONST1_X386)
	{
		iRet = 1; // < 0.175REN
		if(bellowDot175)(*bellowDot175) = 1;
	}
	else if(cnt < LOWREN_CONST2_X386)
	{
		iRet = 0; // 0.175 ~ 3REN
	}
	else
	{
		iRet = 2; // > 3REN
	}
xerr:
	n68x386WriteReg(pInst, chnl, NX386_R43_LAMC, olamc);
	n68x386WriteReg(pInst, chnl, NX386_R4C_OHVC, oohv);
	n68x386WriteReg(pInst, chnl, NX386_R4D_GMVC, oogm);
	n68x386WriteReg(pInst, chnl, NX386_R56_VOVC, ovov);
	return iRet;
}

#define HIGHREN_CONST_X386 970
NXFUNIMPSTR int N68X386RENTest(__chip_inst pInst, __chip_chnl chnl, __i32 *above5REN, __i32 *bellowDot175, __i32 *calibConstHigh, __i32 *calibConstLow)
{
	int iRet;
	__i16 datai[128]; // 2^7, don't change
	__i16 datav[128];
	__i32 sum;
	__i32 vrms, irms;
	int i;

	if(calibConstHigh)(*calibConstHigh) = 0;
	if(above5REN)(*above5REN) = 0;

	iRet = N68X386LowRENTest(pInst, chnl, bellowDot175, calibConstLow);
	if(iRet < 0) return iRet;
	if(iRet != 2 && !calibConstHigh)
	{
		return iRet;
	}

	// need get calibConstHigh or do High REN test
	// use 40Hz, 24Vpeak Ring to do the test
	N68X386SetupRingtone(pInst, chnl, RING_LINE_T);
	N68X386Ring(pInst, chnl, 1, 0, 0);
	DELAY_MS(50);
	for(i=0; i<128; i++)
	{
		datav[i] = n68x386ReadReg(pInst, chnl, NX386_R8D_LPVC)<<4;
		datai[i] = n68x386ReadReg(pInst, chnl, NX386_R90_LPIC)<<4;
		DELAY_MS(1);
	}
	N68X386Ring(pInst, chnl, 0, 0, 0);

	sum = 0;
	for(i=0; i<128; i++)
	{
		sum += datav[i];
	}
	sum /= 128;
	if(sum)
	{
		for(i=0; i<128; i++)
		{
			datav[i] -= sum;
		}
	}

	sum = 0;
	for(i=0; i<128; i++)
	{
		sum += (datav[i]*datav[i]);
	}
	sum >>= 7; // (7 + 8)
	vrms = nxSqrt(sum);

	sum = 0;
	for(i=0; i<128; i++)
	{
		sum += datai[i];
	}
	sum /= 128;
	if(sum)
	{
		for(i=0; i<128; i++)
		{
			datai[i] -= sum;
		}
	}

	sum = 0;
	for(i=0; i<128; i++)
	{
		sum += (datai[i]*datai[i]);
	}
	sum >>= 7; // (7 + 8)
	irms = nxSqrt(sum);

	sum = (vrms << 10)/(irms);

	if(calibConstHigh) (*calibConstHigh) = sum;

	if(sum < HIGHREN_CONST_X386)
	{
		iRet = 1;
	}
	else
	{
		if(iRet == 2)iRet = 0;
	}

	return iRet;
}

#define RES_CONST1_X386 421
#define RES_CONST2_X386 215
#define RES_VOLT_X386 0x20
NXFUNIMPSTR int N68X386ResFaultTest(__chip_inst pInst, __chip_chnl chnl, __i32 *RTR, __i32 *RTG, __i32 *RRG)
{
	int iTmp;
	int v1, v2;
	__i32 vt, vr;
	__i32 x;
	__i32 rtr1, rtr2;
	__i32 vtot, vtor;
	__i32 vrot, vror;
	__i32 ft;
	//__u32 ut1, ut2;
	__i32 rtr, rtg, rrg;
	int iRet = -1;

	int olamc;
	int oohv;
	int ovov;
	int oogm;

	olamc = n68x386ReadReg(pInst, chnl, NX386_R43_LAMC);
	oohv = n68x386ReadReg(pInst, chnl, NX386_R4C_OHVC);
	ovov = n68x386ReadReg(pInst, chnl, NX386_R56_VOVC);
	oogm  = n68x386ReadReg(pInst, chnl, NX386_R4D_GMVC);
	if(olamc < 0 || oohv < 0 || ovov < 0 || oogm < 0)return -1;

	n68x386WriteReg(pInst, chnl, NX386_R43_LAMC, 0x06);
	n68x386WriteReg(pInst, chnl, NX386_R56_VOVC, 0x03);
	n68x386WriteReg(pInst, chnl, NX386_R4D_GMVC, 0x03);

	if(N68X386SetLineStatusTo(pInst, chnl, LS_FORWARD_IDLE) < 0)
	{
		goto xerr;
	}

	n68x386WriteReg(pInst, chnl, NX386_R4C_OHVC, 0x00);
	DELAY_MS(100);
	if(N68X386SetLineStatusTo(pInst, chnl, LS_OPEN) < 0)
	{
		goto xerr;
	}
	DELAY_MS(200);
	if(N68X386SetLineStatusTo(pInst, chnl, LS_TIPOPEN) < 0)
	{
		goto xerr;
	}

	n68x386WriteReg(pInst, chnl, NX386_R4C_OHVC, RES_VOLT_X386);
	DELAY_MS(400);
	vt = 0;
	vr = 0;
	for(iTmp = 0; iTmp < 20; iTmp ++)
	{
		v1 = n68x386ReadReg(pInst, chnl, NX386_R81_TIPV);
		v2 = n68x386ReadReg(pInst, chnl, NX386_R82_RINGV);
		//v1 = n68x386ReadShort(pInst, chnl, NX386_R81_TIPV);
		//v2 = n68x386ReadShort(pInst, chnl, NX386_R82_RINGV);
		if(v1 != -1 && v2 != -1)
		{
			vt += v1;
			vr += v2;
		}
		else
		{
			goto xerr;
		}
	}
	// vt /= 20;
	// vr /= 20;

	vtot = vt;
	vtor = vr;
	if(RES_CONST2_X386*vt + RES_CONST1_X386*vt != RES_CONST1_X386*vr)
	{
		x = (RES_CONST2_X386*RES_CONST1_X386*vr - RES_CONST1_X386*vt*RES_CONST2_X386)/(RES_CONST2_X386*vt - RES_CONST1_X386*vr + RES_CONST1_X386*vt);
	}
	else
	{
		x = -1;
	}

	rtr1 = x;
	if(N68X386SetLineStatusTo(pInst, chnl, LS_OPEN) < 0)
	{
		goto xerr;
	}
	DELAY_MS(200);
	if(N68X386SetLineStatusTo(pInst, chnl, LS_RINGOPEN) < 0)
	{
		goto xerr;
	}
	//n68x386WriteReg(pInst, chnl, NX386_R4C_OHVC, RES_VOLT_X386);
	DELAY_MS(400);
	vt = 0;
	vr = 0;
	for(iTmp = 0; iTmp < 20; iTmp ++)
	{
		v1 = n68x386ReadReg(pInst, chnl, NX386_R81_TIPV);
		v2 = n68x386ReadReg(pInst, chnl, NX386_R82_RINGV);
		//v1 = n68x386ReadShort(pInst, chnl, NX386_R81_TIPV);
		//v2 = n68x386ReadShort(pInst, chnl, NX386_R82_RINGV);
		if(v1 != -1 && v2 != -1)
		{
			vt += v1;
			vr += v2;
		}
		else
		{
			goto xerr;
		}
	}
	// vt /= 20;
	// vr /= 20;

	vrot = vt;
	vror = vr;
	if(RES_CONST2_X386*vt + RES_CONST1_X386*vt != RES_CONST1_X386*vr)
	{
		x = (RES_CONST2_X386*RES_CONST1_X386*vr - RES_CONST1_X386*vt*RES_CONST2_X386)/(RES_CONST2_X386*vt - RES_CONST1_X386*vr + RES_CONST1_X386*vt);
	}
	else
	{
		x = -1;
	}
	rtr2 = x;
	if(rtr1 >= 0 && rtr2 >= 0)
	{
		if(rtr1 > rtr2)
		{
			x = rtr2;
		}
		else
		{
			x = rtr1;
		}
	}
	else
	{
		if(rtr1 > rtr2)
		{
			x = rtr1;
		}
		else
		{
			x = rtr2;
		}
	}

	rtr = x;
	if(x > 0)
	{
		//ft = RES_CONST2_X386;
		//RES_CONST2_X386 = (ft * x)/(ft + x);
		if(x > 10000)
		{
			if(x > 100000)
			{
				ft = x*10/((x*10/RES_CONST2_X386) + 10);
			}
			else
			{
				ft = (x * RES_CONST2_X386)/(x + RES_CONST2_X386);
			}
		}
		else
		{
			ft = (x * RES_CONST2_X386)/(x + RES_CONST2_X386);
		}

		vr = vtor;
		vt = vtot;
		if(RES_CONST1_X386*vr != vt*(RES_CONST1_X386 + ft))
		{
			x = vt*ft*RES_CONST1_X386/(RES_CONST1_X386*vr - vt*RES_CONST1_X386 - vt*ft);
		}
		else
		{
			x = -1;
		}

		rtg = x;

		vr = vror;
		vt = vrot;
		if(RES_CONST1_X386*vt != vr*(RES_CONST1_X386 + ft))
		{
			x = vr*ft*RES_CONST1_X386/(RES_CONST1_X386*vt - vr*RES_CONST1_X386 - vr*ft);
		}
		else
		{
			x = -1;
		}

		rrg = x;

		//RES_CONST2_X386 = ft;
	}
	else
	{
		vr = vtor;
		vt = vtot;
		if(RES_CONST1_X386*vr != vt*(RES_CONST1_X386 + RES_CONST2_X386))
		{
			x = vt*RES_CONST2_X386*RES_CONST1_X386/(RES_CONST1_X386*vr - vt*RES_CONST1_X386 - vt*RES_CONST2_X386);
		}
		else
		{
			x = -1;
		}

		rtg = x;

		vr = vror;
		vt = vrot;
		if(RES_CONST1_X386*vt != vr*(RES_CONST1_X386 + RES_CONST2_X386))
		{
			x = vr*RES_CONST2_X386*RES_CONST1_X386/(RES_CONST1_X386*vt - vr*RES_CONST1_X386 - vr*RES_CONST2_X386);
		}
		else
		{
			x = -1;
		}

		rrg = x;
	}

	if(RTR) (*RTR) = rtr;
	if(RTG) (*RTG) = rtg;
	if(RRG) (*RRG) = rrg;
	iRet = ((rtr < 150 && rtr > 0) || (rtg < 150 && rtg > 0) || (rrg < 150 && rrg > 0));
xerr:
	N68X386SetLineStatusTo(pInst, chnl, LS_FORWARD_IDLE);
	n68x386WriteReg(pInst, chnl, NX386_R43_LAMC, olamc);
	n68x386WriteReg(pInst, chnl, NX386_R4C_OHVC, oohv);
	n68x386WriteReg(pInst, chnl, NX386_R56_VOVC, ovov);
	n68x386WriteReg(pInst, chnl, NX386_R4D_GMVC, oogm);
	return iRet;
}

NXFUNIMPSTR int N68X386OffhookTest(__chip_inst pInst, __chip_chnl chnl)
{
	int iRet = -1;
	int v, i;
	//int vr, vt;
	__i32 r1, r2;
	//__i32 ft;

	int olamc;
	int oohv;
	int ovov;
	int olcl;

	olamc = n68x386ReadReg(pInst, chnl, NX386_R43_LAMC);
	oohv = n68x386ReadReg(pInst, chnl, NX386_R4C_OHVC);
	ovov = n68x386ReadReg(pInst, chnl, NX386_R56_VOVC);
	olcl = n68x386ReadReg(pInst, chnl, NX386_R45_LCLC);
	if(olamc < 0 || oohv < 0 || ovov < 0 || olcl < 0)return -1;

	n68x386WriteReg(pInst, chnl, NX386_R43_LAMC, 0x06);
	n68x386WriteReg(pInst, chnl, NX386_R45_LCLC, 0x00);

	v = 0x0A;
	n68x386WriteReg(pInst, chnl, NX386_R4C_OHVC, v);

	if(N68X386SetLineStatusTo(pInst, chnl, LS_FORWARD_ONHOOKTRANS) < 0)
	{
		goto xerr;
	}

	while(v < 0x20)
	{
		i = n68x386ReadReg(pInst, chnl, NX386_R90_LPIC);
		if(i < 0) goto xerr;
		if(i >= 0x30) break;
		v += 7;
		n68x386WriteReg(pInst, chnl, NX386_R4C_OHVC, v);
		DELAY_MS(50);
	}

	if(i >= 0x30)
	{
		v = 0x03;
		n68x386WriteReg(pInst, chnl, NX386_R4C_OHVC, v);
		DELAY_MS(350);
		r1 = n68x386ReadReg(pInst, chnl, NX386_R8D_LPVC);
		i = n68x386ReadReg(pInst, chnl, NX386_R90_LPIC);
		r1 *= 1024;
		r1 /= i;
		// if(fr1)((*fr1) = r1);
		v += 7;
		n68x386WriteReg(pInst, chnl, NX386_R4C_OHVC, v);
		DELAY_MS(350);
		r2 = n68x386ReadReg(pInst, chnl, NX386_R8D_LPVC);
		i = n68x386ReadReg(pInst, chnl, NX386_R90_LPIC);
		r2 *= 1024;
		r2 /= i;
		// if(fr2)((*fr2) = r2);
		if(r1 == 0)r1 = 1;
		if(r2 == 0)r2 = 1;
		if(r2 > r1)
		{
			if((r2-r1)*1000/r1 >= 150)
			{
				iRet = 1; // off-hook
			}
			else
			{
				iRet = 2; // resistance fault
			}
		}
		else
		{
			if((r1-r2)*1000/r2 >= 150)
			{
				iRet = 1; // off-hook
			}
			else
			{
				iRet = 2; // resistance fault
			}
		}

	}
	else if(v >= 0x20)
	{
		iRet = 0;
	}
xerr:
	N68X386SetLineStatusTo(pInst, chnl, LS_FORWARD_IDLE);
	n68x386WriteReg(pInst, chnl, NX386_R43_LAMC, olamc);
	n68x386WriteReg(pInst, chnl, NX386_R4C_OHVC, oohv);
	n68x386WriteReg(pInst, chnl, NX386_R56_VOVC, ovov);
	n68x386WriteReg(pInst, chnl, NX386_R45_LCLC, olcl);
	return iRet;
}

NXFUNIMPSTR __i32 N68X386GetVtToVr(__chip_inst pInst, __chip_chnl chnl)
{
	int iTmp;
	int v1, v2;
	__i32 vr, vt;

	int olamc;
	int oohv;
	int ovov;
	int oogm;

	olamc = n68x386ReadReg(pInst, chnl, NX386_R43_LAMC);
	oohv  = n68x386ReadReg(pInst, chnl, NX386_R4C_OHVC);
	ovov  = n68x386ReadReg(pInst, chnl, NX386_R56_VOVC);
	oogm  = n68x386ReadReg(pInst, chnl, NX386_R4D_GMVC);
	if(olamc < 0 || oohv < 0 || ovov < 0 || oogm < 0)return -1;

	n68x386WriteReg(pInst, chnl, NX386_R43_LAMC, 0x06);
	n68x386WriteReg(pInst, chnl, NX386_R56_VOVC, 0x03);
	n68x386WriteReg(pInst, chnl, NX386_R4D_GMVC, 0x03);
	n68x386WriteReg(pInst, chnl, NX386_R4C_OHVC, RES_VOLT_X386);

	if(N68X386SetLineStatusTo(pInst, chnl, LS_TIPOPEN) < 0)
	{
		vt = -1;
		goto err;
	}
	DELAY_MS(400);
	vr = 0;
	vt = 0;
	for(iTmp = 0; iTmp < 20; iTmp ++)
	{
		v1 = n68x386ReadReg(pInst, chnl, NX386_R81_TIPV);
		v2 = n68x386ReadReg(pInst, chnl, NX386_R82_RINGV);
		//v1 = nxReadShort(pInst, chnl, NX386_R81_TIPV);
		//v2 = nxReadShort(pInst, chnl, NX386_R82_RINGV);
		if(v1 != -1 && v2 != -1)
		{
			vt += v1;
			vr += v2;
		}
		else
		{
			vt = -1;
			goto err;
		}
	}
	if(vr)
	{
		vt <<= 10;
		vt /= vr;
	}
	else
	{
		vt = -1;
	}
err:
	N68X386SetLineStatusTo(pInst, chnl, LS_FORWARD_IDLE);
	n68x386WriteReg(pInst, chnl, NX386_R43_LAMC, olamc);
	n68x386WriteReg(pInst, chnl, NX386_R4C_OHVC, oohv);
	n68x386WriteReg(pInst, chnl, NX386_R56_VOVC, ovov);
	n68x386WriteReg(pInst, chnl, NX386_R4D_GMVC, oogm);
	return vt;
}

NXFUNIMPSTR int N68X386CalResConsts(__i32 VtToVrOpen, __i32 VtToVrRx, __i32 RxKOhms, __i32 *CONST1, __i32 *CONST2)
{
	__i32 c1 = VtToVrOpen;
	__i32 c2 = VtToVrRx;
	__i32 r = RxKOhms;
	__i32 x;
	if((c1 == 0) || ((1024 - c1 - c2*(1024 - c1)) == 0)) return -1;
	x = (c2*r*c1 + c2*r*(1024 - c1) - 1024*r*c1)/(1024*1024 - 1024*c1 - c2*(1024 - c1));
	if(CONST1)(*CONST1) = x;
	r = x*(1024 - c1)/c1;
	if(CONST2)(*CONST2) = r;
	return 1;
}
