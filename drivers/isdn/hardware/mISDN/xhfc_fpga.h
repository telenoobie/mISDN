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


extern int  xhfc_reg_read(
                  xhfc_t *xhfc,
              __u8 const *reg_addr,
                    __u8 *ipValues,
                unsigned  ivCount);


extern int  xhfc_reg_write(
                   xhfc_t *xhfc,
               __u8 const *reg_addr,
                     __u8 *ipValues,
                 unsigned  ivCount);

extern int  xhfc_array_read(
                    xhfc_t *xhfc,
                __u8 const  reg_addr,
                      __u8 *ipValues,
                  unsigned  ivCount);


extern int  xhfc_array_write(
                     xhfc_t *xhfc,
                 __u8 const  reg_addr,
                       __u8 *ipValues,
                   unsigned  ivCount);


static inline __u8  xhfc_read(
                      xhfc_t *xhfc,
                        __u8  reg_addr)
{
    __u8  lvValue;

    xhfc_reg_read(xhfc,
                 &reg_addr,
                 &lvValue,
                  1);

    return(lvValue);
}


static inline void  xhfc_write(
                       xhfc_t *xhfc,
                         __u8  reg_addr,
                         __u8  value)
{
    xhfc_reg_write(xhfc,
                  &reg_addr,
                  &value,
                   1);

    return;
}


static inline void  xhfc_read32(
                        xhfc_t *xhfc,
                          __u8  reg_addr,
                          __u8 *ipData)
{
    xhfc_array_read(xhfc,
                    reg_addr,
                    ipData,
                    1);

    return;
}


static inline void  xhfc_write32(
                         xhfc_t *xhfc,
                           __u8  reg_addr,
                           __u8 *ipData)
{
    xhfc_array_write(xhfc,
                     reg_addr,
                     ipData,
                     1);

    return;
}


static inline __u8  xhfc_sread(
                       xhfc_t *xhfc,
                         __u8  reg_addr)
{
    return(xhfc_read(xhfc,
                     reg_addr));
}


#define XHFC_RESET_CHIP  // ToDo


#endif /* __XHFC_FPGA_H__ */
