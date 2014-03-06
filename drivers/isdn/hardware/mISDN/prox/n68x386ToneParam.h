/**
	@file
	@brief Tone generate parameters for N68x38x.
**/

#ifndef N68X386TONEPARAMHDR
#define N68X386TONEPARAMHDR

// for tones

#ifndef PROXTONEGENSTRUCE
#define PROXTONEGENSTRUCE 1
typedef struct tagXVAL_TONE
{
	unsigned short  OSC1_FREQ;
	unsigned short  OSC1_AMP;
	unsigned short  OSC1_OnTime;
	unsigned short  OSC1_OffTime;
	unsigned short  OSC2_FREQ;
	unsigned short  OSC2_AMP;
	unsigned short  OSC2_OnTime;
	unsigned short  OSC2_OffTime;
}XVAL_TONE;
#endif // PROXTONEGENSTRUCE

#if WIDE_BAND // todo

static const XVAL_TONE DialtoneValues_x386[] = 
{
	// OS1C(Freq)	OS1IC (Ampl)	OS1AT(OnTime)	OS1IT (OffTime)	OS2C (Freq)	OS2IC (Amplitude) 	OS2AT (OnTime) 	OS2IT (OffTime)
#if CONFIG_TONE_USA
	{ 0x7E18,	0x036C,	0x1F40,	0,	0x7ECB,	0x02BA,	0x1F40,	0 },	//USA	// 440+350 @ -13dBm
#endif
#if CONFIG_TONE_JAPAN
	{ 0x7E6D,	0x031D,	0x1F40,	0,	0,	0,	0,	0 },	// JAPAN	// 400 @ -13dBm
#endif
#if CONFIG_TONE_KOREA
	{ 0x7E18,	0x036C,	0x1F40,	0,	0x7ECB,	0x02BA,	0x1F40,	0 },	// KOREA 	// 440+350 @ -13dBm
#endif
#if CONFIG_TONE_CHINA
	{ 0x7E18,	0x036C,	0x1F40,	0,	0x7ECB,	0x02BA,	0x1F40,	0 },	// CHINA	// 440+350 @ -13dBm
#endif
#if CONFIG_TONE_FRANCE
	{ 0x7E18,	0x036C,	0x1F40,	0,	0,	0,	0,	0 },	// FRANCE	// 440 @ -13dBm
#endif
#if CONFIG_TONE_ITALY
	{ 0x7E38,	0x034E,	0x12C0,	0x0320,	0,	0,	0,	0 },	// ITALY	// 425 @ -13dBm
#endif
#if CONFIG_TONE_GERMANY
	{ 0x7E38,	0x034E,	0x1F40,	0,	0,	0,	0,	0 },	// GERMANY	// 425 @ -13dBm
#endif
#if CONFIG_TONE_UK
	{ 0x7E18,	0x036C,	0x12C0,	0x1F40,	0x7ECB,	0x02BA,	0x1F40,	0 },	// UK	// 440+350 @ -13dBm
#endif
#if CONFIG_TONE_BELGUIM
	{ 0x7E02,	0x037F,	0x1F40,	0,	0,	0,	0,	0 },	// BELGUIM 	// 450 @ -13dBm
#endif
#if CONFIG_TONE_ISRAEL
	{ 0x7E6D,	0x031D,	0x1F40,	0,	0,	0,	0,	0 },	// ISRAEL	//400 @ -13dBm
#endif
#if CONFIG_TONE_NETHERLANDS
	{ 0x7E38,	0x034E,	0x1F40,	0,	0,	0,	0,	0 },	// NETHERLANDS	//425 @ -13dBm
#endif
#if CONFIG_TONE_NORWAY
	{ 0x7E38,	0x034E,	0x1F40,	0,	0,	0,	0,	0 },	// NORWAY	//425 @ -13dBm
#endif
#if CONFIG_TONE_SINGAPORE
	{ 0x7EFE,	0x027E,	0x1F40,	0,	0x7F48,	0x021B,	0x1F40,	0 },	// SINGAPORE	//320 + 270 @ -13dBm
#endif
#if CONFIG_TONE_SWEDEN
	{ 0x7E38,	0x034E,	0x1F40,	0,	0,	0,	0,	0 },	// SWEDEN	//425 @ -13dBm
#endif
#if CONFIG_TONE_SWITZERLAND
	{ 0x7E38,	0x034E,	0x1F40,	0,	0,	0,	0,	0 },	// SWITZERLAND	//425 @ -13dBm
#endif
#if CONFIG_TONE_TAIWAN
	{ 0x7E18,	0x036C,	0x1F40,	0,	0x7ECB,	0x02BA,	0x1F40,	0 },	// TAIWAN	// 440+350 @ -13dBm
#endif
};

