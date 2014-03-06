// UK v.2
const unsigned char vIMD10[] =
{
	0x0F, /* value of reg 0x41, hb/gain, for US PBX, Korea, Taiwan and Standard line it should be 0x5B, others 0x0F */
	0x00, /* value of reg 0xA8, IM1, for Standard, it is 0x01, others 0x00 */
	0x40, /* value of reg 0xF4 */
	0x00, /* value of reg 0xFB, value 0xFF means don't set */
	0x00, /* value of reg 0xDD, Low Byte of ADC Gain */
	0x00, /* value of reg 0xDE, Low Byte of DAC Gain */
	0x44, /* value of reg 0xDF, High Half Bytes of DAC and ADC Gain */
	0x01, /* 276: High Byte count of values to write to FIFO 0xF3 */
	0x14, /* 276: Low Byte count of values to write to FIFO 0xF3 */
	/* { 0,    1,    2,    3,    4,    5,    6,    7 */
	  0xEE, 0x60, 0x01, 0x4C, 0xA9, 0x06, 0x0F, 0x19, /*   1-  8 */
	  0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA8, /*   9- 16 */
	  0x00, 0x00, 0x51, 0x01, 0x00, 0xB4, 0xD0, 0x06, /*  17- 24 */
	  0xA8, 0x00, 0x00, 0x38, 0x5E, 0x00, 0x00, 0x00, /*  25- 32 */
	  0x01, 0xFF, 0xFF, 0x01, 0xD0, 0x8B, 0x06, 0xFF, /*  33- 40 */
	  0xFF, 0x00, 0xC4, 0xAD, 0x00, 0xAF, 0x4C, 0x00, /*  41- 48 */
	  0x8A, 0xB5, 0x07, 0x8B, 0x19, 0x07, 0x00, 0x00, /*  49- 56 */
	  0x00, 0x00, 0x00, 0x00, 0x0B, 0x59, 0x07, 0xF5, /*  57- 64 */
	  0xA6, 0x00, 0xAF, 0xBB, 0x07, 0x00, 0x00, 0x00, /*  65- 72 */
	  0x00, 0x00, 0x00, 0x26, 0x05, 0x00, 0x26, 0x05, /*  73- 80 */
	  0x00, 0xC3, 0x91, 0x07, 0x00, 0x00, 0x00, 0x00, /*  81- 88 */
	  0x00, 0x00, 0x1C, 0xAC, 0x00, 0x38, 0x58, 0x01, /*  89- 96 */
	  0x14, 0xEE, 0x06, 0x1C, 0xAC, 0x00, 0x8F, 0x7D, /*  97-104 */
	  0x00, 0xA8, 0x00, 0x00, 0x51, 0x01, 0x00, 0xB4, /* 105-112 */
	  0xD0, 0x06, 0xA8, 0x00, 0x00, 0x38, 0x5E, 0x00, /* 113-120 */
	  0x00, 0x00, 0x01, 0xFF, 0xFF, 0x01, 0xD0, 0x8B, /* 121-128 */
	  0x06, 0xFF, 0xFF, 0x00, 0xC4, 0xAD, 0x00, 0x99, /* 129-136 */
	  0x99, 0x00, 0xEE, 0x60, 0x01, 0x4C, 0xA9, 0x06, /* 137-144 */
	  0x0F, 0x19, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, /* 145-152 */
	  0x00, 0xA8, 0x00, 0x00, 0x51, 0x01, 0x00, 0xB4, /* 153-160 */
	  0xD0, 0x06, 0xA8, 0x00, 0x00, 0x38, 0x5E, 0x00, /* 161-168 */
	  0x00, 0x00, 0x01, 0xFF, 0xFF, 0x01, 0xD0, 0x8B, /* 169-176 */
	  0x06, 0xFF, 0xFF, 0x00, 0xC4, 0xAD, 0x00, 0xAF, /* 177-184 */
	  0x4C, 0x00, 0x8A, 0xB5, 0x07, 0x8B, 0x19, 0x07, /* 185-192 */
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x59, /* 193-200 */
	  0x07, 0xF5, 0xA6, 0x00, 0xAF, 0xBB, 0x07, 0x00, /* 201-208 */
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x26, 0x05, 0x00, /* 209-216 */
	  0x26, 0x05, 0x00, 0xC3, 0x91, 0x07, 0x00, 0x00, /* 217-224 */
	  0x00, 0x00, 0x00, 0x00, 0x1C, 0xAC, 0x00, 0x38, /* 225-232 */
	  0x58, 0x01, 0x14, 0xEE, 0x06, 0x1C, 0xAC, 0x00, /* 233-240 */
	  0x8F, 0x7D, 0x00, 0xA8, 0x00, 0x00, 0x51, 0x01, /* 241-248 */
	  0x00, 0xB4, 0xD0, 0x06, 0xA8, 0x00, 0x00, 0x38, /* 249-256 */
	  0x5E, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0x01, /* 257-264 */
	  0xD0, 0x8B, 0x06, 0xFF, 0xFF, 0x00, 0xC4, 0xAD, /* 265-272 */
	  0x00, 0x99, 0x99, 0x00, /* 273-276 */
	/* } */
};
