/**
	@file
	@brief Your context definition for the API.
**/

#ifndef PROXCONTEXTHDR
#define PROXCONTEXTHDR

/**
	@addtogroup G05_dtd Driver/Call Context Types and Functions
	@{
*/

/**
	@brief This is a example of channel instance definition to store the data per channel.
	You can modify it according to your requirement.
*/
typedef struct tagProXChannel
{
	enum tagProXBATType bat_type; ///< Battery type of the channel.
	enum tagIMZRegion imz; ///< impedance settings of the channel.
	unsigned char last_ls; ///< latest LS status
	unsigned char chk_flash; ///< check hook flash action; work with pull counter, generally 10mS/pull, flash <= 600mS
}ProXChannel;

#ifndef CHIP_CHANNELS
#define CHIP_CHANNELS 1
#endif

/**
	@brief This is a example of chip instance definition.
	You can modify it according to your requirement.
*/
typedef struct tagProXChip
{
	int spi_addr; ///< Maybe you need this for SPI read/write. And for multiple application/thread read/write lock.
	ProXChannel channels[CHIP_CHANNELS(0, 0)]; ///< Stores the channel configuration.
	//unsigned long spi_inst; // for Myson
}ProXChip;

typedef struct tagProXChip * __chip_inst;	///< ProX chip instance (context)
// typedef struct void *__chip_inst; // for Digium WCTDMxxx
typedef int __chip_chnl;	///< ProX channel identification (context)

/**
	@}
*/

#endif // PROXCONTEXTHDR