static const XVAL_TONE RingbacktoneValues_x386[] = 
{
	// OS1C(Freq)	OS1IC (Ampl)	OS1AT(OnTime)	OS1IT (OffTime)	OS2C (Freq)	OS2IC (Amplitude) 	OS2AT (OnTime) 	OS2IT (OffTime)
#if CONFIG_TONE_USA
	{ 0x7DBC,	0x01DE,	0x3E80,	0x7D00,	0x7E18,	0x01B7,	0x3E80,	0x7D00 }, // USA	// 480+440 @ -19dBm
#endif
#if CONFIG_TONE_JAPAN
	{ 0x7E4C,	0x019F,	0x1F40,	0x3E80,	0x7E8C,	0x017F,	0x1F40,	0x3E80 }, // JAPAN	// 416+384 @ -19dBm
#endif
#if CONFIG_TONE_KOREA
	{ 0x7DBC,	0x01DE,	0x3E80,	0x7D00,	0x7E18,	0x01B7,	0x3E80,	0x7D00 }, // KOREA	// 480+440 @ -19dBm
#endif
#if CONFIG_TONE_CHINA
	{ 0x7DBC,	0x01DE,	0x3E80,	0x7D00,	0x7E18,	0x01B7,	0x3E80,	0x7D00 }, // CHINA	// 480+440 @ -19dBm
#endif
#if CONFIG_TONE_FRANCE
	{ 0x7E6D,	0x018F,	0x338F,	0x68AF,	0,	0,	0,	0 },	// FRANCE	// 400 @ -19dBm
#endif
#if CONFIG_TONE_ITALY
	{ 0x7E39,	0x01A8,	0x1F40,	0x7D00,	0,	0,	0,	0 },	// ITALY	// 425 @ 19dBm
#endif
#if CONFIG_TONE_GERMANY
	{ 0x7E39,	0x01A8,	0x1F40,	0x7D00,	0,	0,	0,	0 },	// GERMANY// 425 @ 19dBm
#endif
#if CONFIG_TONE_UK
	{ 0x7E02,	0x01C1,	0x1F40,	0x7D00,	0x7E6C,	0x018F,	0x1F40,	0x7D00 },	// UK	// 450+400@ -19dBm
#endif
#if CONFIG_TONE_BELGUIM
	{ 0x7E02,	0x01C1,	0x1F40,	0x5DC0,	0,	0,	0x1F40,	0 },	// BELGUIM// 450 @ -19dBm
#endif
#if CONFIG_TONE_ISRAEL
	{ 0x7E02,	0x01C1,	0x1F40,	0x5DC0,	0x7E6C,	0x018F,	0x1F40,	0x5DC0 },	// ISRAEL// 450+400@ -19dBm
#endif
#if CONFIG_TONE_NETHERLANDS
	{ 0x7E39,	0x01A8,	0x1F40,	0x7D00,	0,	0,	0,	0 },	// NETHERLANDS//425 @ -19dBm
#endif
#if CONFIG_TONE_NORWAY
	{ 0x7E39,	0x01A8,	0x1F40,	0x7D00,	0,	0,	0,	0 },	// NORWAY//425 @ -19dBm
#endif
#if CONFIG_TONE_SINGAPORE
	{ 0x7E6D,	0x018F,	0x1F40,	0x3E80,	0,	0,	0,	0 },	// SINGAPORE//400 @ -19dBm
#endif
#if CONFIG_TONE_SWEDEN
	{ 0x7E39,	0x01A8,	0x1F40,	0x7D00,	0,	0,	0,	0 },	// SWEDEN	//425 @ -19dBm
#endif
#if CONFIG_TONE_SWITZERLAND
	{ 0x7E39,	0x01A8,	0x1F40,	0x7D00,	0,	0,	0,	0 },	// SWITZERLAND//425 @ -19dBm
#endif
#if CONFIG_TONE_TAIWAN
	{ 0x7DBC,	0x01DE,	0x3E80,	0x7D00,	0x7E18,	0x01B7,	0x3E80,	0x7D00 },	// TAIWAN // 480+440 @ -19dBm
#endif
};

