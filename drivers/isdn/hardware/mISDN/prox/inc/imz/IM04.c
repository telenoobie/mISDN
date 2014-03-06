// CTR21 
const unsigned char vIM04[] =
{
	0x0F, /* value of reg 0x41, hb/gain, for US PBX, Korea, Taiwan and Standard line it should be 0x5B, others 0x0F */
	0x00, /* value of reg 0xA8, IM1, for Standard, it is 0x01, others 0x00 */
	0x50, /* value of reg 0xF4 */
	0x00, /* value of reg 0xFB, value 0xFF means don't set */
	0x00, /* value of reg 0xDD, Low Byte of ADC Gain */
	0x00, /* value of reg 0xDE, Low Byte of DAC Gain */
	0x44, /* value of reg 0xDF, High Half Bytes of DAC and ADC Gain */
	0x00, /* 192: High Byte count of values to write to FIFO 0xF3 */
	0xC0, /* 192: Low Byte count of values to write to FIFO 0xF3 */
	/* { 0,    1,    2,    3,    4,    5,    6,    7 */
	  0x60, 0x7B, 0x01, 0xFD, 0x95, 0x06, 0x19, 0x1E, /*   1-  8 */
	  0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, /*   9- 16 */
	  0x01, 0x00, 0x62, 0x02, 0x00, 0x6E, 0xF1, 0x06, /*  17- 24 */
	  0x31, 0x01, 0x00, 0xDE, 0x4C, 0x00, 0x00, 0x00, /*  25- 32 */
	  0x01, 0xFF, 0xFF, 0x01, 0xAF, 0xAB, 0x06, 0xFF, /*  33- 40 */
	  0xFF, 0x00, 0xAC, 0xA2, 0x00, 0xED, 0x6A, 0x00, /*  41- 48 */
	  0x19, 0x99, 0x07, 0xE4, 0x18, 0x07, 0x00, 0x00, /*  49- 56 */
	  0x00, 0x00, 0x00, 0x00, 0x4D, 0x71, 0x07, 0xB3, /*  57- 64 */
	  0x8E, 0x00, 0xAF, 0xBB, 0x07, 0x00, 0x00, 0x00, /*  65- 72 */
	  0x00, 0x00, 0x00, 0x86, 0x05, 0x00, 0x86, 0x05, /*  73- 80 */
	  0x00, 0xC3, 0x91, 0x07, 0x00, 0x00, 0x00, 0x00, /*  81- 88 */
	  0x00, 0x00, 0x9A, 0xB8, 0x00, 0x34, 0x71, 0x01, /*  89- 96 */
	  0x14, 0xEE, 0x06, 0x9A, 0xB8, 0x00, 0x8F, 0x7D, /*  97-104 */
	  0x00, 0x31, 0x01, 0x00, 0x62, 0x02, 0x00, 0x6E, /* 105-112 */
	  0xF1, 0x06, 0x31, 0x01, 0x00, 0xDE, 0x4C, 0x00, /* 113-120 */
	  0x00, 0x00, 0x01, 0xFF, 0xFF, 0x01, 0xAF, 0xAB, /* 121-128 */
	  0x06, 0xFF, 0xFF, 0x00, 0xAC, 0xA2, 0x00, 0x99, /* 129-136 */
	  0x99, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 137-144 */
	  0x00, 0x00, 0x00, 0xA1, 0x07, 0x00, 0x65, 0xF4, /* 145-152 */
	  0x07, 0xA5, 0x49, 0x06, 0xA1, 0x07, 0x00, 0xBA, /* 153-160 */
	  0xBE, 0x00, 0x00, 0x00, 0x01, 0x39, 0x1A, 0x06, /* 161-168 */
	  0x18, 0x25, 0x06, 0xFF, 0xFF, 0x00, 0xB4, 0xE8, /* 169-176 */
	  0x00, 0x00, 0x00, 0x01, 0x1A, 0x13, 0x06, 0xA5, /* 177-184 */
	  0x14, 0x06, 0x00, 0x00, 0x01, 0x60, 0xFB, 0x00, /* 185-192 */
	/* } */
};
