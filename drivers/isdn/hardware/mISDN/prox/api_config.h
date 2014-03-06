/**
	@file
	@brief Your configuration for the API.
**/

#ifndef PROXAPICONFIGHDR
#define PROXAPICONFIGHDR

#ifndef FORDOCUMENT
#define FORDOCUMENT 0
#endif

#define FXS_PROX 1

//#define NXDBG(arg...) printk(KERN_NOTICE arg)
//#define NXDBG(arg...) SYSLOG(LOG_DEBUG, arg)
//#define NXDBG(arg...) printf(arg)

/**
	@addtogroup G01_csd Chip Selection
	Define one of the chip that you are using to 1, others to 0.

	Define wether you are using wide band configuration.

	Define wether you are using N681622 line driver.
	@{
*/

#if FORDOCUMENT
#define N681386 1       ///< If you are using N681386, define this value to 1, else undefine this.
#define N682386 1       ///< If you are using N682386, define this value to 1, else undefine this.
#define N681387 1       ///< If you are using N681387, define this value to 1, else undefine this.
#define N682387 1       ///< If you are using N682387, define this value to 1, else undefine this.
#define W684386 1       ///< If you are using W684386, define this value to 1, else undefine this.
#define W681388 1       ///< If you are using W681388, define this value to 1, else undefine this.
#undef N681386
#undef N682386
#undef N681387
#undef N682387
#undef W684386
#undef W681388
#else
#define N681386 1	///< If you are using N681386, define this value to 1, else undefine this.
//#define N682386 1	///< If you are using N682386, define this value to 1, else undefine this.
//#define N681387 1	///< If you are using N681387, define this value to 1, else undefine this.
//#define N682387 1	///< If you are using N682387, define this value to 1, else undefine this.
//#define W684386 1	///< If you are using W684386, define this value to 1, else undefine this.
//#define W681388 1	///< If you are using W681388, define this value to 1, else undefine this.
#endif

#define WIDE_BAND 0	///< If you are using N681387/N682387 with wide band (16K SR), define this value to 1, else 0.

#define N681622 1	///< If you are using N681622 integrated line driver, define this value to 1, else 0.

#define HAS_FLOAT_POINT 0 ///< If your compiler supports soft-float or your MCU has floating point feature, set to 1.

#define HAS_SIN_COS 0 ///< If you have sin/cos math function library, define this to 1.

#define SPI_DAISY_CHAIN 1 ///< If the SPI of the chips are connected in daisy chain mode define to 1. Used in low level xxxxRW.C.

#define GAIN_STEP_0DOT5 1 ///< If you want to set the DAC/ADC and DTMF gain in 0.5dB step, define this to 1. Then the parameter for these functions should be dBVal*10 to make it as a integer data type. E.g. -125 means -12.5dB. In 1dB step it is -12 or -13.

#define MULTICHNL_SEPERATE_IMZ 1///< If you want to use different IMZ settings for multi-channel device, define this

#ifndef __LINUX__
#define __LINUX__	///< For Linux, please define this.
#endif
#ifndef __WIN32__
//#define __WIN32__	///< For Windows/WinCE please define this.
#endif
#ifndef __KERNEL__
#define __KERNEL__	///< For drivers(Win32/Linux) please define this.
#endif
#ifndef __MYSON__
//#define __MYSON__ ///< For Myson
#endif

#if 1
#define NXFUNSTR extern ///< NXFUNSTR is the nx functions storage modifier of function declaration. When you want to set nx functions to static, define this to "static".
#define NXFUNIMPSTR ///<  NXFUNSTR is the nx functions storage modifier of function implementation/defination. When you want to set nx functions to static, define this to "static".
#else
#define NXFUNSTR static
#define NXFUNIMPSTR static
#endif

//#define TDM400MODULE	1
//#define TDM2400MODULE 1
//#define TDM24XXMODULE 1

//#define CHIP_CHANNELS(argInst, argChnl) nxGetChipChannels(argInst, argChnl)
#define DYN_CHIP_CHANNELS 0

