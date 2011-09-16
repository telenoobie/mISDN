
/*
 * Author: Brian J Forde
 */

#include <linux/module.h>
#include <asm/semaphore.h>
#include "xhfc_su.h"
#include "xhfc_fpga.h"

#include "fpgaDriverEx/drv_fpga_common.h"
#include "fpgaDriverEx/drv_fpga_hl_device_driver.h"
#include "fpgaDriverEx/drv_fpga_registration.h"
#include "fpgaDriverEx/drv_fpga_irq.h"
#include "fpgaDriverEx/drv_fpga_irq_defs.h"
#include "fpgaDriverEx/drv_fpga_exp_card.h"
#include "fpgaDriverEx/drv_fpga_exp_card_device.h"
#include "fpgaDriverEx/drv_fpga_readwrite.h"


#define MAX_EXP_CARD_DEVICES    16
static xhfc_t *xhfc_array[MAX_EXP_CARD_DEVICES];


//#define XHFC_DEBUG



/* SPI mode transaction bit definitions */
#define SPI_ADDR        0x40
#define SPI_DATA        0x00
#define SPI_RD          0x80
#define SPI_WR          0x00
#define SPI_BROAD       0x20 // Not used - only 1 chip on the board.
#define SPI_NO_BROAD    0x00
#define SPI_MULTI       0x20
#define SPI_NO_MULTI    0x00

// Defines for writing data to the SPI bus
#define WRITE_BYTE_BUF_LEN              4 // Buffer for writing a single byte to a specific address.
                                          // Byte 1 - Control byte for register address
                                          // Byte 2 - Register address to write to
                                          // Byte 3 - Control byte for data (0000 0000)
                                          // Byte 4 - Data to write
#define WRITE_MULTIPLE_BYTE_BUF         7 // Buffer for writing 4 bytes
                                          // Byte 1 - Control byte for register addres
                                          // Byte 2 - Register address to write to
                                          // Byte 3 - Control byte for data (0010 0000)
                                          // Byte 4 - 7 - Data to write


// Defines for reading data from the SPI bus
#define READ_BYTE_BUF_LEN               4 // Buffer for reading a single byte to a specific address.
                                          // Byte 1 - Control byte for register address
                                          // Byte 2 - Register address to read from
                                          // Byte 3 - Control byte for data (1000 0000)
                                          // Byte 4 - Storage to read data into
#define READ_MULTIPLE_BYTE_BUF          7 // Buffer for reading 4 bytes
                                          // Byte 1 - Control byte for register addres
                                          // Byte 2 - Register address to read from
                                          // Byte 3 - Control byte for data (1010 0000)
                                          // Byte 4 - 7 - Storage to read data into

// Common Defines for reading and writing to the SPI bus
#define REG_CONTROL_BYTE                0 // First byte is the control byte for the address
#define REG_ADDR_BYTE                   1 // Second byte is the address byte
#define DATA_CONTORL_BYTE               2 // Third byte is the contol byte for the data
#define DATA_START_BYTE                 3 // Offset into the buffer for the start of the data.
#define MULTIPLE_BYTES_NUM              4 // When transmitting in multiple byte mode the number of bytes that are transmitted is 4.

// Device name for registering with SPI driver
#define DEVICE_NAME "xhfc2su"

// This device ID is from pins 8,9,10 and 11 which are all tied to ground.
// There can be multiple xhfc chips on the same SPI bus and this device ID
// is used for multiplexing. If we ever require more than one device then
// multiple device IDs should be defined to match the hardware config and thus
// used when creating the control byte to talk to different chips.
#define XHFC_DEVICE_ID 0x00




inline  void xhfc_PrintBuffer(
               unsigned char *ipBuffer,
                    unsigned  ivLength)
{
    unsigned lvIndex;

    printk(KERN_INFO);
    for(lvIndex = 0;
        lvIndex < ivLength;
        lvIndex++)
    {
        if(lvIndex && !(lvIndex % 8))
            printk("\n" KERN_INFO);
        printk("0x%2.2X, ", ipBuffer[lvIndex]);
    }
    printk("\n");
}


inline  unsigned  xhfc_reg_append_addr(
                       unsigned char *ipBuffer,
                            unsigned  ivAvailable,
                                __u8  reg_addr)
{
    if(ivAvailable < 2)
        return(0);

    ipBuffer[0] = SPI_WR | SPI_ADDR | SPI_NO_BROAD | XHFC_DEVICE_ID;
    ipBuffer[1] = reg_addr;


    return(2);
}


inline  unsigned  xhfc_reg_append_read(
                       unsigned char *ipBuffer,
                            unsigned  ivAvailable,
                                __u8  ivValue)
{
    if(ivAvailable < 2)
        return(0);

    ipBuffer[0] = SPI_RD | SPI_DATA | SPI_NO_MULTI;
    ipBuffer[1] = ivValue;


    return(2);
}