static const XVAL_TONE BusytoneValues_x386[] = 
{
	// OS1C(Freq)	OS1IC (Ampl)	OS1AT(OnTime)	OS1IT (OffTime)	OS2C (Freq)	OS2IC (Amplitude) 	OS2AT (OnTime) 	OS2IT (OffTime)
#if CONFIG_TONE_USA
	{ 0x7E18,	0x01B7,	0x0FA0,	0x0FA0,	0x7C39,	0x0267,	0x0FA0,	0x0FA0 },	// USA	// 440+620 @ -19dBm
#endif
#if CONFIG_TONE_JAPAN
	{ 0x7E6D,	0x018F,	0x0FA0,	0x0FA0,	0,	0,	0,	0 },	// JAPAN	// 400 @ -19dBm
#endif
#if CONFIG_TONE_KOREA
	{ 0x7E18,	0x01B7,	0x0FA0,	0x0FA0,	0x7C39,	0x0267,	0x0FA0,	0x0FA0 },	// KOREA	// 440+620 @ -19dBm
#endif
#if CONFIG_TONE_CHINA
	{ 0x7E18,	0x01B7,	0x0FA0,	0x0FA0,	0x7C39,	0x0267,	0x0FA0,	0x0FA0 },	// CHINA	// 440+620 @ -19dBm
#endif
#if CONFIG_TONE_FRANCE
	{ 0x7E18,	0x01B7,	0x0FA0,	0x0FA0,	0,	0,	0,	0 },	// FRANCE// 440 @ -19dBm
#endif
#if CONFIG_TONE_ITALY
	{ 0x7E39,	0x01A8,	0x0FA0,	0x0FA0,	0,	0,	0,	0 },	// ITALY// 425 @ -19dBm
#endif
#if CONFIG_TONE_GERMANY
	{ 0x7E39,	0x01A8,	0x0FA0,	0x0FA0,	0,	0,	0,	0 },	// GERMANY// 425 @ -19dBm
#endif
#if CONFIG_TONE_UK
	{ 0x7E18,	0x01B7,	0x0C80,	0x0C80,	0,	0,	0,	0 },	// UK	// 440 @ -19dBm
#endif
#if CONFIG_TONE_BELGUIM
	{ 0x7E02,	0x01C1,	0x04B0,	0x04B0,	0,	0,	0,	0 },	// BELGUIM// 450 @ -19dBm
#endif
#if CONFIG_TONE_ISRAEL
	{ 0x7E18,	0x01B7,	0x0C80,	0x0C80,	0,	0,	0,	0 },	// ISRAEL// 440 @ -19dBm
#endif
#if CONFIG_TONE_NETHERLANDS
	{ 0x7E39,	0x01A8,	0x0FA0,	0x0FA0,	0,	0,	0,	0 },	// NETHERLANDS// 425 @ -19dBm
#endif
#if CONFIG_TONE_NORWAY
	{ 0x7E39,	0x01A8,	0x0FA0,	0x0FA0,	0,	0,	0,	0 },	// NORWAY	// 425 @ -19dBm
#endif
#if CONFIG_TONE_SINGAPORE
	{ 0x7EFE,	0x0140,	0x1770,	0x1770,	0x7F48,	0x010E,	0x1770,	0x1770 },	// SINGAPORE	//320 + 270 @ -19dBm
#endif
#if CONFIG_TONE_SWEDEN
	{ 0x7E39,	0x01A8,	0x07D0,	0x07D0,	0,	0,	0,	0 },	// SWEDEN//425 @ -13dBm
#endif
#if CONFIG_TONE_SWITZERLAND
	{ 0x7E39,	0x01A8,	0x0FA0,	0x0FA0,	0,	0,	0,	0 },	// SWITZERLAND// 425 @ -19dBm
#endif
#if CONFIG_TONE_TAIWAN
	{ 0x7E18,	0x01B7,	0x0FA0,	0x0FA0,	0x7C39,	0x0267,	0x0FA0,	0x0FA0 }, // TAIWAN // 440+620 @ -19dBm
#endif
};