#define CHIP_CHANNELS(argInst, argChnl) 1

/**
	@}
*/

/**
	@addtogroup G02_fsd Functions Selection
	@{
*/

/**
	@brief Whether you want to use API hardware initialization function.
	
	If you want to use, define to 1, else 0.
	
	You have to supply/define functions nxHardwareInit(), nxAssertReset(), nxReleaseReset()
	for this feature. If you do hardware reset by yourself, you don't need this feature.

	Please see nxHWReset() for details.
*/

// when generate document, please notice bellow #if USE_HARDWARE_INIT
#define USE_HARDWARE_INIT	1

/**
	@}
*/

/**
	@addtogroup G03_imzd Impendence Presets Selection

	For N68x38x, the impedence presets will takes up to 300 bytes max each.
	If you want to select which impedence presets you want to use, modify these definitions.
	@{
*/
#define	CONFIG_IMZ_Japan_CO			1	///< Define to 1 to use impendence presets Japan CO, else 0
#define	CONFIG_IMZ_Bellcore			1	///< Define to 1 to use impendence presets Bellcore, else 0
#define	CONFIG_IMZ_CTR21			1	///< Define to 1 to use impendence presets CTR21, else 0
#define	CONFIG_IMZ_China_CO			1	///< Define to 1 to use impendence presets China CO, else 0
#define	CONFIG_IMZ_China_PBX		1	///< Define to 1 to use impendence presets China PBX, else 0
#define	CONFIG_IMZ_Japan_PBX		1	///< Define to 1 to use impendence presets Japan PBX, else 0
#define	CONFIG_IMZ_India_NewZealand	1	///< Define to 1 to use impendence presets NewZealand, else 0
#define	CONFIG_IMZ_Germany_Legacy	1	///< Define to 1 to use impendence presets Germany Legacy, else 0
#define	CONFIG_IMZ_UK_Legacy		1	///< Define to 1 to use impendence presets UK Legacy, else 0
#define	CONFIG_IMZ_Australia		1	///< Define to 1 to use impendence presets Australia, else 0
#define	CONFIG_IMZ_Variation		1	///< Define to 1 to use impendence presets Variation, else 0
/**
	@}
*/

/**
	@addtogroup G031_td Tone parameters Presets Selection

	If you want to select which tone presets you want to use, modify these definitions.
	@{
*/
#define CONFIG_TONE_USA				1
#define CONFIG_TONE_JAPAN			1
#define CONFIG_TONE_KOREA			1
#define CONFIG_TONE_CHINA			1
#define CONFIG_TONE_FRANCE			1
#define CONFIG_TONE_ITALY			1
#define CONFIG_TONE_GERMANY			1
#define CONFIG_TONE_UK				1
#define CONFIG_TONE_BELGUIM			1
#define CONFIG_TONE_ISRAEL			1
#define CONFIG_TONE_NETHERLANDS		1
#define CONFIG_TONE_NORWAY			1
#define CONFIG_TONE_SINGAPORE		1
#define CONFIG_TONE_SWEDEN			1
#define CONFIG_TONE_SWITZERLAND		1
#define CONFIG_TONE_TAIWAN			1
/**
	@}
*/

/**
	@addtogroup G032_rd Ringer parameters Presets Selection

	If you want to select which ringer presets you want to use, modify these definitions.
	@{
*/
#define CONFIG_RINGER_USA			1
#define CONFIG_RINGER_JAPAN			1
#define CONFIG_RINGER_KOREA			1
#define CONFIG_RINGER_CHINA			1
#define CONFIG_RINGER_FRANCE		1
#define CONFIG_RINGER_ITALY			1
#define CONFIG_RINGER_GERMANY		1
#define CONFIG_RINGER_UK_BT			1
#define CONFIG_RINGER_UK_CCA		1
#define CONFIG_RINGER_LINETEST		1
/**
	@}
*/

#endif // PROXAPICONFIGHDR
