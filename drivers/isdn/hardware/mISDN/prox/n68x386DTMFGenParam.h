/**
	@file
	@brief DTMF generate parameters for N68x38x.
**/

#ifndef N68X386DTMFGENPARAMHDR
#define N68X386DTMFGENPARAMHDR

#if !WIDE_BAND // todo
static const short awFreqRow_x386[4] =
{
	/* Row 0: 697 Hz */
	0x7b3c,
	/* Row 1: 770 Hz */
	0x7a31,
	/* Row 2: 852 Hz */
	0x78e7,
	/* Row 3: 941 Hz */
	0x775c
};

// OSC2: MSB 15 bits
static const short awFreqColumn_x386[4] =
{
	/* Column 0: 1209 Hz */
	0x71D8,
	/* Column 1: 1336 Hz */
	0x6EC9,
	/* Column 2: 1477 Hz */
	0x6B11,
	/* Column 3: 1633 Hz */
	0x6692
};

static const short awAmplitudeRow_x386[4] =
{
	/* Row 0: 697 Hz */
	0x06CC,
	/* Row 1: 770 Hz */
	0x077D,
	/* Row 2: 852 Hz */
	0x0842,
	/* Row 3: 941 Hz */
	0x0915		
};

static const short awAmplitudeColumn_x386[4] =
{
	/* Column 0: 1209 Hz */
	0x1465,
	/* Column 1: 1336 Hz */
	0x1659,
	/* Column 2: 1477 Hz */
	0x1874,
	/* Column 3: 1633 Hz */
	0x1AB1		
};

#else // WIDE_BAND
static const short awFreqRow_x386[4] =
{
	/* Row 0: 697 Hz */
	0x6D4B,
	/* Row 1: 770 Hz */
	0x694C,
	/* Row 2: 852 Hz */
	0x6465,
	/* Row 3: 941 Hz */
	0x5E9A
};

// OSC2: MSB 15 bits
static const short awFreqColumn_x386[4] =
{
	/* Column 0: 1209 Hz */
	0x4A80,
	/* Column 1: 1336 Hz */
	0x3FC4,
	/* Column 2: 1477 Hz */
	0x331C,
	/* Column 3: 1633 Hz */
	0x2462
};

static const short awAmplitudeRow_x386[4] =
{
	/* Row 0: 697 Hz */
	0x0D09,
	/* Row 1: 770 Hz */
	0x0E3D,
	/* Row 2: 852 Hz */
	0x0F89,
	/* Row 3: 941 Hz */
	0x10DE		
};

static const short awAmplitudeColumn_x386[4] =
{
	/* Column 0: 1209 Hz */
	0x2437,
	/* Column 1: 1336 Hz */
	0x269D,
	/* Column 2: 1477 Hz */
	0x28D5,
	/* Column 3: 1633 Hz */
	0x2AB3		
};

#endif // WIDE_BAND

#ifndef PROXDTMFGENSTRUCTS

#define PROXDTMFGENSTRUCTS 1

typedef struct tagDTMFAXIS
{
	int row;
	int col;
}DTMFAXIS;

static DTMFAXIS asDigit[16] =
{ 
	/* Row, col: index beginning 0*/
	{3, 1},  /*0*/ 

	{0, 0},  /*1*/ 
	{0, 1},  /*2*/ 
	{0, 2},  /*3*/ 
	{1, 0},  /*4*/ 
	{1, 1},  /*5*/ 
	{1, 2},  /*6*/ 
	{2, 0},  /*7*/ 
	{2, 1},  /*8*/ 
	{2, 2},  /*9*/ 

	{0, 3},  /*A*/ 
	{1, 3},  /*B*/ 
	{2, 3},  /*C*/ 
	{3, 3},  /*D*/ 

	{3, 0},  /***/ 
	{3, 2},  /*#*/ 
};

#endif // PROXDTMFGENSTRUCTS

#endif // N68X386DTMFGENPARAMHDR
