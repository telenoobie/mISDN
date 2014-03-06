/**
	@file
	@brief Registers definition for N68x38x.
**/

/*
qkdang
nuvoton
2008.9

registers definations
NX386_RXX_Name, XX is the hex address of the register

ver 0.8: change R06_DVID to R07
     add R81, R82 for GR909 test

*/
#ifndef NX386REGSHDR
#define NX386REGSHDR

#define	NX386_R00_PCMC		0x00
#define	NX386_R01_TTLC		0x01
#define	NX386_R02_RTLC		0x02
#define	NX386_R03_TCHC		0x03
#define	NX386_R04_PLLS		0x04
#define	NX386_R05_PCMFS		0x05
//#define	NX386_R06_DVID		0x06
#define	NX386_R07_DVID		0x07

#define NX386_R08_TTLWB		0x08
#define NX386_R09_RTLWB		0x09

#define	NX386_R10_FSKC		0x10
#define	NX386_R11_FSKTD		0x11
#define	NX386_R12_FSKS		0x12
#define	NX386_R13_FSKLCR	0x13
#define	NX386_R14_FSKTCR	0x14

#define	NX386_R15_DIAGXC0	0x15
#define	NX386_R16_DIAGXC1	0x16
#define	NX386_R17_DIAGXC2	0x17
#define	NX386_R18_DIAGXC3	0x18
#define	NX386_R19_DIAGXC4	0x19
#define	NX386_R1A_DIAGXC5	0x1A
#define	NX386_R1B_DIAGXC6	0x1B
#define	NX386_R1C_DIAGXC7	0x1C
#define	NX386_R1D_DIAGXC8	0x1D
#define	NX386_R1E_DIAGXF0	0x1E
#define	NX386_R1F_DIAGXF1	0x1F

#define	NX386_R20_HPFC		0x20
#define	NX386_R21_LBC		0x21
#define	NX386_R22_PONC		0x22

#define	NX386_R23_ILIM		0x23

#define	NX386_R24_INTVL		0x24
//#define	NX386_R25_INTVH		0x25
#define	NX386_R26_INT1		0x26
#define	NX386_R27_IE1		0x27
#define	NX386_R28_INT2		0x28
#define	NX386_R29_IE2		0x29
#define	NX386_R2A_INT3		0x2A
#define	NX386_R2B_IE3		0x2B

#define	NX386_R30_DTMFC				0x30
#define	NX386_R33_DTMFS				0x33
#define	NX386_R34_DTMFTH			0x34
#define	NX386_R35_DTMFTL			0x35
#define	NX386_R36_DTMFPT			0x36
#define	NX386_R37_DTMFAT			0x37
#define	NX386_R38_DTMFDT			0x38
#define	NX386_R39_DTMFTEST			0x39
#define	NX386_R3A_DTMFD				0x3A
#define	NX386_R3B_DTMFROWFREQH		0x3B
#define	NX386_R3C_DTMFROWFREQL		0x3C
#define	NX386_R3D_DTMFCOLFREQH		0x3D
#define	NX386_R3E_DTMFCOLFREQL		0x3E

#define	NX386_R40_ACGC		0x40
#define	NX386_R41_HBC		0x41
#define	NX386_R42_VCMRC		0x42
#define	NX386_R43_LAMC		0x43
#define	NX386_R44_LSC		0x44
#define	NX386_R45_LCLC		0x45
#define	NX386_R46_RTDSLC	0x46
#define	NX386_R47_LCDB		0x47
#define	NX386_R48_RTDBA		0x48
#define	NX386_R49_PWMT		0x49
#define	NX386_R4A_DDCC		0x4A

#define	NX386_R4C_OHVC		0x4C
#define	NX386_R4D_GMVC		0x4D
#define	NX386_R4E_VBHVC		0x4E
#define	NX386_R4F_VBLVC		0x4F
#define	NX386_R50_LCDCL		0x50
#define	NX386_R51_RTDFCLD	0x51
#define	NX386_R52_DCHD		0x52
#define	NX386_R53_LCT		0x53
#define	NX386_R54_LCTHY		0x54
#define	NX386_R55_RTTA		0x55
#define	NX386_R56_VOVC		0x56
#define	NX386_R57_DCTON		0x57
#define	NX386_R58_XBAG		0x58

#define	NX386_R5E_AUTOMT	0x5E
#define	NX386_R5F_XBTC		0x5F
#define	NX386_R60_GKDH		0x60
#define	NX386_R61_GKDL		0x61
#define	NX386_R62_GKDDT		0x62
#define	NX386_R63_GKDFCL	0x63
#define	NX386_R64_GKDFCH	0x64
#define	NX386_R65_RTDFCLD	0x65
#define	NX386_R66_DCHD		0x66
#define	NX386_R67_RTTD		0x67
#define	NX386_R68_RTDBD		0x68

