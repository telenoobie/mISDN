/**
	@file
	@brief ProX products API data type and function declareration.

	This file defines data types for API. For example, a chip instance, a channel.

	And declare API functions. Map to different functions according to chip type.

	<a href="http://www.nuvoton.com">www.nuvoton.com</a>

	@author qkdang@nuvoton.com
*/

#ifndef NXAPIHDR
#define NXAPIHDR

#include "api_config.h"

#ifndef CHIP_CHANNELS

#if defined(N681386)
#define CHIP_CHANNELS(argInst, argChnl) 1
#elif defined(N681387)
#define CHIP_CHANNELS(argInst, argChnl) 1
#elif defined(N682386)
#define CHIP_CHANNELS(argInst, argChnl) 2
#elif defined(N682387)
#define CHIP_CHANNELS(argInst, argChnl) 2
#elif defined(W684386)
#define CHIP_CHANNELS(argInst, argChnl) 4
#elif defined(W681388)
#define CHIP_CHANNELS(argInst, argChnl) 1
#else
#define CHIP_CHANNELS(argInst, argChnl) 1
#endif

#endif

/**
	@addtogroup G04_bdtd Basic Data Types
	@{
*/
#if defined(__LINUX__)
#include <linux/types.h>
#else
typedef unsigned long __u32;	///< Define the unsigned 32bit for your platform
typedef unsigned short __u16;	///< Define the unsigned 16bit for your platform
#endif
typedef signed long __i32;	///< Define the signed 32bit for your platform
typedef signed short __i16;	///< Define the signed 16bit for your platform
/**
	@}
*/

/**
	@addtogroup G04_bdtd1 About SPI and Device ID

	All models can be read write in 16bit-8bit SPI mode.

	N68X38X and W684386 registers can be read write in 8bit-8bit-8bit SPI mode.

	All device ID can read from register 0x07 on common channel (channel 0);

	Silicon ID is read from register 0x06 on common channel (channel 0);
	@{
*/
#define DVID_1386_88	0x01	///< Device ID of N681386 and N681388, use SLID_1386 to judge which it is.
#define SLID_1386		0xEF	///< Silicon ID of N681386. Use SLID_1386 to distinguish from W681388.
#define DVID_4386		0xC0	///< Device ID of W684386.
#define DVID_2386		0x41	///< Device ID of N682386.
#define DVID_1387		0x09	///< Device ID of N681387.
#define DVID_2387		0x49	///< Device ID of N682387.
/**
	@}
*/

// some enum-index will use this
typedef enum enumEnv
{
	enumFirstValue,
}EnumEnv;

/**
	@addtogroup G05_dtd Driver/Call Context Types and Functions

	The application/driver calls API functions with chip and channel context.
	The API functions will use these contexts to get the configuration of the channel of the chip.
	For example, battary type (inductor or transformer or external), Impendence, cirtical section, etc.
	The API functions will call the low level register read/write functions with these contexts, too.
	But the contexts are totally transparent to the API, all OS depended and context depended functions are written by API user according to their platform and requirements.
	These OS/Contexts depended functions are in "PLATFORMLIB/PROX_Plat_Dummy.C".
	@{
*/

/**
	@brief Battary Type
*/
typedef enum tagProXBATType
{
	BAT_INDUCTOR,	///< On-chip DC-DC with inductor
	BAT_TRANSFORMER,	///< On-chip DC-DC with transformer
	BAT_EXTERNAL,	///< External power supply, include ring voltage
	BAT_INDUCTOR_EXT_RING, ///< On-chip DC-DC with inductor, external ring voltage
	BAT_TRANSFORMER_EXT_RING, ///< On-chip DC-DC with transformer, external ring voltage
}ProXBATType;

/**
	@brief Region definitions for impedence match
*/
typedef enum tagIMZRegion
{
	IMZ_US_PBX = 0,		///< USA
	IMZ_Standard = 1,	///< Standard
#if CONFIG_IMZ_Japan_CO
	IMZ_Japan_CO,	///< Japan CO
#endif
#if CONFIG_IMZ_Bellcore
	IMZ_Bellcore,	///< Bellcore
#endif
#if CONFIG_IMZ_CTR21
	IMZ_CTR21,	///< CTR21
#endif
#if CONFIG_IMZ_China_CO
	IMZ_China_CO,	///< China CO
#endif
#if CONFIG_IMZ_China_PBX
	IMZ_China_PBX,	///< China PBX
#endif
#if CONFIG_IMZ_Japan_PBX
	IMZ_Japan_PBX,	///< Japan PBX
#endif
#if CONFIG_IMZ_India_NewZealand
	IMZ_India_NewZealand,	///< NewZealand
#endif
#if CONFIG_IMZ_Germany_Legacy
	IMZ_Germany_Legacy,	///< Germany Legacy
#endif
#if CONFIG_IMZ_UK_Legacy
	IMZ_UK_Legacy,	///< UK legacy
#endif
#if CONFIG_IMZ_Australia
	IMZ_Australia,	///< Australia
#endif
#if CONFIG_IMZ_Variation
	IMZ_Variation,	///< Variation
#endif
	IMZ_Max,	///< Placeholder for programming
}imzRegion;

#include "prox_context.h"

/**
	@}
*/

/// <!-- end of defination -->

/**
	@addtogroup G06_lvfuncs Platform Depended Functions
	@{
*/

#if 1 // platform depended functions

/**
	@brief Delay for miliseconds.
	@param valms The miliseconds to delay
*/
NXFUNSTR void nxDelayMS(int valms);

/**
	@brief Get current time in miliseconds. (for line test).
	@return The miliseconds
*/
NXFUNSTR __u32 nxGetTickCount(void);

#if USE_HARDWARE_INIT

