/*
 * Author: Brian J Forde
 */
#ifndef __XHFC_FPGA_H__
#define __XHFC_FPGA_H__

#include <linux/delay.h>
#include "ifx_core_drv/include/port.h"
#define xhfc_port_lock_init(p)	while(0)
#define xhfc_port_lock(p)	while(0)
#define xhfc_port_unlock(p)	while(0)
#define xhfc_port_lock_bh(p)	while(0)
#define xhfc_port_unlock_bh(p)	while(0)


typedef struct xhfc xhfc_t;

extern __u8  read_xhfc(
               xhfc_t *xhfc,
                 __u8  reg_addr);

extern void  write_xhfc(
                xhfc_t *xhfc,
                  __u8  reg_addr,
                  __u8  value);


static inline __u8  sread_xhfc(
                       xhfc_t *xhfc,
                         __u8  reg_addr)
{
    return(read_xhfc(xhfc,
                     reg_addr));
}


#define XHFC_RESET_CHIP  // ToDo


#endif /* __XHFC_FPGA_H__ */