inline  unsigned  xhfc_reg_append_write(
                        unsigned char *ipBuffer,
                             unsigned  ivAvailable,
                                 __u8  ivValue)
{
    if(ivAvailable < 2)
        return(0);

    ipBuffer[0] = SPI_WR | SPI_DATA | SPI_NO_MULTI;
    ipBuffer[1] = ivValue;


    return(2);
}


inline  unsigned  xhfc_reg_append_read32(
                         unsigned char *ipBuffer,
                              unsigned  ivAvailable,
                                  __u8 *iaValue)
{
    if(ivAvailable < 5)
        return(0);

    ipBuffer[0] = SPI_RD | SPI_DATA | SPI_MULTI;
    ipBuffer[1] = iaValue[0];
    ipBuffer[2] = iaValue[1];
    ipBuffer[3] = iaValue[2];
    ipBuffer[4] = iaValue[3];


    return(5);
}


inline  unsigned  xhfc_reg_append_write32(
                          unsigned char *ipBuffer,
                               unsigned  ivAvailable,
                                   __u8 *iaValue)
{
    if(ivAvailable < 5)
        return(0);

    ipBuffer[0] = SPI_WR | SPI_DATA | SPI_MULTI;
    ipBuffer[1] = iaValue[0];
    ipBuffer[2] = iaValue[1];
    ipBuffer[3] = iaValue[2];
    ipBuffer[4] = iaValue[3];


    return(5);
}


inline  unsigned  xhfc_reg_build_read(
                       unsigned char *ipBuffer,
                            unsigned  ivAvailable,
                                __u8  reg_addr,
                                __u8  value)
{
    unsigned  lvOffset = 0;

    lvOffset += xhfc_reg_append_addr(ipBuffer + lvOffset,
                                     ivAvailable - lvOffset,
                                     reg_addr);

    lvOffset += xhfc_reg_append_read(ipBuffer + lvOffset,
                                     ivAvailable - lvOffset,
                                     value);

    return(lvOffset);
}


inline  unsigned  xhfc_reg_build_write(
                        unsigned char *ipBuffer,
                             unsigned  ivAvailable,
                                 __u8  reg_addr,
                                 __u8  value)
{
    unsigned  lvOffset = 0;

    lvOffset += xhfc_reg_append_addr(ipBuffer + lvOffset,
                                     ivAvailable - lvOffset,
                                     reg_addr);

    lvOffset += xhfc_reg_append_write(ipBuffer + lvOffset,
                                      ivAvailable - lvOffset,
                                      value);

    return(lvOffset);
}


inline  unsigned  xhfc_reg_build_read32(
                       unsigned char *ipBuffer,
                            unsigned  ivAvailable,
                                __u8  reg_addr,
                                __u8 *pValue)
{
    unsigned  lvOffset = 0;

    lvOffset += xhfc_reg_append_addr(ipBuffer + lvOffset,
                                     ivAvailable - lvOffset,
                                     reg_addr);

    lvOffset += xhfc_reg_append_read32(ipBuffer + lvOffset,
                                       ivAvailable - lvOffset,
                                       pValue);

    return(lvOffset);
}


inline  unsigned  xhfc_reg_build_write32(
                        unsigned char *ipBuffer,
                             unsigned  ivAvailable,
                                 __u8  reg_addr,
                                 __u8 *pValue)
{
    unsigned  lvOffset = 0;

    lvOffset += xhfc_reg_append_addr(ipBuffer + lvOffset,
                                     ivAvailable - lvOffset,
                                     reg_addr);

    lvOffset += xhfc_reg_append_write32(ipBuffer + lvOffset,
                                        ivAvailable - lvOffset,
                                        pValue);

    return(lvOffset);
}


int  xhfc_reg_read(
           xhfc_t *xhfc,
       __u8 const *reg_addr,
             __u8 *ipValues,
         unsigned  ivCount)
{
    unsigned  lvOffset;
    unsigned  lvIndex;
    unsigned char  lpBuffer[55];


    lvOffset = 1;
    for(lvIndex = 0;
        lvIndex < ivCount;
        lvIndex++)
    {
        unsigned  lvAppended;

        lvAppended = xhfc_reg_build_read(lpBuffer + lvOffset,
                                         sizeof(lpBuffer) - lvOffset,
                                         reg_addr[lvIndex],
                                         ipValues[lvIndex]);
        if(!lvAppended)
            return(-1);

        lvOffset += lvAppended;
    }

    lpBuffer[0] = lvOffset - 1;


    if(fpga_write_wait(xhfc->exp_card_device,
                       0,
                       lpBuffer,
                       lvOffset) < 0)
        return(-1);

    if(fpga_read(xhfc->exp_card_device,
                 0,
                 lpBuffer,
                 lvOffset) < 0)
        return(-1);


    for(lvIndex = 0;
        lvIndex < ivCount;
        lvIndex++)
    {
        ipValues[lvIndex] = lpBuffer[1 + (lvIndex * READ_BYTE_BUF_LEN) + DATA_START_BYTE];
    }


    return(0);
}