#define	NX386_R6A_XBSDCN	0x6A
#define	NX386_R6B_XBSDCP	0x6B
//#define	NX386_R6C_XRDLY1	0x6C
//#define	NX386_R6D_XRDLY2	0x6D
#define	NX386_R6E_LOAD		0x6E

#define	NX386_R77_DCTRC		0x77

#define	NX386_R80_BATVC		0x80

#define	NX386_R81_TIPV		0x81
#define	NX386_R82_RINGV		0x82

#define	NX386_R83_QT3VC		0x83
#define	NX386_R84_QR3VC		0x84
#define	NX386_R85_QT3IC		0x85
#define	NX386_R86_QR3IC		0x86
#define	NX386_R87_QT1IC		0x87
#define	NX386_R88_QT2IC		0x88
#define	NX386_R89_QR1IC		0x89
#define	NX386_R8A_QR2IC		0x8A
//#define	NX386_R8B_VT		0x8B
#define	NX386_R8C_LGIC		0x8C
#define	NX386_R8D_LPVC		0x8D
#define	NX386_R8E_TIPIC		0x8E
#define	NX386_R8F_RINGIC	0x8F
#define	NX386_R90_LPIC		0x90
#define	NX386_R91_POLC		0x91
#define	NX386_R92_SCMC		0x92

#define	NX386_R93_VEQT1		0x93
#define	NX386_R94_VQT1		0x94
#define	NX386_R95_VEQR1		0x95
#define	NX386_R96_VQR1		0x96

//#define	NX386_R97_XVRGSC	0x97
//#define	NX386_R98_CAL5C 	0x98

#define	NX386_R99_TEMP		0x99
#define	NX386_R9A_VBGAP		0x9A

#define	NX386_R9B_VLPP2P	0x9B
#define	NX386_R9C_ILPP2P	0x9C
#define	NX386_R9D_VQT3		0x9D
#define	NX386_R9E_VQR3		0x9E

#define	NX386_R9F_PALCNT	0x9F

#define	NX386_RA0_PALPQ2	0xA0
#define	NX386_RA1_PALPQ1	0xA1
#define	NX386_RA2_PALPQ3	0xA2
#define	NX386_RA3_PALPQH1	0xA3
#define	NX386_RA4_PALPQH2	0xA4
#define	NX386_RA5_PATHQ2	0xA5
#define	NX386_RA6_PATHQ1	0xA6
#define	NX386_RA7_PATHQ3	0xA7
#define	NX386_RA8_IM1C		0xA8
#define	NX386_RA9_IM2C		0xA9
#define	NX386_RAA_THAT		0xAA
#define	NX386_RAB_LCMCNT	0xAB
#define	NX386_RAC_CC		0xAC
#define	NX386_RAD_OS2RPD	0xAD

#define	NX386_RAF_CAL1		0xAF
#define	NX386_RB0_CAL2		0xB0
#define	NX386_RB1_CAL3		0xB1
#define	NX386_RB2_VBTOSC	0xB2
#define	NX386_RB3_RDCOSC	0xB3
#define	NX386_RB4_IQTROSC	0xB4
#define	NX386_RB5_PWCTC  	0xB5
#define	NX386_RB6_CAL4C		0xB6

