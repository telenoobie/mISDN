// Variation v.2
const unsigned char vIM12[] =
{
	0x0F, /* value of reg 0x41, hb/gain, for US PBX, Korea, Taiwan and Standard line it should be 0x5B, others 0x0F */
	0x00, /* value of reg 0xA8, IM1, for Standard, it is 0x01, others 0x00 */
	0x20, /* value of reg 0xF4 */
	0x01, /* value of reg 0xFB, value 0xFF means don't set */
	0x00, /* value of reg 0xDD, Low Byte of ADC Gain */
	0x00, /* value of reg 0xDE, Low Byte of DAC Gain */
	0x44, /* value of reg 0xDF, High Half Bytes of DAC and ADC Gain */
	0x00, /* 192: High Byte count of values to write to FIFO 0xF3 */
	0xC0, /* 192: Low Byte count of values to write to FIFO 0xF3 */
	/* { 0,    1,    2,    3,    4,    5,    6,    7 */
	  0x6D, 0xCC, 0x00, 0x93, 0x33, 0x07, 0xF0, 0x00, /*   1-  8 */
	  0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA8, /*   9- 16 */
	  0x00, 0x00, 0x51, 0x01, 0x00, 0xD0, 0x8B, 0x06, /*  17- 24 */
	  0xA8, 0x00, 0x00, 0xC4, 0xAD, 0x00, 0x00, 0x00, /*  25- 32 */
	  0x01, 0x00, 0x00, 0x02, 0xB4, 0xD0, 0x06, 0x00, /*  33- 40 */
	  0x00, 0x01, 0x38, 0x5E, 0x00, 0x77, 0x9F, 0x00, /*  41- 48 */
	  0x89, 0x60, 0x07, 0xE4, 0x00, 0x07, 0x00, 0x00, /*  49- 56 */
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*  57- 64 */
	  0x00, 0x00, 0xF6, 0xC3, 0x07, 0x00, 0x00, 0x00, /*  65- 72 */
	  0x00, 0x00, 0x00, 0x2A, 0x00, 0x00, 0x54, 0x00, /*  73- 80 */
	  0x00, 0x7F, 0x3B, 0x06, 0x2A, 0x00, 0x00, 0x43, /*  81- 88 */
	  0xCF, 0x00, 0x1C, 0xAC, 0x00, 0x1C, 0xAC, 0x00, /*  89- 96 */
	  0x42, 0x31, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, /*  97-104 */
	  0x00, 0xA8, 0x00, 0x00, 0x51, 0x01, 0x00, 0xD0, /* 105-112 */
	  0x8B, 0x06, 0xA8, 0x00, 0x00, 0xC4, 0xAD, 0x00, /* 113-120 */
	  0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0xB4, 0xD0, /* 121-128 */
	  0x06, 0x00, 0x00, 0x01, 0x38, 0x5E, 0x00, 0xEF, /* 129-136 */
	  0xA7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 137-144 */
	  0x00, 0x00, 0x00, 0xA1, 0x07, 0x00, 0x50, 0xF1, /* 145-152 */
	  0x07, 0xA5, 0x14, 0x06, 0xA1, 0x07, 0x00, 0x60, /* 153-160 */
	  0xFB, 0x00, 0x00, 0x00, 0x01, 0x39, 0x1A, 0x06, /* 161-168 */
	  0x18, 0x25, 0x06, 0xFF, 0xFF, 0x00, 0xB4, 0xE8, /* 169-176 */
	  0x00, 0x00, 0x00, 0x01, 0x88, 0x7A, 0x06, 0xA5, /* 177-184 */
	  0x49, 0x06, 0x00, 0x00, 0x01, 0xBA, 0xBE, 0x00, /* 185-192 */
	/* } */
};