#else // WIDE_BAND

static const XVAL_TONE DialtoneValues_x386[] = 
{
	// OS1C(Freq)	OS1IC (Ampl)	OS1AT(OnTime)	OS1IT (OffTime)	OS2C (Freq)	OS2IC (Amplitude) 	OS2AT (OnTime) 	OS2IT (OffTime)
#if CONFIG_TONE_USA
	{ 0x7E18,	0x036C,	0x1F40,	0,	0x7ECB,	0x02BA,	0x1F40,	0 },  //USA	// 440+350 @ -13dBm
#endif
#if CONFIG_TONE_JAPAN
	{ 0x7E6D,	0x031D,	0x1F40,	0,	0,	0,	0,	0 },	// JAPAN	// 400 @ -13dBm
#endif
#if CONFIG_TONE_KOREA
	{ 0x7E18,	0x036C,	0x1F40,	0,	0x7ECB,	0x02BA,	0x1F40,	0 },	// KOREA 	// 440+350 @ -13dBm
#endif
#if CONFIG_TONE_CHINA
	{ 0x7E18,	0x036C,	0x1F40,	0,	0x7ECB,	0x02BA,	0x1F40,	0 },	// CHINA	// 440+350 @ -13dBm
#endif
#if CONFIG_TONE_FRANCE
	{ 0x7E18,	0x036C,	0x1F40,	0,	0,	0,	0,	0 },	// FRANCE	// 440 @ -13dBm
#endif
#if CONFIG_TONE_ITALY
	{ 0x7E38,	0x034E,	0x12C0,	0x0320,	0,	0,	0,	0 },	// ITALY	// 425 @ -13dBm
#endif
#if CONFIG_TONE_GERMANY
	{ 0x7E38,	0x034E,	0x1F40,	0,	0,	0,	0,	0 },	// GERMANY	// 425 @ -13dBm
#endif
#if CONFIG_TONE_UK
	{ 0x7E18,	0x036C,	0x12C0,	0x1F40,	0x7ECB,	0x02BA,	0x1F40,	0 },	// UK	// 440+350 @ -13dBm
#endif
#if CONFIG_TONE_BELGUIM
	{ 0x7E02,	0x037F,	0x1F40,	0,	0,	0,	0,	0 },	// BELGUIM 	// 450 @ -13dBm
#endif
#if CONFIG_TONE_ISRAEL
	{ 0x7E6D,	0x031D,	0x1F40,	0,	0,	0,	0,	0 },	// ISRAEL	//400 @ -13dBm
#endif
#if CONFIG_TONE_NETHERLANDS
	{ 0x7E38,	0x034E,	0x1F40,	0,	0,	0,	0,	0 },	// NETHERLANDS	//425 @ -13dBm
#endif
#if CONFIG_TONE_NORWAY
	{ 0x7E38,	0x034E,	0x1F40,	0,	0,	0,	0,	0 },	// NORWAY	//425 @ -13dBm
#endif
#if CONFIG_TONE_SINGAPORE
	{ 0x7EFE,	0x027E,	0x1F40,	0,	0x7F48,	0x021B,	0x1F40,	0 },	// SINGAPORE	//320 + 270 @ -13dBm
#endif
#if CONFIG_TONE_SWEDEN
	{ 0x7E38,	0x034E,	0x1F40,	0,	0,	0,	0,	0 },	// SWEDEN	//425 @ -13dBm
#endif
#if CONFIG_TONE_SWITZERLAND
	{ 0x7E38,	0x034E,	0x1F40,	0,	0,	0,	0,	0 },	// SWITZERLAND	//425 @ -13dBm
#endif
#if CONFIG_TONE_TAIWAN
	{ 0x7E18,	0x036C,	0x1F40,	0,	0x7ECB,	0x02BA,	0x1F40,	0 },	// TAIWAN	// 440+350 @ -13dBm
#endif
};