/**
	@brief Prepare for chip initialization. Apply power, clock, set SPI to idle (SCLK High), CS off.
	@param pInst The instance of the chip (chip context)
*/
NXFUNSTR void nxHardwareInit(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Set reset pin of the chip to low.
	@param pInst The instance of the chip (chip context)
*/
NXFUNSTR void nxAssertReset(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Set reset pin of the chip to high.
	@param pInst The instance of the chip (chip context)
*/
NXFUNSTR void nxReleaseReset(__chip_inst pInst, __chip_chnl chnl);

#endif // USE_HARDWARE_INIT

/**
	@brief Query the common channel of the chip. (channel 0 of one chip).
	@param pInst The instance of the chip (chip context)
	@return The common channel.
*/
NXFUNSTR __chip_chnl nxGetCommonChnl(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief If you are using N681387/N682387 with wide band (16K SR) for this chip/channel, return 1, else 0.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval none-zero This channel uses wide band.
	@retval zero This channel doesn't use wide band.
*/
NXFUNSTR int nxIsWideBand(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Query whether this is the common channel of the chip. (channel 0 of one chip).
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval none-zero This is common channel.
	@retval zero This is not common channel.
*/
NXFUNSTR int nxIsCommonChnl(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Get the index of the channel of the chip (extract the channel index from the contexts).
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@return chnl the channel index from 0.
*/
NXFUNSTR int nxGetChnlIndex(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Save caliberation data.

	This function saves the caliberation data. When can't do caliberation (phone off-hooked),
	The code can load the old caliberation data instead of do it again.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param dat The data to store.
	@param len The data size to store.
*/
NXFUNSTR void nxSaveCalibData(__chip_inst pInst, __chip_chnl chnl, unsigned char *dat, int len);

/**
	@brief Load caliberation data.

	This function load the caliberation data saved before. When can't do caliberation (phone off-hooked),
	The code can load the old caliberation data instead of do it again.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param dat The data buffer.
	@param len The data size to load.
	@retval none-zero succeeded.
	@retval zero failed.
*/
NXFUNSTR int nxLoadCalibData(__chip_inst pInst, __chip_chnl chnl, unsigned char *dat, int len);

/**
	@brief Get battary type of the specified chip/channel.

	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@return The battary type.
*/
NXFUNSTR ProXBATType nxGetBatType(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief How many channels of the chip. If you consider the channels dynamictlly, you may use this.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@return channels of the chip
*/
NXFUNSTR int nxGetChipChannels(__chip_inst pInst, __chip_chnl chnl);

#endif // end of platform depended functions

/**
	@}
*/

/**
	@addtogroup G07_apifuncs API Functions
	@{
*/

/**
	@addtogroup G08_regreadw Registers Read/Write
	@{
*/

#if FORDOCUMENT // for document generation
/**
	@brief Read a register.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param regAddr The register address to read.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxReadReg(__chip_inst pInst, __chip_chnl chnl, int regAddr);

/**
	@brief Read unsigned short value of a reigster. Now only N68x386/7
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param regAddr The register address.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxReadShort(__chip_inst pInst, __chip_chnl chnl, int regAddr);

/**
	@brief Write one register
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param regAddr The register address.
	@param regVal The value to write.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxWriteReg(__chip_inst pInst, __chip_chnl chnl, int regAddr, int regVal);

/**
	@brief Write multiple registers
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param regStartAddr Start register address
	@param vals Values to write
	@param cnt Counts
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxWriteRegs(__chip_inst pInst, __chip_chnl chnl, int regStartAddr, const unsigned char *vals, int cnt);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@addtogroup G09_apifinitd Initialization
	@{
*/

#if FORDOCUMENT // for document generation

/**
	@brief Do hardware/chip reset
	@param pInst The instance of the chip (chip context)
	@retval >=0 succeeded.
	@retval <0 failed.
	@remarks
	The hardware reset procedure is:
	- 1. Assert reset pin. (pull low)
	- 2. Power supply on. (can be put before item 1)
	- 3. Apply BCLK, FS. If you use the device as PCM master, FS is not required.
	- 4. Set CS pin and SCLK pin to high.
	- 5. Release reset pin. (pull high)
	- 6. Wait for PLL lock.
*/
extern int nxHWReset(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Initialize all registers
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval >=0 succeeded.
	@retval <0 failed.
	@note Initialize channel 0 first because when initialize channel 0 the code will soft reset the chip.
*/
extern int nxInit(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Power on/off the battary.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param onoff Turn on/off
		@arg @c none-zero turn on
		@arg @c zero turn off
	@retval >=0 succeeded.
	@retval <0 failed.
	@remarks This will set on-chip DC-DC to on/off, or call external power control according to BAT type.
*/
extern int nxBATOn(__chip_inst pInst, __chip_chnl chnl, int onoff);

/**
	@brief Do caliberation.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval >=0 succeeded.
	@retval <0 failed.
	@remarks This will caliberate the external circuit of the chip. The phone must in on-hook status.
*/
extern int nxCaliberate(__chip_inst pInst, __chip_chnl chnl);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@addtogroup G10_apiringfd Ringing
	@{
	@image html ring_volt.png
	@image html ubring_volt.png
*/


/**
	@brief Region definitions for ring settings
*/
typedef enum tagRingRegion
{
#if CONFIG_RINGER_USA
	RING_USA,	///< USA 20Hz 70Vpk 2s 4s
#endif
#if CONFIG_RINGER_JAPAN
	RING_JAPAN,	///< JAPAN 16Hz 75Vpk 1s 2s
#endif
#if CONFIG_RINGER_KOREA
	RING_KOREA,	///< KOREA 20Hz 75Vpk 1s 2s
#endif
#if CONFIG_RINGER_CHINA
	RING_CHINA,	///< CHINA 25Hz 75Vpk 1s 4s
#endif
#if CONFIG_RINGER_FRANCE
	RING_FRANCE,	///< FRANCE 50Hz 80Vpk 1.5s 3.5s
#endif
#if CONFIG_RINGER_ITALY
	RING_ITALY,	///< ITALY 25Hz 60Vpk 1s 2s
#endif
#if CONFIG_RINGER_GERMANY
	RING_GERMANY,	///< GERMANY 25Hz 48Vpk 1s 4s
	RING_GERMANY_50,///< GERMANY 50Hz 48Vpk 1s 4s
	RING_GERMANY_HV,///< GERMANY 25Hz 80Vpk 1s 4s
#endif
#if CONFIG_RINGER_UK_BT
	RING_UK,   		///< UK 25Hz 70Vpk 1s 4s
#endif
#if CONFIG_RINGER_UK_CCA
	RING_UK_CCA,	///< UK 25Hz 75Vpk 1s 4s
#endif
#if CONFIG_RINGER_LINETEST
	RING_LINE_T,	///< For line test, 40Hz 24Vpk contiunus
#endif
}ringRegion;

#if FORDOCUMENT // for document generation

/**
	@brief Setup ring generator according to the region.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param xregion region settings.
	@retval >=0 succeeded.
	@retval <0 failed.
	@remarks This will config the ring generator according to the region. Includes voltage, freq, ring trip detection, etc.
	When line is set to ring and automatic ring enabled it ring voltage and waveform will generated.
*/
extern int nxSetupRingtone(__chip_inst pInst, __chip_chnl chnl, enum tagRingRegion xregion);

/**
	@brief Setup ring generator.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param freq Ringing frequency. e.g. 20, 25, 50, ...
	@param vpeak Peak voltage. e.g. 75
	@param msecOn Active miliseconds. range: 0~8000
	@param msecOff Inactive miliseconds. range: 0~8000
	@param dcv DC part value of the ring, VROFF. Generally 0.
	Range: W681388 is 0V to 47.488V; Others 0 to 94.976V.
	@param usecOffset microseconds of the phase offset. Generally 0.
	@retval >=0 succeeded.
	@retval <0 failed.
	@remarks
	If you want to keep ringing, set inactive time to 0, set active time to nonzero.
*/
extern int nxSetupRing(__chip_inst pInst, __chip_chnl chnl, int freq, __u32 vpeak, __u32 msecOn, __u32 msecOff,
					__u32 dcv, __u32 usecOffset);

/**
	@brief Set ring generator ring active time
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param msecOn Active miliseconds. range: 0~8000
	@retval >=0 succeeded.
	@retval <0 failed.
	@remarks
	If you want to keep ringing, set inactive time to 0, set active time to nonzero.
*/
extern int nxSetRingActiveTime(__chip_inst pInst, __chip_chnl chnl, __u32 msecOn);

/**
	@brief Set ring generator ring inactive time
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param msecOff Inactive miliseconds. range: 0~8000
	@retval >=0 succeeded.
	@retval <0 failed.
	@remarks
	If you want to keep ringing, set inactive time to 0, set active time to nonzero.
*/
extern int nxSetRingInactiveTime(__chip_inst pInst, __chip_chnl chnl, __u32 msecOff);

/**
	@brief Setup ring generator.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param freq Ringing frequency. e.g. 20, 25, 50, ...
	@param vpeak Peak voltage. e.g. 75
	@param msecOn Active miliseconds. range: 0~8000
	@param msecOff Inactive miliseconds. range: 0~8000
	@param dcv DC part value of the ring, VROFF. Generally 0.
	Range: W681388 is 0V to 47.488V; Others 0 to 94.976V.
	@param usecOffset microseconds of the phase offset. Generally 0.
	@param crest_factor100 The crest factor of the waveform multiplied with 100. e.g. for crest factor 1.3, the value is 130.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetupTrapRing(__chip_inst pInst, __chip_chnl chnl, int freq, __u32 vpeak, __u32 msecOn, __u32 msecOff,
					__u32 dcv, __u32 usecOffset, int crest_factor100);

/**
	@brief Setup ring waveform phase delay. 0~31800us
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param uSec How long to delay, in us. The range is 0 to 31800
	@param zcEna Whether to enable the zero-cross feature.
	@retval >=0 succeeded.
	@retval <0 failed.
	@remarks This will control the precise phase position where the ringing signal begins to transmit.
	if the zero-crossing feature is enabled the signal transmission will end at the equivalent phase position.
*/
extern int nxSetRingPhase(__chip_inst pInst, __chip_chnl chnl, int uSec, int zcEna);

/**
	@brief Set high battary voltage. The high battary voltage is for ringing.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param valmV The value of the voltage in mV (1V == 1000mV).
	@retval >=0 succeeded.
	@retval <0 failed.
	@remarks
	VBHV > 2*(Vringing-peak + Vringing-dc-offset + Vcmr)
*/
extern int nxSetVBHV(__chip_inst pInst, __chip_chnl chnl, __u32 valmV);

/**
	@brief Set common ringing bias voltage.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param valmV The value of the voltage in mV (1V == 1000mV).
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetVCMR(__chip_inst pInst, __chip_chnl chnl, __u32 valmV);

/**
	@brief Set DC offset of the ringing, VROFF.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param valmV The value of the voltage in mV (1V == 1000mV).
	Range: W681388 is 0V to 47.488V; Others 0 to 94.976V.
	@retval >=0 succeeded.
	@retval <0 failed.
	@remarks
	Same with "dcv" in function nxSetupRing(), nxSetupTrapRing().
*/
extern int nxSetVROFF(__chip_inst pInst, __chip_chnl chnl, __u32 valmV);

/**
	@brief Enable/Disable unbalanced ring.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param ena Enable/Disable unbalanced ring
		@arg @c none-zero Enable
		@arg @c zero Disable
	@param onTip Select which line has the waveform. (The other one tied to VGM). Not supported now.
		@arg @c none-zero The ringing voltage generated on TIP line. The RING line tied to VGM.
		@arg @c zero The ringing voltage generated on RING line. The TIP line tied to VGM.
	@retval >=0 succeeded.
	@retval <0 failed.
	@note
	W681388 doesn't support this feature. The "onTip" parameter is not supported now. Only ringing on RING line supported.
	
	Vgm + Vov + VROFF + Vpeak-ringing < VBATH
*/
extern int nxSetUnbalancedRing(__chip_inst pInst, __chip_chnl chnl, int ena, int onTip);

/**
	@brief Setup ring trip detect
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param currentLim Current threthold for ring trip detect. Generally 60~80mA
	@param freqLP Low pass filter frequency. Generally set this to ring frequency. e.g. 20, 25, 50, ...
	@param intena Enable ring trip interrupt, or not.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetupRingTripDetect(__chip_inst pInst, __chip_chnl chnl, int currentLim, int freqLP, int intena);

/**
	@brief Setup ring trip detect current threshold.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param currentLim Current threthold for ring trip detect. Generally 60~80mA
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetupRingTripCurrent(__chip_inst pInst, __chip_chnl chnl, int currentLim);

/**
	@brief Start/Stop ringing
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param en Start ringing, or stop.
	@param intena Enable ring active interrupt, or not.
	@param inteni Enable ring inactive interrupt, or not.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxRing(__chip_inst pInst, __chip_chnl chnl, int en, int intena, int inteni);

#endif // FORDOCUMENT // for document generation

/// <!-- end of group Ringing -->
/**
	@}
*/

/**
	@addtogroup G11_lsd Line Status
	@{
*/

/**
	@brief Line status
*/
typedef enum tagNXLINESTATUS
{
	LS_OPEN = 0,	///< TIP and RING are not drived, floated
	LS_FORWARD_ACTIVE,	///< Vtip > Vring, generally phone off-hooked
	LS_FORWARD_ONHOOKTRANS,	///< Vtip > Vring, generally phone on-hooked, ringing inactive
	LS_TIPOPEN,		///< TIP is not drived. VBAT drives RING
	LS_RINGING,		///< Ringing active
	LS_REVERSE_ACTIVE,	///< Vring > Vtip, generally phone off-hooked
	LS_REVERSE_ONHOOKTRANS,	///< Vring > Vtip, generally phone on-hooked, ringing inactive
	LS_RINGOPEN,	///< RING is not drived. VBAT drives TIP
	LS_FORWARD_IDLE = 9,	///< Vtip > Vring, generally phone on-hooked
	LS_REVERSE_IDLE = 13,	///< Vring > Vtip, generally phone off-hooked
	LS_CALIBRATION = 14,	///< for calibration
}NXLINESTATUS;

#if FORDOCUMENT // for document generation

/**
	@brief Get current line status
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval >=0 succeeded, the value of NXLINESTATUS.
	@retval <0 failed.
*/
extern int nxGetLineStatus(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Set current line status
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param st The line status want to set to.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetLineStatus(__chip_inst pInst, __chip_chnl chnl, NXLINESTATUS st);

/**
	@brief Set current line status, and make sure the result is right.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param st The line status want to set to.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetLineStatusTo(__chip_inst pInst, __chip_chnl chnl, NXLINESTATUS st);

/**
	@brief Check whether the lins polority is positive
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param st The line status want to set to.
	@retval >0 The line polority is positive. (Vtip > Vring)
	@retval ==0 The line polority is negitive. (Vring > Vtip)
	@retval <0 failed.
*/
extern int nxIsLinePositive(__chip_inst pInst, __chip_chnl chnl);

#endif // FORDOCUMENT // for document generation

/// <!-- end of groupt line status -->
/**
	@}
*/

/**
	@addtogroup G11_lsdr Ring Trip and Loop Closure status
	@{
*/

/**
	@brief Ring Trip and Loop Closure status
*/
typedef enum tagRTLCSTATUS
{
	RTLC_LCD = 0x01,	///< Loop closure detected
	RTLC_RTD = 0x02,	///< Ring trip detected
	RTLC_ULCD = 0x04,	///< Unfiltered (not debounced) Loop closure detected
	RTLC_ACURTD = 0x08,	///< Unfiltered (not debounced) AC path Ring trip detected
	RTLC_DCURTD = 0x10,	///< Unfiltered (not debounced) DC path Ring trip detected
}NXRTLCSTATUS;

#if FORDOCUMENT // for document generation

/**
	@brief Get current Ring Trip and Loop Closure status
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval >=0 succeeded, the value of combined NXRTLCSTATUS.
	@retval <0 failed.
*/
extern int nxGetRTLCStatus(__chip_inst pInst, __chip_chnl chnl);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@addtogroup G12_loptd Line/Voltage Parameters
	@{
*/
/**
	@addtogroup G13_lacoptd AC Parameters (include impendence settings)
	@{
*/

#if FORDOCUMENT // for document generation

/**
	@brief Set the line TX DAC Gain.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param valdB The gain value. (-41dB to +6dB in 1dB step). (-41dB to +6dB in 0.5dB step). If you use 0.5dB step (GAIN_STEP_0DOT5 defined as 1), the value is from -410 to +60 step 5 means -41.0dB to 6dB step 0.5.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetTxDACGain(__chip_inst pInst, __chip_chnl chnl, int valdB);

/**
	@brief Set the line RX ADC Gain.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param valdB The gain value. (-41dB to +6dB in 1dB step). (-41dB to +6dB in 0.5dB step). If you use 0.5dB step (GAIN_STEP_0DOT5 defined as 1), the value is from -410 to +60 step 5 means -41.0dB to 6dB step 0.5.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetRxADCGain(__chip_inst pInst, __chip_chnl chnl, int valdB);

#endif // FORDOCUMENT // for document generation

#if FORDOCUMENT // for document generation

#if MULTICHNL_SEPERATE_IMZ
/**
	@brief Get the IMZ region settings of the channel of the chip
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@return chnl the region settings. (can store in the context)
*/
NXFUNSTR enum tagIMZRegion nxGetIMZRegion(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Get next channel of the chip. For dual channels chip, return the other channel
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@return chnl the channel
*/
NXFUNSTR __chip_chnl nxGetNextChnl(__chip_inst pInst, __chip_chnl chnl);
#endif // MULTICHNL_SEPERATE_IMZ


/**
	@brief Set line impendence matching parameters according to the region.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param xregion The region.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetLineImpendence(__chip_inst pInst, __chip_chnl chnl, enum tagIMZRegion xregion);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@addtogroup G14_ldcoptd DC Parameters (Voltages, loop-closure, etc.)
	@{
	@image html dc_volt.png
*/

#if FORDOCUMENT // for document generation

/**
	@brief Set ground margin voltage. The most positive line voltage.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param valmV The value of ground margin voltage in mV (1V == 1000mV). The suggestion is 4500mV.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetVGM(__chip_inst pInst, __chip_chnl chnl, __u32 valmV);

/**
	@brief Set offset voltage. The most negative line voltage to VBAT (battary voltage).
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param valmV The value of the voltage offset in mV (1V == 1000mV). The recommanded value is 4500mV.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetVOV(__chip_inst pInst, __chip_chnl chnl, __u32 valmV);

/**
	@brief Set on-hook voltage. When phone is on-hook, the line is in idle status.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param valmV The value of the voltage in mV (1V == 1000mV). Generally it is 48000mV.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetVOH(__chip_inst pInst, __chip_chnl chnl, __u32 valmV);

/**
	@brief Set low battary value.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param valmV The value of the voltage in mV (1V == 1000mV). Generally it is 24000mV.
	@retval >=0 succeeded.
	@retval <0 failed.
	@remarks
	The VBLV (Low Battary Value), is used when the line goes into const current status (off-hook),
	and the voltage tracking feature is disabled, and on-chip DC-DC used.
*/
extern int nxSetVBLV(__chip_inst pInst, __chip_chnl chnl, __u32 valmV);

/**
	@brief Enable/Disable voltage tracking when line is in const current region. (off-hooked)
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param ena Enable/Disable
		@arg @c none-zero Enable
		@arg @c zero Disable
	@retval >=0 succeeded.
	@retval <0 failed.
	@remarks
	Enable voltage tracking to save power. Else get better performance. Only for on-chip DC-DC.
*/
extern int nxSetVoltageTrack(__chip_inst pInst, __chip_chnl chnl, int ena);

/**
	@brief Set loop-closure (off-hook) detection parameters
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param CurOrVolt Use current detect or voltage detect
		@arg @c none-zero Use current detect. This is recommanded.
		@arg @c zero Use voltage detect.
	@param valmAmV Use threthold value of current/voltage detection. For current detection, generally it is 15mA.
	@param debouncemS This is the debounce time for the detection. 10mS is recommanded.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetLoopClosureDetect(__chip_inst pInst, __chip_chnl chnl, int CurOrVolt, __i32 valmAmV, int debouncemS);

/**
	@brief Set loop current limit
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param valmA Limit current to valmA mA. (20mA~41mA).
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetMaxLoopCurrent(__chip_inst pInst, __chip_chnl chnl, int valmA);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@}
*/

/**
	@addtogroup G15_lotoptd Supervision
	@{
*/

#if FORDOCUMENT // for document generation

/**
	@brief Get temperature of the chip.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval >=0 succeeded, the temperature in degree Celsius
	@retval <0 failed.
*/
extern int nxGetTemperature(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Get BCLK in 1000Hz unit;
	@param pInst The instance of the chip (chip context)
	@retval >=0 succeeded, the temperature in degree Celsius
	@retval <0 failed.
*/
extern int nxGetBCLK(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Get BAT Voltage in mV.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval >=0 succeeded, the temperature in degree Celsius
	@retval <0 failed.
*/
extern __i32 nxGetBATVoltage(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Get Tip Voltage in mV.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval >=0 succeeded, the temperature in degree Celsius
	@retval <0 failed.
*/
extern __i32 nxGetTipVoltage(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Get Ring Voltage in mV.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval >=0 succeeded, the temperature in degree Celsius
	@retval <0 failed.
*/
extern __i32 nxGetRingVoltage(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Get Loop Voltage in mV.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval >=0 succeeded, the temperature in degree Celsius
	@retval <0 failed.
*/
extern __i32 nxGetLoopVoltage(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Get current of the calculated loop current in uA.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval >=0 succeeded, the temperature in degree Celsius
	@retval <0 failed.
*/
extern __i32 nxGetLoopCurrent(__chip_inst pInst, __chip_chnl chnl);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@addtogroup G16_pcmd PCM Interface
	@{
*/

/**
	@brief Operations for nxSetPCM()
*/
typedef enum tagPCMOpt
{
	PROX_PCM_ENABLE			= 0x001, ///< Enable PCM interface
	PROX_PCM_DISABLE		= 0x002, ///< Disable PCM interface
	PROX_PCM_ALAW			= 0x004, ///< PCM uses 8 bits A-Law
	PROX_PCM_ULAW			= 0x008, ///< PCM uses 8 bits u-Law
	PROX_PCM_LINEAR			= 0x010, ///< PCM uses 16 bits linear
	PROX_PCM_DATA1BCLK		= 0x020, ///< 1 data bit uses 1 bit clock
	PROX_PCM_DATA2BCLK		= 0x040, ///< 1 data bit uses 2 bit clock
}PCM_OPERATION;

#if FORDOCUMENT // for document generation

/**
	@brief Setup PCM interface
	
	e.g. nxSetPCM(pInst, chnl, PROX_PCM_ENABLE|PROX_PCM_ULAW|PROX_PCM_DATA1BCLK); and then if you want to change it to linear you can call nxSetPCM(pInst, chnl, PROX_PCM_LINEAR);
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param opts The operations/options want to apply. Can be combination of one or more PCM_OPERATION. e.g. nxSetPCM(0, 0, PROX_PCM_ENABLE|PROX_PCM_LINEAR|PROX_PCM_DATA1BCLK);
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetPCM(__chip_inst pInst, __chip_chnl chnl, int opts, int *regbuf);

/**
	@brief Set ProX Device Tx Time Slot.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param valBits The 0 based bits offset count from first valid bit of one frame.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetTxTimeSlot(__chip_inst pInst, __chip_chnl chnl, unsigned short valBits);

/**
	@brief Set ProX Device Rx Time Slot.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param valBits The 0 based bits offset count from first valid bit of one frame.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetRxTimeSlot(__chip_inst pInst, __chip_chnl chnl, unsigned short valBits);

#endif // FORDOCUMENT // for document generation

/**
	@brief Gains for nxSetRxAPG(), nxSetTxAPG(); analog path gain.
*/
typedef enum tagAPGValue
{
	APG_0_DB				= 0x000, ///< 0 dB
	APG_M3DOT5_DB			= 0x001, ///< -3.5 dB
	APG_3DOT5_DB			= 0x002, ///< 3.5 dB
	APG_MUTED				= 0x003, ///< mute
}APG_VALUE;

#if FORDOCUMENT // for document generation

/**
	@brief Set the RX analog path gain. You can use it to mute/unmute the RX path.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param val The gain to set to.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetRxAPG(__chip_inst pInst, __chip_chnl chnl, enum tagAPGValue val);

/**
	@brief Set the TX analog path gain. You can use it to mute/unmute the TX path.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param val The gain to set to.
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxSetTxAPG(__chip_inst pInst, __chip_chnl chnl, enum tagAPGValue val);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@addtogroup G17_intotoptd Interrupts
	@{
*/

/**
	@brief Interrupts
*/
typedef enum tagProxInt
{
	PROX_INT_LC			= 0x0001, ///< Loop Closure Interrupt
	PROX_INT_RT			= 0x0002, ///< Ringing Trip Interrupt
	PROX_INT_RA			= 0x0004, ///< Ringing Active Interrupt
	PROX_INT_RI			= 0x0008, ///< Ringing Inactive Interrupt
	PROX_INT_PA			= 0x0010, ///< Power Alarm Interrupt
	PROX_INT_TMP		= 0x0020, ///< Temperature Alarm Interrupt
	PROX_INT_GKD		= 0x0040, ///< Groundkey Detected Interrupt
	PROX_INT_T1A		= 0x0080, ///< Tone 1 Active Interrupt
	PROX_INT_T1I		= 0x0100, ///< Tone 1 Inactive Interrupt
	PROX_INT_T2A		= 0x0200, ///< Tone 2 Active Interrupt
	PROX_INT_T2I		= 0x0400, ///< Tone 2 Inactive Interrupt
	PROX_INT_DTMF		= 0x0800, ///< DTMF Decoder Interrupt
	PROX_INT_FSK		= 0x1000, ///< FSK Encoder Interrupt
	PROX_INT_MPA		= 0x2000, ///< Metering pulse Interrupt
	PROX_INT_MPI		= 0x4000, ///< Metering pulse Interrupt
}PROX_INTERRUPTS;

#if FORDOCUMENT // for document generation

/**
	@brief Get interrupts of the channel, and clear the interrupts
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval >=0 succeeded,
	@retval <0 failed.
*/
extern int nxGetInterrupts(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Enable interrupts of the channel
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval >=0 succeeded,
	@retval <0 failed.
*/
extern int nxIntEnable(__chip_inst pInst, __chip_chnl chnl, int ints);

/**
	@brief Disable interrupts of the channel
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval >=0 succeeded,
	@retval <0 failed.
*/
extern int nxIntDisable(__chip_inst pInst, __chip_chnl chnl, int ints);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/


/**
	@addtogroup G18_apitonesd Tones
	@{
*/

/**
	@addtogroup G19_tdtdtmf DTMF Generating
	@{
*/

#if FORDOCUMENT // for document generation

/**
	@brief Generate DTMF tone. This is a nonblock function that it will config the OSC1 and OSC2 and return immediatelly.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param xdigit The tone to generate: '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '0', '#', 'A', 'B', 'C', 'D'
	@param msActive The active time of the tone in miliseconds
	@param msInactive The inactive time of the tone in miliseconds
	@retval >=0 succeeded,
	@retval <0 failed.
*/
extern int nxDTMFGen(__chip_inst pInst, __chip_chnl chnl, unsigned char xdigit, unsigned int msActive, unsigned int msInactive, int db);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@addtogroup G20_tdtdfsk FSK Generater/Encoder
	@{
*/

/**
	@brief Operations for nxFSKGenSetup()
*/
typedef enum tagFSKGenOpt
{
	PROX_FSKG_ENABLE		= 0x0001, ///< Enable FSK generator
	PROX_FSKG_DISABLE		= 0x0002, ///< Disable FSK generator
	PROX_FSKG_ITUTV23		= 0x0004, ///< Use ITU-T V.23 specification: 1300Hz for Mark 2100 Hz for Space
	PROX_FSKG_BELL202		= 0x0008, ///< Use Bell 202 specification: 1200Hz for Mark 2200 Hz for Space
	PROX_FSKG_1STOPBIT		= 0x0010, ///< 1 stop bit (packed mode)
	PROX_FSKG_2STOPBIT		= 0x0020, ///< 2 stop bits (packed mode)
	PROX_FSKG_START			= 0x0040, ///< start transmit
	PROX_FSKG_STOP			= 0x0080, ///< stop transmit
	PROX_FSKG_PARITY_EVEN	= 0x0100, ///< even parity (packed mode)
	PROX_FSKG_PARITY_ODD	= 0x0200, ///< odd parity (packed mode)
	PROX_FSKG_PARITY_NONE	= 0x0400, ///< no parity bit (packed mode)
	PROX_FSKG_MODE_RAW		= 0x0800, ///< raw mode, the data is not packet, 8 bits of one byte will send directly.
	PROX_FSKG_MODE_PACKED	= 0x1000, ///< packed mode, the data is packed, the byte of the data will has start bit, stop bit and optional parity bit.
	PROX_FSKG_POL_NORMAL	= 0x2000, ///< notmal polority.
	PROX_FSKG_POL_INVERT	= 0x4000, ///< inverted polority.
}FSKGEN_OPTIONS;

#if FORDOCUMENT // for document generation

/**
	@brief FSK generator configration/operation.

	e.g. nxFSKGenSetup(pInst, chnl, PROX_FSKG_ENABLE|PROX_FSKG_BELL202|PROX_FSKG_MODE_RAW|PROX_FSKG_POL_NORMAL);
	After call nxFSKGenWrite() to write first bytes of data to the FIFO, then you can call nxFSKGenSetup(pInst, chnl, PROX_FSKG_START) to start transmit the data.
	Then you can call nxFSKGenBusy() to check whether the data was sent. You can use nxFSKGenSetup(pInst, chnl, PROX_FSKG_1STOPBIT|PROX_FSKG_PARITY_NONE|PROX_FSKG_MODE_PACKED) to change it to packed mode then.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param opts The combination of FSKGEN_OPTIONS.
	@retval >=0 succeeded,
	@retval <0 failed.
*/
extern int nxFSKGenSetup(__chip_inst pInst, __chip_chnl chnl, int opts);

/**
	@brief Set the gain level of the FSK encoder/generator.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param dBVal The attenuation value to max PCM level (+3.14 dBm0). It is not linear, the range is from -24dB to 0dB.
	@retval >=0 succeeded,
	@retval <0 failed.
*/
extern int nxFSKGenSetLevel(__chip_inst pInst, __chip_chnl chnl, int dBVal);

/**
	@brief Write FSK data to FSK encoder FIFO
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param dat The data to write.
	@param cnt The byte count of the data
	@retval >=0 succeeded, bytes written
	@retval <0 failed.
*/
extern int nxFSKGenWrite(__chip_inst pInst, __chip_chnl chnl, unsigned char *dat, int cnt);

/**
	@brief Get the FSK encoder busy status.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval >0 succeeded, busy, FSK buffer not empty
	@retval ==0 succeeded, not busy, FSK buffer is empty
	@retval <0 failed.
*/
extern int nxFSKGenIsBusy(__chip_inst pInst, __chip_chnl chnl);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@addtogroup G21_tdtdtmfd DTMF Decoder
	@{
*/

#if FORDOCUMENT // for document generation

/**
	@brief Set the input of the DTMF decoder. from ADC path or DAC(PCM) path.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param adcOrdac none-zero: ADC path, zero: DAC path
	@retval >=0 succeeded,
	@retval <0 failed.
*/
extern int nxDTMFDSetInput(__chip_inst pInst, __chip_chnl chnl, int adcOrdac);

/**
	@brief Set the acceptable frequency tolerance level.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param tolerancelevel value from 0 to 3, the deviation: 1.5% to 4.5% step 0.5%
	@retval >=0 succeeded,
	@retval <0 failed.
*/
extern int nxDTMFDSetFreqError(__chip_inst pInst, __chip_chnl chnl, int tolerancelevel);

/**
	@brief Set the decode speed level.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param speedlevel The speed level value from 0 to 15, more big more slow but more accuricy. 5 is recommended for a default value.
	@retval >=0 succeeded,
	@retval <0 failed.
*/
extern int nxDTMFDSetDecSpeed(__chip_inst pInst, __chip_chnl chnl, int speedlevel);

/**
	@brief Set the signal level of the detection threshold.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param dBVal The acceptable signal level:  (-41dB to +6dB in 1dB step). (-41dB to +6dB in 0.5dB step). If you use 0.5dB step (GAIN_STEP_0DOT5 defined as 1), the value is from -410 to +60 step 5 means -41.0dB to 6dB step 0.5.
	@retval >=0 succeeded,
	@retval <0 failed.
*/
extern int nxDTMFDSetSigLevel(__chip_inst pInst, __chip_chnl chnl, int dBVal);

/**
	@brief Set the time threshold of the decoder.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param msPreActive The time to start one char detection start from DTMF tone detected. 0~127 miliseconds.
	@param msActive The time to consider that one char detected from detection started. 0~127 miliseconds.
	@param msInactive The time of inactive time to consider as another individual DTMF tone. 0~127 miliseconds.
	@note The (msPreActive + msActive) is the minimal exist time of one valid DTMF tone. The msInactive is the minimal absend time to distinguish another individual DTMF tone.
	@retval >=0 succeeded,
	@retval <0 failed.
*/
extern int nxDTMFDSetTimes(__chip_inst pInst, __chip_chnl chnl, int msPreActive, int msActive, int msInactive);

/**
	@brief Get the DTMF decoder data
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval >0 succeeded, the hex value of the char: '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '0', '#', 'A', 'B', 'C', 'D'
	@retval ==0 succeeded, no char detected; For W681388 you have to enable and check DTMF interrupt to check whether DTMF detected.
	@retval <0 failed.
*/
extern int nxDTMFDGetData(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Enable/Disable DTMF decode
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param ena None zero: Enable; Zero: Disable
	@retval >=0 succeeded
	@retval <0 failed.
*/
extern int nxDTMFDEnable(__chip_inst pInst, __chip_chnl chnl, int ena);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@addtogroup G22_tdtdmpg Metering Pulse Generater
	@{
*/

/**
	@}
*/

/**
	@addtogroup G23_tdtd Dial Tone
	@{
*/
/**
	@brief Region definitions for dial tone settings
*/
typedef enum tagDialtoneRegion
{
#if CONFIG_TONE_USA
	DT_USA, ///< USA 440+350 @ -13dBm
#endif
#if CONFIG_TONE_JAPAN
	DT_JAPAN, ///< JAPAN 400 @ -13dBm
#endif
#if CONFIG_TONE_KOREA
	DT_KOREA, ///< KOREA 440+350 @ -13dBm
#endif
#if CONFIG_TONE_CHINA
	DT_CHINA, ///< CHINA 440+350 @ -13dBm
#endif
#if CONFIG_TONE_FRANCE
	DT_FRANCE, ///< FRANCE 440 @ -13dBm
#endif
#if CONFIG_TONE_ITALY
	DT_ITALY, ///< ITALY 425 @ -13dBm
#endif
#if CONFIG_TONE_GERMANY
	DT_GERMANY, ///< GERMANY 425 @ -13dBm
#endif
#if CONFIG_TONE_UK
	DT_UK, ///< UK 440+350 @ -13dBm
#endif
#if CONFIG_TONE_BELGUIM
	DT_BELGUIM, ///< BELGUIM 450 @ -13dBm
#endif
#if CONFIG_TONE_ISRAEL
	DT_ISRAEL, ///< ISRAEL 400 @ -13dBm
#endif
#if CONFIG_TONE_NETHERLANDS
	DT_NETHERLANDS, ///< NETHERLANDS 425 @ -13dBm
#endif
#if CONFIG_TONE_NORWAY
	DT_NORWAY, ///< NORWAY 425 @ -13dBm
#endif
#if CONFIG_TONE_SINGAPORE
	DT_SINGAPORE, ///< SINGAPORE 320 + 270 @ -13dBm
#endif
#if CONFIG_TONE_SWEDEN
	DT_SWEDEN, ///< SWEDEN 425 @ -13dBm
#endif
#if CONFIG_TONE_SWITZERLAND
	DT_SWITZERLAND, ///< SWITZERLAND 425 @ -13dBm
#endif
#if CONFIG_TONE_TAIWAN
	DT_TAIWAN, ///< TAIWAN 440+350 @ -13dBm
#endif
}dialtoneRegion;

#if FORDOCUMENT // for document generation

/**
	@brief Generate dial tone according to the region.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param xregion region settings. @see nxStopTones()
	@retval >=0 succeeded.
	@retval <0 failed.
	@remarks This will config the tone generators according to the region and start the tone generation.
	Use nxStopTones() to stop the tone generation.
*/
extern int nxGenDialTone(__chip_inst pInst, __chip_chnl chnl, enum tagDialtoneRegion xregion);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@addtogroup G24_trbtd Ring-Back Tone
	@{
*/

/**
	@brief Region definitions for ring-back tone settings
*/
typedef enum tagRingbacktoneRegion
{
#if CONFIG_TONE_USA
	RBT_USA,	///< USA 480+440 @ -19dBm
#endif
#if CONFIG_TONE_JAPAN
	RBT_JAPAN,	///< JAPAN 416+384 @ -19dBm
#endif
#if CONFIG_TONE_KOREA
	RBT_KOREA,	///< KOREA 480+440 @ -19dBm
#endif
#if CONFIG_TONE_CHINA
	RBT_CHINA,	///< CHINA 480+440 @ -19dBm
#endif
#if CONFIG_TONE_FRANCE
	RBT_FRANCE,	///< FRANCE 400 @ -19dBm
#endif
#if CONFIG_TONE_ITALY
	RBT_ITALY,	///< ITALY 425 @ 19dBm
#endif
#if CONFIG_TONE_GERMANY
	RBT_GERMANY, ///< GERMANY 425 @ 19dBm
#endif
#if CONFIG_TONE_UK
	RBT_UK,	///< UK 450+400@ -19dBm
#endif
#if CONFIG_TONE_BELGUIM
	RBT_BELGUIM, ///< BELGUIM 450 @ -19dBm
#endif
#if CONFIG_TONE_ISRAEL
	RBT_ISRAEL, ///< ISRAEL 450+400@ -19dBm
#endif
#if CONFIG_TONE_NETHERLANDS
	RBT_NETHERLANDS, ///< NETHERLANDS 425 @ -19dBm
#endif
#if CONFIG_TONE_NORWAY
	RBT_NORWAY, ///< NORWAY 425 @ -19dBm
#endif
#if CONFIG_TONE_SINGAPORE
	RBT_SINGAPORE, ///< SINGAPORE 400 @ -19dBm
#endif
#if CONFIG_TONE_SWEDEN
	RBT_SWEDEN,	///< SWEDEN 425 @ -19dBm
#endif
#if CONFIG_TONE_SWITZERLAND
	RBT_SWITZERLAND, ///< SWITZERLAND 425 @ -19dBm
#endif
#if CONFIG_TONE_TAIWAN
	RBT_TAIWAN, ///< TAIWAN 480+440 @ -19dBm
#endif
}ringbacktongRegion;

#if FORDOCUMENT // for document generation

/**
	@brief Generate ring back tone according to the region.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param xregion region settings. @see nxStopTones()
	@retval >=0 succeeded.
	@retval <0 failed.
	@remarks This will config the tone generators according to the region and start the tone generation.
	Use nxStopTones() to stop the tone generation.
*/
extern int nxGenRingbackTone(__chip_inst pInst, __chip_chnl chnl, enum tagRingbacktoneRegion xregion);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@addtogroup G25_tbtdd Busy Tone
	@{
*/

/**
	@brief Region definitions for busy tone settings
*/
typedef enum tagBusytoneRegion
{
#if CONFIG_TONE_USA
	BT_USA, ///< USA 440+620 @ -19dBm
#endif
#if CONFIG_TONE_JAPAN
	BT_JAPAN, ///< JAPAN 400 @ -19dBm
#endif
#if CONFIG_TONE_KOREA
	BT_KOREA, ///< KOREA 440+620 @ -19dBm
#endif
#if CONFIG_TONE_CHINA
	BT_CHINA, ///< CHINA 440+620 @ -19dBm
#endif
#if CONFIG_TONE_FRANCE
	BT_FRANCE, ///< FRANCE 440 @ -19dBm
#endif
#if CONFIG_TONE_ITALY
	BT_ITALY, ///< ITALY 425 @ -19dBm
#endif
#if CONFIG_TONE_GERMANY
	BT_GERMANY, ///< GERMANY 425 @ -19dBm
#endif
#if CONFIG_TONE_UK
	BT_UK, ///< UK 440 @ -19dBm
#endif
#if CONFIG_TONE_BELGUIM
	BT_BELGUIM, ///< BELGUIM 450 @ -19dBm
#endif
#if CONFIG_TONE_ISRAEL
	BT_ISRAEL, ///< ISRAEL 440 @ -19dBm
#endif
#if CONFIG_TONE_NETHERLANDS
	BT_NETHERLANDS, ///< NETHERLANDS 425 @ -19dBm
#endif
#if CONFIG_TONE_NORWAY
	BT_NORWAY, ///< NORWAY 425 @ -19dBm
#endif
#if CONFIG_TONE_SINGAPORE
	BT_SINGAPORE, ///< SINGAPORE 320 + 270 @ -19dBm
#endif
#if CONFIG_TONE_SWEDEN
	BT_SWEDEN, ///< SWEDEN 425 @ -13dBm
#endif
#if CONFIG_TONE_SWITZERLAND
	BT_SWITZERLAND, ///< SWITZERLAND 425 @ -19dBm
#endif
#if CONFIG_TONE_TAIWAN
	BT_TAIWAN, ///< TAIWAN 440+620 @ -19dBm
#endif
}busytoneRegion;

#if FORDOCUMENT // for document generation

/**
	@brief Generate busy tone according to the region.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param xregion region settings. @see nxStopTones()
	@retval >=0 succeeded.
	@retval <0 failed.
	@remarks This will config the tone generators according to the region and start the tone generation.
	Use nxStopTones() to stop the tone generation.
*/
extern int nxGenBusyTone(__chip_inst pInst, __chip_chnl chnl, enum tagBusytoneRegion xregion);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@addtogroup G26_tonessfd Shared Functions
	@{
*/

#if FORDOCUMENT // for document generation

/**
	@brief Stop tone generation.
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@retval >=0 succeeded.
	@retval <0 failed.
	@remarks This will stop the tone generation (OSC1 and OSC2 will be disabled).
*/
extern int nxStopTones(__chip_inst pInst, __chip_chnl chnl);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/// <!-- end of group tones -->
/**
	@}
*/

/**
	@addtogroup G27_linetestfunctions Line Test Functions
	@{
*/

/**
	@addtogroup G28_linetefeef Line Test Functions
	@{
*/

#if FORDOCUMENT // for document generation

/**
	@brief The open voltage test measures dc and ac current by taking a series of samples and averging them.
	The dc voltage is found first and is subtracted from the samples to find the AC current.
	@retval -1 system error
	@retval 0  passed
	@retval 1  not passed
*/
extern int nxOpenVoltageTest(__chip_inst pInst, __chip_chnl chnl,
					 __i32 *acmVloop, __i32 *dcmVloop,
					 __i32 *acmVring, __i32 *dcmVring,
					 __i32 *acmVtip, __i32 *dcmVtip,
					 int aForeignVolt);

/**
	@brief For REN test that the REN is lower than 3 REN.
	@retval -1 System error
	@retval 0 0.175~3REN
	@retval 1 < 0.175REN
	@retval 2 >= 3REN
*/
extern int nxLowRENTest(__chip_inst pInst, __chip_chnl chnl, __i32 *bellowDot175, __i32 *calibConst);

/**
	@brief For REN test that the REN is higher than 5 REN
	@retval -1 system error
	@retval 0  passed
	@retval 1  not passed
*/
extern int nxRENTest(__chip_inst pInst, __chip_chnl chnl, __i32 *above5REN, __i32 *bellowDot175,
					__i32 *calibConstHigh, __i32 *calibConstLow);

/**
	@brief Resistive Faults Test 
	@retval -1 System error.
	@retval 0 Passed
	@retval 1 not passed
*/
extern int nxResFaultTest(__chip_inst pInst, __chip_chnl chnl, __i32 *RTR, __i32 *RTG, __i32 *RRG);

/**
	@brief off-hook test
	@retval -1 System error
	@retval 0  passed
	@retval 1  off-hook, not passed
	@retval 2  resistance fault
*/
extern int nxOffhookTest(__chip_inst pInst, __chip_chnl chnl);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@addtogroup G29_linetestconstcalib Const values acquire helper
	@{
*/

#if FORDOCUMENT // for document generation

/**
	@brief Get the value for further calculation with nxCalResConsts
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@return The value.
*/
extern __i32 nxGetVtToVr(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Calculate the consts for resistance test.
	@details After the board was initialized, remove the phone, call nxGetVtToVr to get
	the parameter VtToVrOpen, then connect a resistor whose value is RxKOhms KOhms to
	Tip and Ring, then call nxGetVtToVr again to get the parameter VtToVrRx, finally,
	Use these parameters to call nxCalResConsts to get two consts: CONST1, CONST2
	@param VtToVrOpen return value of nxGetVtToVr when nothing connected to the line.
	@param VtToVrRx return value of nxGetVtToVr when the resistor connected.
	@param RxKOhms the resistor value in KOhms.
	@param CONST1 to store the calculated constant 1.
	@param CONST2 to store the calculated constant 2.
	@retval >=0 succeeded
	@retval <0 failed
*/
extern int nxCalResConsts(__i32 VtToVrOpen, __i32 VtToVrRx, __i32 RxKOhms, __i32 *CONST1, __i32 *CONST2);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@}
*/

/**
	@addtogroup G30_diagnosticfunctions Diagnostic
	@{
*/

#if FORDOCUMENT // for document generation

/**
	@brief Enable/Disable the first level of the loop of digital interface (PCM/I2S).
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param ena Enable/Disable
	@return >=0: succeeded. <0: Failed
*/
extern int nxDiagDLoop0(__chip_inst pInst, __chip_chnl chnl, int ena);

/**
	@brief Enable/Disable the second level of the loop of digital interface (PCM/I2S).
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param ena Enable/Disable
	@return >=0: succeeded. <0: Failed
*/
extern int nxDiagDLoop1(__chip_inst pInst, __chip_chnl chnl, int ena);

/**
	@brief Enable/Disable the third level of the loop of digital interface (PCM/I2S).
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param ena Enable/Disable
	@return >=0: succeeded. <0: Failed
*/
extern int nxDiagDLoop2(__chip_inst pInst, __chip_chnl chnl, int ena);

/**
	@brief Enable/Disable the first level of the loop of analog interface (RJ11).
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param ena Enable/Disable
	@return >=0: succeeded. <0: Failed
*/
extern int nxDiagALoop0(__chip_inst pInst, __chip_chnl chnl, int ena);

/**
	@brief Enable/Disable the second level of the loop of analog interface (RJ11).
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param ena Enable/Disable
	@return >=0: succeeded. <0: Failed
*/
extern int nxDiagALoop1(__chip_inst pInst, __chip_chnl chnl, int ena);

#endif // FORDOCUMENT // for document generation

/**
	@brief Operations for nxDiagXConfig()
*/
typedef enum tagDIAGXOpt
{
	DIAGX_ENABLE		= 0x0001, ///< Enable Extended Diagnostic Function
	DIAGX_DISABLE		= 0x0002, ///< Disable Extended Diagnostic Function
	DIAGX_FIFO_ENA		= 0x0004, ///< Enable Diagnostic FIFO
	DIAGX_FIFO_DIS		= 0x0008, ///< Disable Diagnostic FIFO
	DIAGX_FIFOSRC_ADC	= 0x0010, ///< Route ADC data to FIFO
	DIAGX_FIFOSRC_SEL	= 0x0020, ///< Route Diagnostic data to FIFO
	DIAGX_ACLPF_ENA		= 0x0040, ///< AC Path low pass filter enable
	DIAGX_ACLPF_DIS		= 0x0080, ///< AC Path low pass filter disable
	DIAGX_ACHPF_ENA		= 0x0100, ///< AC Path high pass filter enable, need signed data source
	DIAGX_ACHPF_DIS		= 0x0200, ///< AC Path high pass filter disable, need signed data source
	DIAGX_DCLPF_ENA		= 0x0400, ///< DC Path low pass filter enable
	DIAGX_DCLPF_DIS		= 0x0800, ///< DC Path low pass filter disable
	DIAGX_DATA_SIGNED	= 0x1000, ///< Convert source data to signed
	DIAGX_DATA_USIGNED	= 0x2000, ///< Keep source data as unsigned
}DIAGX_OPTIONS;

/**
	@brief Operations for nxDiagXSetACSrc(), nxDiagXSetDCSrc() and nxDiagXTMSetSrc()
*/
typedef enum tagDIAGXSrc
{
	DIAGX_SRC_TIPV		= 0x00, ///< Tip Voltage
	DIAGX_SRC_RINGV		= 0x01, ///< Ring Voltage
	DIAGX_SRC_LPV		= 0x02, ///< Loop Voltage
	DIAGX_SRC_SCM		= 0x03, ///< SCM Value
	DIAGX_SRC_TIPI		= 0x04, ///< Tip Current
	DIAGX_SRC_RINGI		= 0x05, ///< Ring Current
	DIAGX_SRC_LPI		= 0x06, ///< Loop Current
	DIAGX_SRC_LGI		= 0x07, ///< Longitude Current
}DIAGX_SRC;

#if FORDOCUMENT // for document generation

/**
	@brief Setup extended diagnostic function
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param opts The operations. Can be combinations of DIAGX_OPTIONS;
	@return >=0: succeeded. <0: Failed.
*/
extern int nxDiagXConfig(__chip_inst pInst, __chip_chnl chnl, __u16 opts);

/**
	@brief Setup extended diagnostic AC path data source
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param src The source;
	@return >=0: succeeded. <0: Failed.
*/
extern int nxDiagXSetACSrc(__chip_inst pInst, __chip_chnl chnl, enum tagDIAGXSrc src);

/**
	@brief Setup extended diagnostic DC path data source
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param src The source;
	@return >=0: succeeded. <0: Failed.
*/
extern int nxDiagXSetDCSrc(__chip_inst pInst, __chip_chnl chnl, enum tagDIAGXSrc src);

/**
	@brief Setup extended diagnostic data source for time measurement
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param src The source;
	@return >=0: succeeded. <0: Failed.
*/
extern int nxDiagXTMSetSrc(__chip_inst pInst, __chip_chnl chnl, enum tagDIAGXSrc src);

/**
	@brief Set the high threshold for time measurement
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param val The value;
	@return >=0: succeeded. <0: Failed.
*/
extern int nxDiagXTMSetHighValue(__chip_inst pInst, __chip_chnl chnl, __u16 val);

/**
	@brief Set the low threshold for time measurement
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param val The value;
	@return >=0: succeeded. <0: Failed.
*/
extern int nxDiagXTMSetLowValue(__chip_inst pInst, __chip_chnl chnl, __u16 val);

/**
	@brief Start(enable)/Stop(disable) the time measurement
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@param ena Enable/Disable
	@return >=0: succeeded. <0: Failed.
*/
extern int nxDiagXTMEnable(__chip_inst pInst, __chip_chnl chnl, int ena);

/**
	@brief Get the time measurement result
	@param pInst The instance of the chip (chip context)
	@param chnl The channel of the chip to operate (channel context)
	@return >=0: The value in miliseconds. <0: Failed.
*/
extern int nxDiagXTMGetValue(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Check whether the FIFO 0 is ready
	@param pInst The instance of the chip (chip context)
	@return none 0: Ready. 0: Not ready or Failed.
*/
extern int nxDiagXIsFifo0Done(__chip_inst pInst, __chip_chnl chnl);

/**
	@brief Check whether the FIFO 1 is ready
	@param pInst The instance of the chip (chip context)
	@return none 0: Ready. 0: Not ready or Failed.
*/
extern int nxDiagXIsFifo1Done(__chip_inst pInst, __chip_chnl chnl);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@addtogroup G30_otherapifunctions Others
	@{
*/

/**
	@brief The ProX Chip ID definitions
*/
enum tagProXChipID
{
	PROX_CHIP_W681388,
	PROX_CHIP_W684386,
	PROX_CHIP_N681386,
	PROX_CHIP_N681387,
	PROX_CHIP_N682386,
	PROX_CHIP_N682387,
};

#if FORDOCUMENT // for document generation

/**
	@brief Get ProX chip type. The implementation is in low level SPI read/write layer. xxxRW.c
	@param pInst The instance of the chip (chip context)
	@retval >=0 succeeded.
	@retval <0 failed.
*/
extern int nxGetChipID(__chip_inst pInst, __chip_chnl chnl);

#endif // FORDOCUMENT // for document generation

/**
	@}
*/

/**
	@}
*/

/// <!--  end of group api functions -->

#if (defined(N681386) || defined(N682386) || defined(N681387) || defined(N682387))
#include "n68x386api.h"
#define nxHWReset				N68X386HWReset
#define nxInit					N68X386Init
#define nxBATOn					N68X386BATOn
#define nxCaliberate			N68X386Caliberate
#define nxSetupRingtone			N68X386SetupRingtone
#define nxSetupRing				N68X386SetupRing
#define nxSetRingActiveTime		N68X386SetRingActiveTime
#define nxSetRingInactiveTime	N68X386SetRingInactiveTime
#define nxSetupTrapRing			N68X386SetupTrapRing
#define nxSetRingPhase			N68X386SetRingPhase
#define nxSetVBHV				N68X386SetVBHV
#define nxSetVCMR				N68X386SetVCMR
#define nxSetVROFF				N68X386SetVROFF
#define nxSetUnbalancedRing		N68X386SetUnbalancedRing
#define nxSetupRingTripDetect	N68X386SetupRingTripDetect
#define nxSetupRingTripCurrent	N68X386SetupRingTripCurrent
#define nxRing					N68X386Ring
#define nxGetLineStatus			N68X386GetLineStatus
#define nxSetLineStatus			N68X386SetLineStatus
#define nxSetLineStatusTo		N68X386SetLineStatusTo
#define nxIsLinePositive		N68X386IsLinePositive
#define nxGetRTLCStatus			N68X386GetRTLCStatus
#define nxSetTxDACGain			N68X386SetTxDACGain
#define nxSetRxADCGain			N68X386SetRxADCGain
#define nxSetLineImpendence		N68X386SetLineImpendence
#define nxSetVGM				N68X386SetVGM
#define nxSetVOV				N68X386SetVOV
#define nxSetVOH				N68X386SetVOH
#define nxSetVBLV				N68X386SetVBLV
#define nxSetVoltageTrack		N68X386SetVoltageTrack
#define nxSetLoopClosureDetect	N68X386SetLoopClosureDetect
#define nxSetMaxLoopCurrent		N68X386SetMaxLoopCurrent
#define nxGetTemperature		N68X386GetTemperature
#define nxGetBCLK				N68X386GetBCLK
#define nxGetBATVoltage			N68X386GetBATVoltage
#define nxGetTipVoltage			N68X386GetTipVoltage
#define nxGetRingVoltage		N68X386GetRingVoltage
#define nxGetLoopVoltage		N68X386GetLoopVoltage
#define nxGetLoopCurrent		N68X386GetLoopCurrent
#define nxSetPCM				N68X386SetPCM
#define nxSetTxTimeSlot			N68X386SetTxTimeSlot
#define nxSetRxTimeSlot			N68X386SetRxTimeSlot
#define nxSetRxAPG				N68X386SetRxAPG
#define nxSetTxAPG				N68X386SetTxAPG
#define nxGetInterrupts			N68X386GetInterrupts
#define nxIntEnable				N68X386IntEnable
#define nxIntDisable			N68X386IntDisable
#define nxGenDialTone			N68X386GenDialTone
#define nxGenRingbackTone		N68X386GenRingbackTone
#define nxGenBusyTone			N68X386GenBusyTone
#define nxStopTones				N68X386StopTones
#define nxDiagDLoop0			N68X386DiagDLoop0
#define nxDiagDLoop1			N68X386DiagDLoop1
#define nxDiagDLoop2			N68X386DiagDLoop2
#define nxDiagALoop0			N68X386DiagALoop0
#define nxDiagALoop1			N68X386DiagALoop1
#define nxDiagXConfig			N68X386DiagXConfig
#define nxDiagXSetACSrc			N68X386DiagXSetACSrc
#define nxDiagXSetDCSrc			N68X386DiagXSetDCSrc
#define nxDiagXTMSetSrc			N68X386DiagXTMSetSrc
#define nxDiagXTMSetHighValue	N68X386DiagXTMSetHighThreshold
#define nxDiagXTMSetLowValue	N68X386DiagXTMSetLowThreshold
#define nxDiagXTMEnable			N68X386DiagXTMEnable
#define nxDiagXTMGetValue		N68X386DiagXTMGetValue
#define nxDiagXIsFifo0Done		N68X386DiagXIsFifo0Done
#define nxDiagXIsFifo1Done		N68X386DiagXIsFifo1Done
#define nxOpenVoltageTest		N68X386OpenVoltageTest
#define nxLowRENTest			N68X386LowRENTest
#define nxRENTest				N68X386RENTest
#define nxResFaultTest			N68X386ResFaultTest
#define nxOffhookTest			N68X386OffhookTest
#define nxGetVtToVr				N68X386GetVtToVr
#define nxCalResConsts			N68X386CalResConsts
#define nxDTMFGen				N68X386DTMFGen
#define nxToneGen				N68X386ToneGen
#define nxFSKGenSetup			N68X386FSKGenSetup
#define nxFSKGenSetLevel		N68X386FSKGenSetLevel
#define nxFSKGenWrite			N68X386FSKGenWrite
#define nxFSKGenIsBusy			N68X386FSKGenIsBusy
#define nxDTMFDSetInput			N68X386DTMFDSetInput
#define nxDTMFDSetFreqError		N68X386DTMFDSetFreqError
#define nxDTMFDSetDecSpeed		N68X386DTMFDSetDecSpeed
#define nxDTMFDSetSigLevel		N68X386DTMFDSetSigLevel
#define nxDTMFDSetTimes			N68X386DTMFDSetTimes
#define nxDTMFDGetData			N68X386DTMFDGetData
#define nxDTMFDEnable			N68X386DTMFDEnable
#include "n68x386rw.h"
#define nxReadReg				n68x386ReadReg
#define nxReadShort				n68x386ReadShort
#define nxWriteReg				n68x386WriteReg
#define nxWriteRegs				n68x386WriteRegs
#endif // (defined(N681386) || defined(N682386) || defined(N681387) || defined(N682387))
#if defined(W681388)
#include "W681388/W681388API.H"
#define nxHWReset				W681388HWReset
#define nxInit					W681388Init
#define nxBATOn					W681388BATOn
#define nxCaliberate			W681388Caliberate
#define nxSetupRingtone			W681388SetupRingtone
#define nxSetupRing				W681388SetupRing
#define nxSetRingActiveTime		W681388SetRingActiveTime
#define nxSetRingInactiveTime	W681388SetRingInactiveTime
#define nxSetupTrapRing			W681388SetupTrapRing
#define nxSetRingPhase			W681388SetRingPhase
#define nxSetVBHV				W681388SetVBHV
#define nxSetVCMR				W681388SetVCMR
#define nxSetVROFF				W681388SetVROFF
#define nxSetUnbalancedRing		W681388SetUnbalancedRing
#define nxSetupRingTripDetect	W681388SetupRingTripDetect
#define nxSetupRingTripCurrent	W681388SetupRingTripCurrent
#define nxRing					W681388Ring
#define nxGetLineStatus			W681388GetLineStatus
#define nxSetLineStatus			W681388SetLineStatus
#define nxSetLineStatusTo		W681388SetLineStatusTo
#define nxIsLinePositive		W681388IsLinePositive
#define nxGetRTLCStatus			W681388GetRTLCStatus
#define nxSetTxDACGain			W681388SetTxDACGain
#define nxSetRxADCGain			W681388SetRxADCGain
#define nxSetLineImpendence		W681388SetLineImpendence
#define nxSetVGM				W681388SetVGM
#define nxSetVOV				W681388SetVOV
#define nxSetVOH				W681388SetVOH
#define nxSetVBLV				W681388SetVBLV
#define nxSetVoltageTrack		W681388SetVoltageTrack
#define nxSetLoopClosureDetect	W681388SetLoopClosureDetect
#define nxSetMaxLoopCurrent		W681388SetMaxLoopCurrent
#define nxGetTemperature		W681388GetTemperature
#define nxGetBCLK				W681388GetBCLK
#define nxGetBATVoltage			W681388GetBATVoltage
#define nxGetTipVoltage			W681388GetTipVoltage
#define nxGetRingVoltage		W681388GetRingVoltage
#define nxGetLoopVoltage		W681388GetLoopVoltage
#define nxGetLoopCurrent		W681388GetLoopCurrent
#define nxSetPCM				W681388SetPCM
#define nxSetTxTimeSlot			W681388SetTxTimeSlot
#define nxSetRxTimeSlot			W681388SetRxTimeSlot
#define nxSetRxAPG				W681388SetRxAPG
#define nxSetTxAPG				W681388SetTxAPG
#define nxGetInterrupts			W681388GetInterrupts
#define nxIntEnable				W681388IntEnable
#define nxIntDisable			W681388IntDisable
#define nxGenDialTone			W681388GenDialTone
#define nxGenRingbackTone		W681388GenRingbackTone
#define nxGenBusyTone			W681388GenBusyTone
#define nxStopTones				W681388StopTones
#define nxDiagDLoop0			W681388DiagDLoop0
#define nxDiagDLoop1			W681388DiagDLoop1
#define nxDiagDLoop2			W681388DiagDLoop2
#define nxDiagALoop0			W681388DiagALoop0
#define nxDiagALoop1			W681388DiagALoop1
#define nxDiagXConfig			W681388DiagXConfig
#define nxDiagXSetACSrc			W681388DiagXSetACSrc
#define nxDiagXSetDCSrc			W681388DiagXSetDCSrc
#define nxDiagXTMSetSrc			W681388DiagXTMSetSrc
#define nxDiagXTMSetHighValue	W681388DiagXTMSetHighValue
#define nxDiagXTMSetLowValue	W681388DiagXTMSetLowValue
#define nxDiagXTMEnable			W681388DiagXTMEnable
#define nxDiagXTMGetValue		W681388DiagXTMGetValue
#define nxDiagXIsFifo0Done		W681388DiagXIsFifo0Done
#define nxDiagXIsFifo1Done		W681388DiagXIsFifo1Done
#define nxOpenVoltageTest		W681388OpenVoltageTest
#define nxLowRENTest			W681388LowRENTest
#define nxRENTest				W681388RENTest
#define nxResFaultTest			W681388ResFaultTest
#define nxOffhookTest			W681388OffhookTest
#define nxGetVtToVr				W681388GetVtToVr
#define nxCalResConsts			W681388CalResConsts
#define nxDTMFGen				W681388DTMFGen
#define nxFSKGenSetup			W681388FSKGenSetup
#define nxFSKGenSetLevel		W681388FSKGenSetLevel
#define nxFSKGenWrite			W681388FSKGenWrite
#define nxFSKGenIsBusy			W681388FSKGenIsBusy
#define nxDTMFDSetInput			W681388DTMFDSetInput
#define nxDTMFDSetFreqError		W681388DTMFDSetFreqError
#define nxDTMFDSetDecSpeed		W681388DTMFDSetDecSpeed
#define nxDTMFDSetSigLevel		W681388DTMFDSetSigLevel
#define nxDTMFDSetTimes			W681388DTMFDSetTimes
#define nxDTMFDGetData			W681388DTMFDGetData
#define nxDTMFDEnable			W681388DTMFDEnable
#include "PLATFORMLIB/W681388RW.H"
#define nxReadReg				w1388ReadReg
#define nxReadShort				w1388ReadReg
#define nxWriteReg				w1388WriteReg
#define nxWriteRegs				w1388WriteRegs
#endif // defined(W681388)
#if defined(W684386)
#include "W684386/W684386API.H"
#define nxHWReset				W684386HWReset
#define nxInit					W684386Init
#define nxBATOn					W684386BATOn
#define nxCaliberate			W684386Caliberate
#define nxSetupRingtone			W684386SetupRingtone
#define nxSetupRing				W684386SetupRing
#define nxSetRingActiveTime		W684386SetRingActiveTime
#define nxSetRingInactiveTime	W684386SetRingInactiveTime
#define nxSetupTrapRing			W684386SetupTrapRing
#define nxSetRingPhase			W684386SetRingPhase
#define nxSetVBHV				W684386SetVBHV
#define nxSetVCMR				W684386SetVCMR
#define nxSetVROFF				W684386SetVROFF
#define nxSetUnbalancedRing		W684386SetUnbalancedRing
#define nxSetupRingTripDetect	W684386SetupRingTripDetect
#define nxSetupRingTripCurrent	W684386SetupRingTripCurrent
#define nxRing					W684386Ring
#define nxGetLineStatus			W684386GetLineStatus
#define nxSetLineStatus			W684386SetLineStatus
#define nxSetLineStatusTo		W684386SetLineStatusTo
#define nxIsLinePositive		W684386IsLinePositive
#define nxGetRTLCStatus			W684386GetRTLCStatus
#define nxSetTxDACGain			W684386SetTxDACGain
#define nxSetRxADCGain			W684386SetRxADCGain
#define nxSetLineImpendence		W684386SetLineImpendence
#define nxSetVGM				W684386SetVGM
#define nxSetVOV				W684386SetVOV
#define nxSetVOH				W684386SetVOH
#define nxSetVBLV				W684386SetVBLV
#define nxSetVoltageTrack		W684386SetVoltageTrack
#define nxSetLoopClosureDetect	W684386SetLoopClosureDetect
#define nxSetMaxLoopCurrent		W684386SetMaxLoopCurrent
#define nxGetTemperature		W684386GetTemperature
#define nxGetBCLK				W684386GetBCLK
#define nxGetBATVoltage			W684386GetBATVoltage
#define nxGetTipVoltage			W684386GetTipVoltage
#define nxGetRingVoltage		W684386GetRingVoltage
#define nxGetLoopVoltage		W684386GetLoopVoltage
#define nxGetLoopCurrent		W684386GetLoopCurrent
#define nxSetPCM				W684386SetPCM
#define nxSetTxTimeSlot			W684386SetTxTimeSlot
#define nxSetRxTimeSlot			W684386SetRxTimeSlot
#define nxSetRxAPG				W684386SetRxAPG
#define nxSetTxAPG				W684386SetTxAPG
#define nxGetInterrupts			W684386GetInterrupts
#define nxIntEnable				W684386IntEnable
#define nxIntDisable			W684386IntDisable
#define nxGenDialTone			W684386GenDialTone
#define nxGenRingbackTone		W684386GenRingbackTone
#define nxGenBusyTone			W684386GenBusyTone
#define nxStopTones				W684386StopTones
#define nxDiagDLoop0			W684386DiagDLoop0
#define nxDiagDLoop1			W684386DiagDLoop1
#define nxDiagDLoop2			W684386DiagDLoop2
#define nxDiagALoop0			W684386DiagALoop0
#define nxDiagALoop1			W684386DiagALoop1
#define nxDiagXConfig			W684386DiagXConfig
#define nxDiagXSetACSrc			W684386DiagXSetACSrc
#define nxDiagXSetDCSrc			W684386DiagXSetDCSrc
#define nxDiagXTMSetSrc			W684386DiagXTMSetSrc
#define nxDiagXTMSetHighValue	W684386DiagXTMSetHighValue
#define nxDiagXTMSetLowValue	W684386DiagXTMSetLowValue
#define nxDiagXTMEnable			W684386DiagXTMEnable
#define nxDiagXTMGetValue		W684386DiagXTMGetValue
#define nxDiagXIsFifo0Done		W684386DiagXIsFifo0Done
#define nxDiagXIsFifo1Done		W684386DiagXIsFifo1Done
#define nxOpenVoltageTest		W684386OpenVoltageTest
#define nxLowRENTest			W684386LowRENTest
#define nxRENTest				W684386RENTest
#define nxResFaultTest			W684386ResFaultTest
#define nxOffhookTest			W684386OffhookTest
#define nxGetVtToVr				W684386GetVtToVr
#define nxCalResConsts			W684386CalResConsts
#define nxDTMFGen				W684386DTMFGen
#define nxFSKGenSetup			W684386FSKGenSetup
#define nxFSKGenSetLevel		W684386FSKGenSetLevel
#define nxFSKGenWrite			W684386FSKGenWrite
#define nxFSKGenIsBusy			W684386FSKGenIsBusy
#define nxDTMFDSetInput			W684386DTMFDSetInput
#define nxDTMFDSetFreqError		W684386DTMFDSetFreqError
#define nxDTMFDSetDecSpeed		W684386DTMFDSetDecSpeed
#define nxDTMFDSetSigLevel		W684386DTMFDSetSigLevel
#define nxDTMFDSetTimes			W684386DTMFDSetTimes
#define nxDTMFDGetData			W684386DTMFDGetData
#define nxDTMFDEnable			W684386DTMFDEnable
#include "PLATFORMLIB/W684386RW.H"
#define nxReadReg				W684386ReadReg
#define nxReadShort				W684386ReadReg
#define nxWriteReg				W684386WriteReg
#define nxWriteRegs				W684386WriteRegs
#endif // defined(W684386)

#endif // NXAPIHDR
