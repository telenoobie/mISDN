/*
 * Author: Brian J Forde
 */
#ifndef __XHFC_FPGA_H__
#define __XHFC_FPGA_H__

#include <linux/delay.h>
#include <asm/semaphore.h>
#include "ifx_core_drv/include/port.h"

struct fpga_exp_card_device;
struct xhfc;

#define XHFC_HW_TYPE(v)		struct fpga_exp_card_device *v

#define XHFC_NAME_FMT		"%s_%d"
#define XHFC_NAME_PARAM(x)	xhfc_fpga_get_slot_number(x->hw)

extern int xhfc_fpga_get_slot_number(struct fpga_exp_card_device *);
extern struct xhfc *xhfc_array[];


#undef  XHFC_PORT_LOCKING
#undef	XHFC_PORT_LOCK_TYPE
#undef  XHFC_USE_BH_TASKLET
#define XHFC_USE_BH_WORKQUEUE
#define XHFC_NO_BUSY_WAIT
#define L1_CALLBACK_BH

#define XHFC_CHIP_LOCK
#define XHFC_CHIP_LOCK_TYPE(v)	struct semaphore v

#define xhfc_lock_init(x)	do {sema_init(&x->lock, 1);} while(0)
#define xhfc_port_lock_init(p)	while(0)
#define xhfc_port_lock(p)	do {down_interruptible(&p->xhfc->lock);} while(0)
#define xhfc_port_unlock(p)	do {up(&p->xhfc->lock);} while(0)
#define xhfc_port_lock_bh(p)	do {down_interruptible(&p->xhfc->lock);} while(0)
#define xhfc_port_unlock_bh(p)	do {up(&p->xhfc->lock);} while(0)
#define xhfc_lock(x)		do {down_interruptible(&x->lock);} while(0)
#define xhfc_unlock(x)		do {up(&x->lock);} while(0)

extern int  xhfc_reg_read(struct xhfc *xhfc, __u8 const *reg_addr,
	__u8 *ipValues, unsigned  ivCount);


extern int  xhfc_reg_write(struct xhfc *xhfc, __u8 const *reg_addr,
	__u8 *ipValues, unsigned  ivCount);

extern int  xhfc_array_read(struct xhfc *xhfc, __u8 const  reg_addr,
	__u8 *ipValues, unsigned  ivCount);


extern int  xhfc_array_write(struct xhfc *xhfc, __u8 const  reg_addr,
	__u8 *ipValues, unsigned  ivCount);


static inline __u8  read_xhfc(struct xhfc *xhfc, __u8  reg_addr)
{
    __u8  lvValue;

    xhfc_reg_read(xhfc, &reg_addr, &lvValue, 1);
    return lvValue;
}


static inline void  write_xhfc(struct xhfc *xhfc, __u8  reg_addr, __u8  value)
{
    xhfc_reg_write(xhfc, &reg_addr, &value, 1);
    return;
}


static inline void read32_xhfc(struct xhfc *xhfc, __u8  reg_addr, __u8 *ipData)
{
    xhfc_array_read(xhfc, reg_addr, ipData, 1);
    return;
}


static inline void write32_xhfc(struct xhfc *xhfc, __u8  reg_addr, __u8 *ipData)
{
    xhfc_array_write(xhfc, reg_addr, ipData, 1);
    return;
}


static inline __u8 sread_xhfc(struct xhfc *xhfc,__u8  reg_addr)
{
    return(read_xhfc(xhfc, reg_addr));
}


extern void  xhfc_NTR_Start(
               struct xhfc *xhfc);

extern void  xhfc_NTR_Stop(
               struct xhfc *xhfc);


#define XHFC_RESET_CHIP  // ToDo

#endif /* __XHFC_FPGA_H__ */