static const XVAL_TONE RingbacktoneValues_x386[] = 
{
	// OS1C(Freq)	OS1IC (Ampl)	OS1AT(OnTime)	OS1IT (OffTime)	OS2C (Freq)	OS2IC (Amplitude) 	OS2AT (OnTime) 	OS2IT (OffTime)
#if CONFIG_TONE_USA
	{ 0x7DBC,	0x01DE,	0x3E80,	0x7D00,	0x7E18,	0x01B7,	0x3E80,	0x7D00 }, // USA	// 480+440 @ -19dBm
#endif
#if CONFIG_TONE_JAPAN
	{ 0x7E4C,	0x019F,	0x1F40,	0x3E80,	0x7E8C,	0x017F,	0x1F40,	0x3E80 }, // JAPAN	// 416+384 @ -19dBm
#endif
#if CONFIG_TONE_KOREA
	{ 0x7DBC,	0x01DE,	0x3E80,	0x7D00,	0x7E18,	0x01B7,	0x3E80,	0x7D00 }, // KOREA	// 480+440 @ -19dBm
#endif
#if CONFIG_TONE_CHINA
	{ 0x7DBC,	0x01DE,	0x3E80,	0x7D00,	0x7E18,	0x01B7,	0x3E80,	0x7D00 }, // CHINA	// 480+440 @ -19dBm
#endif
#if CONFIG_TONE_FRANCE
	{ 0x7E6D,	0x018F,	0x338F,	0x68AF,	0,	0,	0,	0 },	// FRANCE	// 400 @ -19dBm
#endif
#if CONFIG_TONE_ITALY
	{ 0x7E39,	0x01A8,	0x1F40,	0x7D00,	0,	0,	0,	0 },	// ITALY	// 425 @ 19dBm
#endif
#if CONFIG_TONE_GERMANY
	{ 0x7E39,	0x01A8,	0x1F40,	0x7D00,	0,	0,	0,	0 },	// GERMANY// 425 @ 19dBm
#endif
#if CONFIG_TONE_UK
	{ 0x7E02,	0x01C1,	0x1F40,	0x7D00,	0x7E6C,	0x018F,	0x1F40,	0x7D00 },	// UK	// 450+400@ -19dBm
#endif
#if CONFIG_TONE_BELGUIM
	{ 0x7E02,	0x01C1,	0x1F40,	0x5DC0,	0,	0,	0x1F40,	0 },	// BELGUIM// 450 @ -19dBm
#endif
#if CONFIG_TONE_ISRAEL
	{ 0x7E02,	0x01C1,	0x1F40,	0x5DC0,	0x7E6C,	0x018F,	0x1F40,	0x5DC0 },	// ISRAEL// 450+400@ -19dBm
#endif
#if CONFIG_TONE_NETHERLANDS
	{ 0x7E39,	0x01A8,	0x1F40,	0x7D00,	0,	0,	0,	0 },	// NETHERLANDS//425 @ -19dBm
#endif
#if CONFIG_TONE_NORWAY
	{ 0x7E39,	0x01A8,	0x1F40,	0x7D00,	0,	0,	0,	0 },	// NORWAY//425 @ -19dBm
#endif
#if CONFIG_TONE_SINGAPORE
	{ 0x7E6D,	0x018F,	0x1F40,	0x3E80,	0,	0,	0,	0 },	// SINGAPORE//400 @ -19dBm
#endif
#if CONFIG_TONE_SWEDEN
	{ 0x7E39,	0x01A8,	0x1F40,	0x7D00,	0,	0,	0,	0 },	// SWEDEN	//425 @ -19dBm
#endif
#if CONFIG_TONE_SWITZERLAND
	{ 0x7E39,	0x01A8,	0x1F40,	0x7D00,	0,	0,	0,	0 },	// SWITZERLAND//425 @ -19dBm
#endif
#if CONFIG_TONE_TAIWAN
	{ 0x7DBC,	0x01DE,	0x3E80,	0x7D00,	0x7E18,	0x01B7,	0x3E80,	0x7D00 },	// TAIWAN // 480+440 @ -19dBm
#endif
};

