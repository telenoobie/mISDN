#ifndef N68X386RWHDR
#define N68X386RWHDR

NXFUNSTR int n68x386ReadReg(__chip_inst pInst, __chip_chnl chnl, int regAddr);
NXFUNSTR int n68x386ReadShort(__chip_inst pInst, __chip_chnl chnl, int regAddr);
NXFUNSTR int n68x386WriteReg(__chip_inst pInst, __chip_chnl chnl, int regAddr, int regVal);
NXFUNSTR int n68x386WriteRegs(__chip_inst pInst, __chip_chnl chnl, int regStartAddr, const unsigned char *vals, int cnt);

#endif // N68X386RWHDR
