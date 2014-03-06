
// qkdang, 2010

#ifndef NXSINCOSHDR
#define NXSINCOSHDR

// return (2^20)*sin(2*PI*idx/16000)
// param idx <= 4000
NXFUNSTR __u32 nxSin(int idx);

// return (2^20)*cos(2*PI*idx/16000)
// param idx <= 4000
NXFUNSTR __u32 nxCos(int idx);

#endif // NXSINCOSHDR