static const XVAL_TONE BusytoneValues_x386[] = 
{
	// OS1C(Freq)	OS1IC (Ampl)	OS1AT(OnTime)	OS1IT (OffTime)	OS2C (Freq)	OS2IC (Amplitude) 	OS2AT (OnTime) 	OS2IT (OffTime)
#if CONFIG_TONE_USA
	{ 0x7E18,	0x01B7,	0x0FA0,	0x0FA0,	0x7C39,	0x0267,	0x0FA0,	0x0FA0 },	// USA	// 440+620 @ -19dBm
#endif
#if CONFIG_TONE_JAPAN
	{ 0x7E6D,	0x018F,	0x0FA0,	0x0FA0,	0,	0,	0,	0 },	// JAPAN	// 400 @ -19dBm
#endif
#if CONFIG_TONE_KOREA
	{ 0x7E18,	0x01B7,	0x0FA0,	0x0FA0,	0x7C39,	0x0267,	0x0FA0,	0x0FA0 },	// KOREA	// 440+620 @ -19dBm
#endif
#if CONFIG_TONE_CHINA
	{ 0x7E18,	0x01B7,	0x0FA0,	0x0FA0,	0x7C39,	0x0267,	0x0FA0,	0x0FA0 },	// CHINA	// 440+620 @ -19dBm
#endif
#if CONFIG_TONE_FRANCE
	{ 0x7E18,	0x01B7,	0x0FA0,	0x0FA0,	0,	0,	0,	0 },	// FRANCE// 440 @ -19dBm
#endif
#if CONFIG_TONE_ITALY
	{ 0x7E39,	0x01A8,	0x0FA0,	0x0FA0,	0,	0,	0,	0 },	// ITALY// 425 @ -19dBm
#endif
#if CONFIG_TONE_GERMANY
	{ 0x7E39,	0x01A8,	0x0FA0,	0x0FA0,	0,	0,	0,	0 },	// GERMANY// 425 @ -19dBm
#endif
#if CONFIG_TONE_UK
	{ 0x7E18,	0x01B7,	0x0C80,	0x0C80,	0,	0,	0,	0 },	// UK	// 440 @ -19dBm
#endif
#if CONFIG_TONE_BELGUIM
	{ 0x7E02,	0x01C1,	0x04B0,	0x04B0,	0,	0,	0,	0 },	// BELGUIM// 450 @ -19dBm
#endif
#if CONFIG_TONE_ISRAEL
	{ 0x7E18,	0x01B7,	0x0C80,	0x0C80,	0,	0,	0,	0 },	// ISRAEL// 440 @ -19dBm
#endif
#if CONFIG_TONE_NETHERLANDS
	{ 0x7E39,	0x01A8,	0x0FA0,	0x0FA0,	0,	0,	0,	0 },	// NETHERLANDS// 425 @ -19dBm
#endif
#if CONFIG_TONE_NORWAY
	{ 0x7E39,	0x01A8,	0x0FA0,	0x0FA0,	0,	0,	0,	0 },	// NORWAY	// 425 @ -19dBm
#endif
#if CONFIG_TONE_SINGAPORE
	{ 0x7EFE,	0x0140,	0x1770,	0x1770,	0x7F48,	0x010E,	0x1770,	0x1770 },	// SINGAPORE	//320 + 270 @ -19dBm
#endif
#if CONFIG_TONE_SWEDEN
	{ 0x7E39,	0x01A8,	0x07D0,	0x07D0,	0,	0,	0,	0 },	// SWEDEN//425 @ -13dBm
#endif
#if CONFIG_TONE_SWITZERLAND
	{ 0x7E39,	0x01A8,	0x0FA0,	0x0FA0,	0,	0,	0,	0 },	// SWITZERLAND// 425 @ -19dBm
#endif
#if CONFIG_TONE_TAIWAN
	{ 0x7E18,	0x01B7,	0x0FA0,	0x0FA0,	0x7C39,	0x0267,	0x0FA0,	0x0FA0 }, // TAIWAN // 440+620 @ -19dBm
#endif
};

#endif // WIDE_BAND

#endif // N68X386TONEPARAMHDR