#define	NX386_RC0_OSNC		0xC0
#define	NX386_RC1_RMPC		0xC1
#define	NX386_RC2_OS1ICL	0xC2
#define	NX386_RC3_OS1ICH	0xC3
#define	NX386_RC4_OS2ICL	0xC4
#define	NX386_RC5_OS2ICH	0xC5
#define	NX386_RC6_OS1CL		0xC6
#define	NX386_RC7_OS1CH		0xC7
#define	NX386_RC8_OS2CL		0xC8
#define	NX386_RC9_OS2CH		0xC9
#define	NX386_RCA_OS1ATL	0xCA
#define	NX386_RCB_OS1ATH	0xCB
#define	NX386_RCC_OS2ATL	0xCC
#define	NX386_RCD_OS2ATH	0xCD
#define	NX386_RCE_OS1ITL	0xCE
#define	NX386_RCF_OS1ITH	0xCF
#define	NX386_RD0_OS2ITL	0xD0
#define	NX386_RD1_OS2ITH	0xD1
//#define	NX386_RD2_MPATL		0xD2
//#define	NX386_RD3_MPATH		0xD3
//#define	NX386_RD4_MPITL		0xD4
//#define	NX386_RD5_MPITH		0xD5
//#define	NX386_RD6_MPICL		0xD6
//#define	NX386_RD7_MPICH		0xD7
//#define	NX386_RD8_MPCL		0xD8
//#define	NX386_RD9_MPCH		0xD9
//#define	NX386_RDA_MPADS		0xDA
//#define	NX386_RDB_MMAR		0xDB
#define	NX386_RDC_ROFFS		0xDC
#define	NX386_RDD_ADCL		0xDD
#define	NX386_RDE_DACL		0xDE
#define	NX386_RDF_DGH		0xDF
#define	NX386_RE0_ST0L0		0xE0
#define	NX386_RE1_ST1L0		0xE1
#define	NX386_RE2_ST2L0		0xE2
#define	NX386_RE3_ST0L1		0xE3
#define	NX386_RE4_ST1L1		0xE4
#define	NX386_RE5_ST2L1		0xE5
#define	NX386_RE6_SK0L0		0xE6
#define	NX386_RE7_SK1L0		0xE7
#define	NX386_RE8_SK2L0		0xE8
#define	NX386_RE9_SK0L1		0xE9
#define	NX386_REA_SK1L1		0xEA
#define	NX386_REB_SK2L1		0xEB
#define	NX386_REC_WM0		0xEC
#define	NX386_RED_WM1		0xED
#define	NX386_REE_WM2		0xEE
#define	NX386_REF_XSTEP		0xEF

//#define	NX386_RF0_TM_OUT_L	0xF0
//#define	NX386_RF1_TM_DIG	0xF1
//#define	NX386_RF2_TM_DIGH	0xF2


#define	NX386_R00	0x00
#define	NX386_R01	0x01
#define	NX386_R02	0x02
#define	NX386_R03	0x03
#define	NX386_R04	0x04
#define	NX386_R05	0x05
#define	NX386_R06	0x06
#define	NX386_R07	0x07
#define	NX386_R08	0x08
#define	NX386_R09	0x09

#define	NX386_R10	0x10
#define	NX386_R11	0x11
#define	NX386_R12	0x12
#define	NX386_R13	0x13
#define	NX386_R14	0x14

#define	NX386_R15	0x15
#define	NX386_R16	0x16
#define	NX386_R17	0x17
#define	NX386_R18	0x18
#define	NX386_R19	0x19
#define	NX386_R1A	0x1A
#define	NX386_R1B	0x1B
#define	NX386_R1C	0x1C
#define	NX386_R1D	0x1D
#define	NX386_R1E	0x1E
#define	NX386_R1F	0x1F

#define	NX386_R20	0x20
#define	NX386_R21	0x21
#define	NX386_R22	0x22

#define	NX386_R24	0x24
//#define	NX386_R25	0x25
#define	NX386_R26	0x26
#define	NX386_R27	0x27
#define	NX386_R28	0x28
#define	NX386_R29	0x29
#define	NX386_R2A	0x2A
#define	NX386_R2B	0x2B

#define	NX386_R30	0x30
#define	NX386_R31	0x31
#define	NX386_R32	0x32
#define	NX386_R33	0x33
#define	NX386_R33	0x33
#define	NX386_R35	0x35
#define	NX386_R36	0x36
#define	NX386_R37	0x37
#define	NX386_R38	0x38
#define	NX386_R39	0x39
#define	NX386_R3A	0x3A
#define	NX386_R3C	0x3C
#define	NX386_R3D	0x3D
#define	NX386_R3E	0x3E

#define	NX386_R40	0x40
#define	NX386_R41	0x41
#define	NX386_R42	0x42
#define	NX386_R43	0x43
#define	NX386_R44	0x44
#define	NX386_R45	0x45
#define	NX386_R46	0x46
#define	NX386_R47	0x47
#define	NX386_R48	0x48
#define	NX386_R49	0x49
#define	NX386_R4A	0x4A

#define	NX386_R4C	0x4C
#define	NX386_R4D	0x4D
#define	NX386_R4E	0x4E
#define	NX386_R4F	0x4F
#define	NX386_R50	0x50
#define	NX386_R51	0x51
#define	NX386_R52	0x52
#define	NX386_R53	0x53
#define	NX386_R54	0x54
#define	NX386_R55	0x55
#define	NX386_R56	0x56
#define	NX386_R57	0x57
#define	NX386_R58	0x58