int  xhfc_reg_write(
            xhfc_t *xhfc,
        __u8 const *reg_addr,
              __u8 *ipValues,
          unsigned  ivCount)
{
    unsigned  lvOffset;
    unsigned  lvIndex;
    unsigned char  lpBuffer[55];


    lvOffset = 1;
    for(lvIndex = 0;
        lvIndex < ivCount;
        lvIndex++)
    {
        unsigned  lvAppended;

        lvAppended = xhfc_reg_build_write(lpBuffer + lvOffset,
                                          sizeof(lpBuffer) - lvOffset,
                                          reg_addr[lvIndex],
                                          ipValues[lvIndex]);
        if(!lvAppended)
            return(-1);

        lvOffset += lvAppended;
    }

    lpBuffer[0] = lvOffset - 1;


    if(fpga_write(xhfc->exp_card_device,
                  0,
                  lpBuffer,
                  lvOffset) < 0)
        return(-1);


    return(0);
}


int  xhfc_array_read(
             xhfc_t *xhfc,
         __u8 const  reg_addr,
               __u8 *ipValues,
           unsigned  ivCount)
/* returns the number of words atually read */
{
    unsigned  lvOffset;
    unsigned  lvIndex, lvI;
    unsigned char  lpBuffer[55];


    lvOffset = 1;
    lvIndex = 0;

    while(lvIndex < ivCount)
    {
        unsigned  lvAppended;

        lvAppended = xhfc_reg_build_read32(lpBuffer + lvOffset,
                                           sizeof(lpBuffer) - lvOffset,
                                           reg_addr,
                                           ipValues + (lvIndex * 4));
        if(!lvAppended)
            break;

        lvOffset += lvAppended;
        lvIndex += 1;
    }

    lpBuffer[0] = lvOffset - 1;


    if(fpga_write_wait(xhfc->exp_card_device,
                       0,
                       lpBuffer,
                       lvOffset) < 0)
        return(-1);

    if(fpga_read(xhfc->exp_card_device,
                 0,
                 lpBuffer,
                 lvOffset) < 0)
        return(-1);


    lvI = 0;
    while(lvI < lvIndex)
    {
        memcpy(ipValues +     (lvI * 4),
               lpBuffer + 1 + (lvI * 4) + DATA_START_BYTE,
               4);

        lvI += 1;
    }

    return(lvIndex);
}


int  xhfc_array_write(
              xhfc_t *xhfc,
          __u8 const  reg_addr,
                __u8 *ipValues,
            unsigned  ivCount)
/* returns the number of words atually written */
{
    unsigned  lvOffset;
    unsigned  lvIndex;
    unsigned char  lpBuffer[55];


    lvOffset = 1;
    lvIndex = 0;

    while(lvIndex < ivCount)
    {
        unsigned  lvAppended;

        lvAppended = xhfc_reg_build_write32(lpBuffer + lvOffset,
                                            sizeof(lpBuffer) - lvOffset,
                                            reg_addr,
                                            ipValues + (lvIndex * 4));
        if(!lvAppended)
            return(-1);

        lvOffset += lvAppended;
        lvIndex += 1;
    }

    lpBuffer[0] = lvOffset - 1;


    if(fpga_write(xhfc->exp_card_device,
                  0,
                  lpBuffer,
                  lvOffset) < 0)
        return(-1);


    return(lvIndex);
}


static void  xhfc_irq_callback(
  struct fpga_exp_card_device *exp_card_device)
{
    if(!exp_card_device)
        printk(KERN_WARNING "%s: No Device\n", __func__);
    else if(!exp_card_device->parent_exp_card)
        printk(KERN_WARNING "%s: Orphan Device\n", __func__);
    else if(!xhfc_array[exp_card_device->parent_exp_card->slot_number])
        printk(KERN_WARNING "%s: Device not registered\n",
            __func__);
    else
    {
        /* Call the handler */
        xhfc_su_irq_handler(xhfc_array[exp_card_device->parent_exp_card->slot_number]);
    }
}


