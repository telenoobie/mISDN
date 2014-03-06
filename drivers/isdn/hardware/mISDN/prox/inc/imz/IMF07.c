// Japan PBX v.2
const unsigned char vIMF07[] =
{
	0x0F, /* value of reg 0x41, hb/gain, for US PBX, Korea, Taiwan and Standard line it should be 0x5B, others 0x0F */
	0x00, /* value of reg 0xA8, IM1, for Standard, it is 0x01, others 0x00 */
	0x40, /* value of reg 0xF4 */
	0x00, /* value of reg 0xFB, value 0xFF means don't set */
	0x00, /* value of reg 0xDD, Low Byte of ADC Gain */
	0x00, /* value of reg 0xDE, Low Byte of DAC Gain */
	0x44, /* value of reg 0xDF, High Half Bytes of DAC and ADC Gain */
	0x00, /* 144: High Byte count of values to write to FIFO 0xF3 */
	0x90, /* 144: Low Byte count of values to write to FIFO 0xF3 */
	/* { 0,    1,    2,    3,    4,    5,    6,    7 */
	  0x66, 0xA1, 0x03, 0xDB, 0x95, 0x04, 0x2D, 0x5B, /*   1-  8 */
	  0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA8, /*   9- 16 */
	  0x00, 0x00, 0x51, 0x01, 0x00, 0xB4, 0xD0, 0x06, /*  17- 24 */
	  0xA8, 0x00, 0x00, 0x38, 0x5E, 0x00, 0x00, 0x00, /*  25- 32 */
	  0x01, 0xFF, 0xFF, 0x01, 0xD0, 0x8B, 0x06, 0xFF, /*  33- 40 */
	  0xFF, 0x00, 0xC4, 0xAD, 0x00, 0x3B, 0x04, 0x01, /*  41- 48 */
	  0x3E, 0x0B, 0x07, 0x2D, 0x5B, 0x07, 0x00, 0x00, /*  49- 56 */
	  0x00, 0x00, 0x00, 0x00, 0x4D, 0x71, 0x07, 0xB3, /*  57- 64 */
	  0x8E, 0x00, 0xAF, 0xBB, 0x07, 0x00, 0x00, 0x00, /*  65- 72 */
	  0x00, 0x00, 0x00, 0x86, 0x05, 0x00, 0x86, 0x05, /*  73- 80 */
	  0x00, 0xC3, 0x91, 0x07, 0x00, 0x00, 0x00, 0x00, /*  81- 88 */
	  0x00, 0x00, 0x9A, 0xB8, 0x00, 0x34, 0x71, 0x01, /*  89- 96 */
	  0x14, 0xEE, 0x06, 0x9A, 0xB8, 0x00, 0x8F, 0x7D, /*  97-104 */
	  0x00, 0xA8, 0x00, 0x00, 0x51, 0x01, 0x00, 0xB4, /* 105-112 */
	  0xD0, 0x06, 0xA8, 0x00, 0x00, 0x38, 0x5E, 0x00, /* 113-120 */
	  0x00, 0x00, 0x01, 0xFF, 0xFF, 0x01, 0xD0, 0x8B, /* 121-128 */
	  0x06, 0xFF, 0xFF, 0x00, 0xC4, 0xAD, 0x00, 0x99, /* 129-136 */
	  0x99, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 137-144 */
	/* } */
};
