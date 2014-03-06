/**
	@file
	@brief API functions declarations for N68x38x.
**/

#ifndef N68X386_API_HDR
#define N68X386_API_HDR

NXFUNSTR int N68X386HWReset(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR int N68X386Init(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR int N68X386BATOn(__chip_inst pInst, __chip_chnl chnl, int onoff);
NXFUNSTR int N68X386Caliberate(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR int N68X386SetupRingtone(__chip_inst pInst, __chip_chnl chnl, enum tagRingRegion xregion);
NXFUNSTR int N68X386SetupRing(__chip_inst pInst, __chip_chnl chnl, int freq, __u32 vpeak, __u32 msecOn, __u32 msecOff,
					__u32 dcv, __u32 nsecOffset);
NXFUNSTR int N68X386SetRingActiveTime(__chip_inst pInst, __chip_chnl chnl, __u32 msecOn);
NXFUNSTR int N68X386SetRingInactiveTime(__chip_inst pInst, __chip_chnl chnl, __u32 msecOff);
NXFUNSTR int N68X386SetupTrapRing(__chip_inst pInst, __chip_chnl chnl, int freq, __u32 vpeak, __u32 msecOn, __u32 msecOff,
					__u32 dcv, __u32 nsecOffset, int crest_factor100);
NXFUNSTR int N68X386SetRingPhase(__chip_inst pInst, __chip_chnl chnl, int uSec, int zcEna);
NXFUNSTR int N68X386SetVBHV(__chip_inst pInst, __chip_chnl chnl, __u32 valmV);
NXFUNSTR int N68X386SetVCMR(__chip_inst pInst, __chip_chnl chnl, __u32 valmV);
NXFUNSTR int N68X386SetVROFF(__chip_inst pInst, __chip_chnl chnl, __u32 valmV);
NXFUNSTR int N68X386SetUnbalancedRing(__chip_inst pInst, __chip_chnl chnl, int ena, int onTip);
NXFUNSTR int N68X386SetupRingTripDetect(__chip_inst pInst, __chip_chnl chnl, int currentLim, int freqLP, int intena);
NXFUNSTR int N68X386SetupRingTripCurrent(__chip_inst pInst, __chip_chnl chnl, int currentLim);
NXFUNSTR int N68X386Ring(__chip_inst pInst, __chip_chnl chnl, int en, int intena, int inteni);
NXFUNSTR int N68X386GetLineStatus(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR int N68X386SetLineStatus(__chip_inst pInst, __chip_chnl chnl, NXLINESTATUS st);
NXFUNSTR int N68X386SetLineStatusTo(__chip_inst pInst, __chip_chnl chnl, NXLINESTATUS st);
NXFUNSTR int N68X386GetRTLCStatus(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR int N68X386SetTxDACGain(__chip_inst pInst, __chip_chnl chnl, int valdB);
NXFUNSTR int N68X386SetRxADCGain(__chip_inst pInst, __chip_chnl chnl, int valdB);
NXFUNSTR int N68X386SetLineImpendence(__chip_inst pInst, __chip_chnl chnl, enum tagIMZRegion xregion);
NXFUNSTR int N68X386SetVGM(__chip_inst pInst, __chip_chnl chnl, __u32 valmV);
NXFUNSTR int N68X386SetVOV(__chip_inst pInst, __chip_chnl chnl, __u32 valmV);
NXFUNSTR int N68X386SetVOH(__chip_inst pInst, __chip_chnl chnl, __u32 valmV);
NXFUNSTR int N68X386SetVBLV(__chip_inst pInst, __chip_chnl chnl, __u32 valmV);
NXFUNSTR int N68X386SetVoltageTrack(__chip_inst pInst, __chip_chnl chnl, int ena);
NXFUNSTR int N68X386SetLoopClosureDetect(__chip_inst pInst, __chip_chnl chnl, int CurOrVolt, __i32 valmAmV, int debouncemS);
NXFUNSTR int N68X386SetMaxLoopCurrent(__chip_inst pInst, __chip_chnl chnl, int valmA);
NXFUNSTR int N68X386GetTemperature(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR int N68X386GetBCLK(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR __i32 N68X386GetBATVoltage(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR __i32 N68X386GetTipVoltage(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR __i32 N68X386GetRingVoltage(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR __i32 N68X386GetLoopVoltage(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR __i32 N68X386GetLoopCurrent(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR int N68X386SetPCM(__chip_inst pInst, __chip_chnl chnl, int opts, int *regbuf);
NXFUNSTR int N68X386SetTxTimeSlot(__chip_inst pInst, __chip_chnl chnl, unsigned short valBits);
NXFUNSTR int N68X386SetRxTimeSlot(__chip_inst pInst, __chip_chnl chnl, unsigned short valBits);
NXFUNSTR int N68X386SetRxAPG(__chip_inst pInst, __chip_chnl chnl, enum tagAPGValue val);
NXFUNSTR int N68X386SetTxAPG(__chip_inst pInst, __chip_chnl chnl, enum tagAPGValue val);
NXFUNSTR int N68X386GetInterrupts(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR int N68X386IntEnable(__chip_inst pInst, __chip_chnl chnl, int ints);
NXFUNSTR int N68X386IntDisable(__chip_inst pInst, __chip_chnl chnl, int ints);
NXFUNSTR int N68X386GenDialTone(__chip_inst pInst, __chip_chnl chnl, enum tagDialtoneRegion xregion);
NXFUNSTR int N68X386GenRingbackTone(__chip_inst pInst, __chip_chnl chnl, enum tagRingbacktoneRegion xregion);
NXFUNSTR int N68X386GenBusyTone(__chip_inst pInst, __chip_chnl chnl, enum tagBusytoneRegion xregion);
NXFUNSTR int N68X386StopTones(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR int N68X386DiagDLoop0(__chip_inst pInst, __chip_chnl chnl, int ena);
NXFUNSTR int N68X386DiagDLoop1(__chip_inst pInst, __chip_chnl chnl, int ena);
NXFUNSTR int N68X386DiagDLoop2(__chip_inst pInst, __chip_chnl chnl, int ena);
NXFUNSTR int N68X386DiagALoop0(__chip_inst pInst, __chip_chnl chnl, int ena);
NXFUNSTR int N68X386DiagALoop1(__chip_inst pInst, __chip_chnl chnl, int ena);
NXFUNSTR int N68X386DiagXConfig(__chip_inst pInst, __chip_chnl chnl, __u16 opts);
NXFUNSTR int N68X386DiagXSetACSrc(__chip_inst pInst, __chip_chnl chnl, enum tagDIAGXSrc src);
NXFUNSTR int N68X386DiagXSetDCSrc(__chip_inst pInst, __chip_chnl chnl, enum tagDIAGXSrc src);
NXFUNSTR int N68X386DiagXTMSetSrc(__chip_inst pInst, __chip_chnl chnl, enum tagDIAGXSrc src);
NXFUNSTR int N68X386DiagXTMSetHighThreshold(__chip_inst pInst, __chip_chnl chnl, __u16 val);
NXFUNSTR int N68X386DiagXTMSetLowThreshold(__chip_inst pInst, __chip_chnl chnl, __u16 val);
NXFUNSTR int N68X386DiagXTMEnable(__chip_inst pInst, __chip_chnl chnl, int ena);
NXFUNSTR int N68X386DiagXTMGetValue(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR int N68X386DiagXIsFifo0Done(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR int N68X386DiagXIsFifo1Done(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR int N68X386OpenVoltageTest(__chip_inst pInst, __chip_chnl chnl, __i32 *acmVloop, __i32 *dcmVloop, __i32 *acmVring, __i32 *dcmVring, __i32 *acmVtip, __i32 *dcmVtip, int aForeignVolt);
NXFUNSTR int N68X386LowRENTest(__chip_inst pInst, __chip_chnl chnl, __i32 *bellowDot175, __i32 *calibConst);
NXFUNSTR int N68X386RENTest(__chip_inst pInst, __chip_chnl chnl, __i32 *above5REN, __i32 *bellowDot175, __i32 *calibConstHigh, __i32 *calibConstLow);
NXFUNSTR int N68X386ResFaultTest(__chip_inst pInst, __chip_chnl chnl, __i32 *RTR, __i32 *RTG, __i32 *RRG);
NXFUNSTR int N68X386OffhookTest(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR __i32 N68X386GetVtToVr(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR int N68X386CalResConsts(__i32 VtToVrOpen, __i32 VtToVrRx, __i32 RxKOhms, __i32 *CONST1, __i32 *CONST2);
NXFUNSTR int N68X386DTMFGen(__chip_inst pInst, __chip_chnl chnl, unsigned char xdigit, unsigned int msActive, unsigned int msInactive, int db);
NXFUNIMPSTR int N68X386ToneGen(__chip_inst pInst, __chip_chnl chnl, unsigned int osc1, unsigned int osic1,
                                        unsigned int osc2, unsigned int osic2, unsigned int msActive, unsigned int msInactive);
NXFUNSTR int N68X386FSKGenSetup(__chip_inst pInst, __chip_chnl chnl, int opts);
NXFUNSTR int N68X386FSKGenSetLevel(__chip_inst pInst, __chip_chnl chnl, int dBVal);
NXFUNSTR int N68X386FSKGenWrite(__chip_inst pInst, __chip_chnl chnl, unsigned char *dat, int cnt);
NXFUNSTR int N68X386FSKGenIsBusy(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR int N68X386DTMFDSetInput(__chip_inst pInst, __chip_chnl chnl, int adcOrdac);
NXFUNSTR int N68X386DTMFDSetFreqError(__chip_inst pInst, __chip_chnl chnl, int tolerancelevel);
NXFUNSTR int N68X386DTMFDSetDecSpeed(__chip_inst pInst, __chip_chnl chnl, int speedlevel);
NXFUNSTR int N68X386DTMFDSetSigLevel(__chip_inst pInst, __chip_chnl chnl, int dBVal);
NXFUNSTR int N68X386DTMFDSetTimes(__chip_inst pInst, __chip_chnl chnl, int msPreActive, int msActive, int msInactive);
NXFUNSTR int N68X386DTMFDGetData(__chip_inst pInst, __chip_chnl chnl);
NXFUNSTR int N68X386DTMFDEnable(__chip_inst pInst, __chip_chnl chnl, int ena);

#endif // N68X386_API_HDR