static int xhfc_probe(struct device *dev)
{
    struct fpga_exp_card_device *exp_card_device;
    xhfc_t *xhfc;
    char wq_name[20];

    if(!dev)
        return(-1);

    exp_card_device = device_to_fpga_exp_card_device(dev);
    printk(KERN_INFO "%s called dev %p exp_card_device %p\n", __func__,
        dev, exp_card_device);
    if(!exp_card_device)
        return(-ENODEV);

    if(exp_card_device->device_type != DEV_COLOGNE)
    {
        printk(KERN_WARNING "%s called for Non XHFC device!\n", __FUNCTION__);
        return(-EINVAL);
    }

    if(!exp_card_device->parent_exp_card)
    {
        printk(KERN_WARNING "%s called for orphan device!\n",
               __func__);
        return(-ENOENT);
    }

    if(exp_card_device->parent_exp_card->slot_number > MAX_EXP_CARD_DEVICES)
    {
        printk(KERN_WARNING "%s: illegal slot number %d\n",
               __FUNCTION__,
               exp_card_device->parent_exp_card->slot_number);
        return(-EINVAL);
    }

    if(xhfc_array[exp_card_device->parent_exp_card->slot_number])
    {
        printk(KERN_WARNING
               "%s: already managing a device on slot %d\n",
               __func__, exp_card_device->parent_exp_card->slot_number);
        return(-1);
    }

    if(!get_device(dev))
        return(-ENODEV);

    xhfc = kzalloc(sizeof(xhfc_t), GFP_KERNEL);
    if(!xhfc)
    {
        printk(KERN_ERR "%s:%d No memory available\n", __func__,
               __LINE__);

        put_device(dev);
        return(-ENOMEM);
    }

    snprintf(xhfc->name, sizeof(xhfc->name), "xhfc.%d", exp_card_device->parent_exp_card->slot_number);
    snprintf(wq_name,
         sizeof(wq_name),
         "xhfc-wq(%d)", exp_card_device->parent_exp_card->slot_number);
    xhfc->workqueue = create_singlethread_workqueue(wq_name);

    INIT_WORK(&xhfc->bh_handler_workstructure,
              xhfc_bh_handler_workfunction);

    xhfc_array[exp_card_device->parent_exp_card->slot_number] = xhfc;
    xhfc->chipidx = exp_card_device->parent_exp_card->slot_number;
    xhfc->exp_card_device = exp_card_device;

    fpga_irq_enable(exp_card_device);

    if(xhfc_su_setup_instance(xhfc, dev))
    {
        printk(KERN_WARNING "%s: failed to set-up xhfc instance %s\n",
               __func__, xhfc->name);
        xhfc_array[exp_card_device->parent_exp_card->slot_number] = NULL;
        kfree(xhfc);
        put_device(dev);
        return(-EIO);
    }

    printk(KERN_INFO "%s called successfully for device in slot %d\n",
           __func__, exp_card_device->parent_exp_card->slot_number);

    return(0);
}


static int xhfc_remove(struct device *dev)
{
    struct fpga_exp_card_device *exp_card_device;
    xhfc_t *xhfc;

    if(!dev)
        return(-EINVAL);

    exp_card_device = device_to_fpga_exp_card_device(dev);
    if(!exp_card_device)
        return(-ENODEV);

    if(!exp_card_device->parent_exp_card)
    {
        printk(KERN_INFO "%s called for orphan device!\n",
               __func__);
        return(-ENODEV);
    }

    xhfc = xhfc_array[exp_card_device->parent_exp_card->slot_number];
    if(xhfc)
    {
        xhfc_array[exp_card_device->parent_exp_card->slot_number] = NULL;
        destroy_workqueue(xhfc->workqueue);
        kfree(xhfc);
    } else
        printk(KERN_WARNING "%s:%d No memory associated device\n",
               __func__, __LINE__);

    unblock_and_end_work(exp_card_device);

    printk(KERN_INFO "%s called successfully for device in slot %d\n",
           __func__, exp_card_device->parent_exp_card->slot_number);

    put_device(dev);

    return(0);
}

DECLARE_FPGA_HL_DEVICE_DRIVER(drv_xhfc,
                  "XHFC-2SU",
                  DEV_COLOGNE,
                  MAX_EXP_CARD_DEVICES,
                  &xhfc_irq_callback, &xhfc_probe, &xhfc_remove);


static int __init  xhfc_fpga_init(void)
{
    int err;

    printk(KERN_INFO DRIVER_NAME ": %s driver %s, %s %s\n",
           __FUNCTION__, xhfc_su_rev, __DATE__, __TIME__);

    memset(xhfc_array,
           0,
           sizeof(xhfc_array));

    err = fpga_register_device_driver(&drv_xhfc);

    printk(KERN_INFO "Registering to FPGA device driver returned %d\n", err);
    return(err);
}

static void __exit  xhfc_fpga_cleanup(void)
{
    printk(KERN_INFO DRIVER_NAME ": %s\n", __func__);

    fpga_unregister_device_driver(&drv_xhfc);
}

module_init(xhfc_fpga_init);
module_exit(xhfc_fpga_cleanup);


/* EOF */