#define	NX386_R5E	0x5E
#define	NX386_R5F	0x5F
#define	NX386_R60	0x60
#define	NX386_R61	0x61
#define	NX386_R62	0x62
#define	NX386_R63	0x63
#define	NX386_R64	0x64
#define	NX386_R65	0x65
#define	NX386_R66	0x66
#define	NX386_R67	0x67
#define	NX386_R68	0x68

#define	NX386_R6A	0x6A
#define	NX386_R6B	0x6B
//#define	NX386_R6C	0x6C
//#define	NX386_R6D	0x6D
#define	NX386_R6E	0x6E

#define	NX386_R77	0x77

#define	NX386_R80	0x80

#define	NX386_R83	0x83
#define	NX386_R84	0x84
#define	NX386_R85	0x85
#define	NX386_R86	0x86
#define	NX386_R87	0x87
#define	NX386_R88	0x88
#define	NX386_R89	0x89
#define	NX386_R8A	0x8A
//#define	NX386_R8B	0x8B
#define	NX386_R8C	0x8C
#define	NX386_R8D	0x8D
#define	NX386_R8E	0x8E
#define	NX386_R8F	0x8F
#define	NX386_R90	0x90
#define	NX386_R91	0x91
#define	NX386_R92	0x92

#define	NX386_R93	0x93
#define	NX386_R94	0x94
#define	NX386_R95	0x95
#define	NX386_R96	0x96

//#define	NX386_R97	0x97
//#define	NX386_R98	0x98

#define	NX386_R99	0x99
#define	NX386_R9A	0x9A

#define	NX386_R9B	0x9B
#define	NX386_R9C	0x9C
#define	NX386_R9D	0x9D
#define	NX386_R9E	0x9E

#define	NX386_R9F	0x9F

#define	NX386_RA0	0xA0
#define	NX386_RA1	0xA1
#define	NX386_RA2	0xA2
#define	NX386_RA3	0xA3
#define	NX386_RA4	0xA4
#define	NX386_RA5	0xA5
#define	NX386_RA6	0xA6
#define	NX386_RA7	0xA7
#define	NX386_RA8	0xA8
#define	NX386_RA9	0xA9
#define	NX386_RAA	0xAA
#define	NX386_RAB	0xAB
#define	NX386_RAC	0xAC
#define	NX386_RAD	0xAD

#define	NX386_RAF	0xAF
#define	NX386_RB0	0xB0
#define	NX386_RB1	0xB1
#define	NX386_RB2	0xB2
#define	NX386_RB3	0xB3
#define	NX386_RB4	0xB4
#define	NX386_RB5	0xB5
#define	NX386_RB6	0xB6

#define	NX386_RC0	0xC0
#define	NX386_RC1	0xC1
#define	NX386_RC2	0xC2
#define	NX386_RC3	0xC3
#define	NX386_RC4	0xC4
#define	NX386_RC5	0xC5
#define	NX386_RC6	0xC6
#define	NX386_RC7	0xC7
#define	NX386_RC8	0xC8
#define	NX386_RC9	0xC9
#define	NX386_RCA	0xCA
#define	NX386_RCB	0xCB
#define	NX386_RCC	0xCC
#define	NX386_RCD	0xCD
#define	NX386_RCE	0xCE
#define	NX386_RCF	0xCF
#define	NX386_RD0	0xD0
#define	NX386_RD1	0xD1
//#define	NX386_RD2	0xD2
//#define	NX386_RD3	0xD3
//#define	NX386_RD4	0xD4
//#define	NX386_RD5	0xD5
//#define	NX386_RD6	0xD6
//#define	NX386_RD7	0xD7
//#define	NX386_RD8	0xD8
//#define	NX386_RD9	0xD9
//#define	NX386_RDA	0xDA
//#define	NX386_RDB	0xDB
#define	NX386_RDC	0xDC
#define	NX386_RDD	0xDD
#define	NX386_RDE	0xDE
#define	NX386_RDF	0xDF
#define	NX386_RE0	0xE0
#define	NX386_RE1	0xE1
#define	NX386_RE2	0xE2
#define	NX386_RE3	0xE3
#define	NX386_RE4	0xE4
#define	NX386_RE5	0xE5
#define	NX386_RE6	0xE6
#define	NX386_RE7	0xE7
#define	NX386_RE8	0xE8
#define	NX386_RE9	0xE9
#define	NX386_REA	0xEA
#define	NX386_REB	0xEB
#define	NX386_REC	0xEC
#define	NX386_RED	0xED
#define	NX386_REE	0xEE
#define	NX386_REF	0xEF

//#define	NX386_RF0	0xF0
//#define	NX386_RF1	0xF1
//#define	NX386_RF2	0xF2

#endif
