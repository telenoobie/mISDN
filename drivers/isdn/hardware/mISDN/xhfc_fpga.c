
/*
 * Author: Brian J Forde
 */

#include <linux/module.h>
#include "xhfc_fpga.h"
#include "xhfc_mgr.h"
#include "xhfc_su.h"
#include "xhfc_sync.h"

#include "fpgaDriverEx/drv_fpga_common.h"
#include "fpgaDriverEx/drv_fpga_hl_device_driver.h"
#include "fpgaDriverEx/drv_fpga_registration.h"
#include "fpgaDriverEx/drv_fpga_irq.h"
#include "fpgaDriverEx/drv_fpga_irq_defs.h"
#include "fpgaDriverEx/drv_fpga_exp_card.h"
#include "fpgaDriverEx/drv_fpga_exp_card_device.h"
#include "fpgaDriverEx/drv_fpga_readwrite.h"

/* This lock has to be taken all time you access xhfc_array */
static struct semaphore	 xhfc_array_lock;
struct xhfc *xhfc_array[MAX_NUMBER_OF_SLOTS];


//#define XHFC_DEBUG

int xhfc_fpga_get_slot_number(struct fpga_exp_card_device *fecd)
{
	return fecd->parent_exp_card->slot_number;
}

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

// Task_10668 - PRI clock Sync
int alternative_clock_source(unsigned status)
{
    int slot_num, i, ret = 0;
    struct xhfc *xhfc;
    int lvPortsUp = 0, portsSync;

    for (slot_num = 0; slot_num < MAX_NUMBER_OF_SLOTS; slot_num++)
    {
        down_interruptible(&xhfc_array_lock);
        xhfc = xhfc_array[slot_num];
        if (!xhfc) {
            up(&xhfc_array_lock);
            continue;
        }

        if (status) {
            for (i = 0; i < xhfc->num_ports; i++) {
                if((xhfc->port[i].mode & PORT_MODE_TE) && ((xhfc->port[i].dch.state == 6) || (xhfc->port[i].dch.state == 7)))
                    lvPortsUp += 1;
            }
            if (lvPortsUp) {
                printk(KERN_DEBUG "%s: Start sync with BRI port in slot %d. lvPortsUp %d\n", __func__, slot_num, lvPortsUp);
                portsSync = 0;
                xmStateChange(xhfc, lvPortsUp, portsSync);
                ret = 1;

            }
            else
                printk(KERN_DEBUG "%s: No BRI ports available to Sync\n", __func__);
        }
        else {
            lvPortsUp = 0;
            portsSync = 1;
            printk(KERN_DEBUG "%s: Stop sync with BRI port in slot %d\n", __func__, slot_num);
            xmStateChange(xhfc, lvPortsUp, portsSync);
        }
        up(&xhfc_array_lock);
    }
    return ret;
}
EXPORT_SYMBOL(alternative_clock_source);
//\ Task_10668

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
           struct xhfc *xhfc,
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


    if(fpga_write_wait(xhfc->hw,
                       0,
                       lpBuffer,
                       lvOffset) < 0)
        return(-1);

    if(fpga_read(xhfc->hw,
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
            struct xhfc *xhfc,
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


    if(fpga_write(xhfc->hw,
                  0,
                  lpBuffer,
                  lvOffset) < 0)
        return(-1);


    return(0);
}


int  xhfc_array_read(
             struct xhfc *xhfc,
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


    if(fpga_write_wait(xhfc->hw,
                       0,
                       lpBuffer,
                       lvOffset) < 0)
        return(-1);

    if(fpga_read(xhfc->hw,
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
              struct xhfc *xhfc,
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


    if(fpga_write(xhfc->hw,
                  0,
                  lpBuffer,
                  lvOffset) < 0)
        return(-1);


    return(lvIndex);
}


static void  xhfc_irq_callback(
  struct fpga_exp_card_device *exp_card_device)
{
    struct xhfc *xhfc;

    if(!exp_card_device)
        printk(KERN_WARNING "%s: No Device\n", __func__);
    else if(!exp_card_device->parent_exp_card)
        printk(KERN_WARNING "%s: Orphan Device\n", __func__);
    else {
        down_interruptible(&xhfc_array_lock);
        xhfc = xhfc_array[exp_card_device->parent_exp_card->slot_number];
        if (!xhfc) {
            up(&xhfc_array_lock);
            printk(KERN_WARNING "%s: Device slot %d not registered\n",
                __func__, exp_card_device->parent_exp_card->slot_number);
        } else {
            xhfc_lock(xhfc);
            up(&xhfc_array_lock);
            /* Call the handler */
            xhfc_su_irq_handler(xhfc);
            xhfc_unlock(xhfc);
        }
    }
}


static int xhfc_probe(struct device *dev)
{
    struct fpga_exp_card_device *exp_card_device;
    struct xhfc *xhfc;
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

    if(exp_card_device->parent_exp_card->slot_number > MAX_NUMBER_OF_SLOTS)
    {
        printk(KERN_WARNING "%s: illegal slot number %d\n",
               __FUNCTION__,
               exp_card_device->parent_exp_card->slot_number);
        return(-EINVAL);
    }
    down_interruptible(&xhfc_array_lock);
    if(xhfc_array[exp_card_device->parent_exp_card->slot_number])
    {
        up(&xhfc_array_lock);
        printk(KERN_WARNING
               "%s: already managing a device on slot %d\n",
               __func__, exp_card_device->parent_exp_card->slot_number);
        return(-1);
    }

    if(!get_device(dev)) {
        up(&xhfc_array_lock);
        return(-ENODEV);
    }

    xhfc = kzalloc(sizeof(struct xhfc), GFP_KERNEL);
    if(!xhfc)
    {
        printk(KERN_ERR "%s:%d No memory available\n", __func__,
               __LINE__);
        up(&xhfc_array_lock);
        put_device(dev);
        return(-ENOMEM);
    }

    snprintf(xhfc->name, sizeof(xhfc->name), "xhfc_%d", exp_card_device->parent_exp_card->slot_number);
    snprintf(wq_name,
         sizeof(wq_name),
         "xhfc-wq(%d)", exp_card_device->parent_exp_card->slot_number);
    xhfc->workqueue = create_singlethread_workqueue(wq_name);

    INIT_WORK(&xhfc->bh_handler_workstructure,
              xhfc_bh_handler_workfunction);

    xhfc_array[exp_card_device->parent_exp_card->slot_number] = xhfc;
    up(&xhfc_array_lock);
    xhfc->chipidx = exp_card_device->parent_exp_card->slot_number;
    xhfc->hw = exp_card_device;

    fpga_irq_enable(exp_card_device);

    if(xhfc_su_setup_instance(xhfc, dev))
    {
        printk(KERN_WARNING "%s: failed to set-up xhfc instance %s\n",
               __func__, xhfc->name);
        down_interruptible(&xhfc_array_lock);
        xhfc_array[exp_card_device->parent_exp_card->slot_number] = NULL;
        up(&xhfc_array_lock);
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
    struct xhfc *xhfc;

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
    down_interruptible(&xhfc_array_lock);
    xhfc = xhfc_array[exp_card_device->parent_exp_card->slot_number];
    if(xhfc)
    {
        xhfc_array[exp_card_device->parent_exp_card->slot_number] = NULL;
        up(&xhfc_array_lock);
        destroy_workqueue(xhfc->workqueue);
        kfree(xhfc);
    } else {
        up(&xhfc_array_lock);
        printk(KERN_WARNING "%s:%d No memory associated device\n",
               __func__, __LINE__);
    }

    unblock_and_end_work(exp_card_device);

    printk(KERN_INFO "%s called successfully for device in slot %d\n",
           __func__, exp_card_device->parent_exp_card->slot_number);

    put_device(dev);

    return(0);
}

DECLARE_FPGA_HL_DEVICE_DRIVER(drv_xhfc,
                  "XHFC-2SU",
                  DEV_COLOGNE,
                  MAX_NUMBER_OF_SLOTS,
                  &xhfc_irq_callback, &xhfc_probe, &xhfc_remove);



static struct fpga_exp_card_device *xhfc_GetBeta(
                                    struct xhfc *xhfc)
{
    struct fpga_exp_card_device *exp_card_device;


    if(!xhfc)
        return(NULL);


    exp_card_device = xhfc->hw;

    if(!exp_card_device->parent_exp_card ||
       !exp_card_device->parent_exp_card->devices ||
       !exp_card_device->parent_exp_card->devices[FPGA_DEV_ID])
    {
        printk(KERN_WARNING "%s device has no Sibling!\n",
               __func__);
        return(NULL);
    }

    if(exp_card_device->parent_exp_card->devices[FPGA_DEV_ID]->device_type != DEV_EXP_FPGA)
    {
        printk(KERN_WARNING "%s sibling not a Beta Device!!!!\n",
               __func__);
        return(NULL);
    }

    return(exp_card_device->parent_exp_card->devices[FPGA_DEV_ID]);
}


static void  xhfc_NTR_Write(
               struct xhfc *xhfc,
                  unsigned  ivEnable)
{
    struct fpga_exp_card_device *beta_device;
    unsigned char  lpBuffer[55];
    int  res;


    if(!(beta_device = xhfc_GetBeta(xhfc)))
        return;


    printk(KERN_DEBUG "%s(%d, %d)\n",
           __func__,
           beta_device->parent_exp_card->slot_number,
           ivEnable);


    lpBuffer[0] = 2;
    lpBuffer[1] = 0x11;
    lpBuffer[2] = ivEnable ? 0x11 : 0;

    res = fpga_write(beta_device,
                     0,
                     lpBuffer,
                     3);

    return;
}

void  xhfc_NTR_Start(
              struct xhfc *xhfc)
{
    xhfc_NTR_Write(xhfc, 1);
    return;
}

void  xhfc_NTR_Stop(
             struct xhfc *xhfc)
{
    xhfc_NTR_Write(xhfc, 0);
    return;
}
/*
static int bri_beta_probe(
           struct device *dev)
{
    struct fpga_exp_card_device *exp_card_device;


    if(!dev)
        return(-ENODEV);

    exp_card_device = device_to_fpga_exp_card_device(dev);
    printk(KERN_INFO "%s called dev %p exp_card_device %p\n",
           __func__,
           dev,
           exp_card_device);
    if(!exp_card_device)
        return(-ENODEV);

    if(exp_card_device->device_type != DEV_EXP_FPGA)
    {
        printk(KERN_WARNING "%s called for Non Beta device!\n",
               __func__);
        return(-EINVAL);
    }

    if(!exp_card_device->parent_exp_card)
    {
        printk(KERN_WARNING "%s called for orphan device!\n",
               __func__);
        return(-ENOENT);
    }

    if(!exp_card_device->parent_exp_card->devices ||
       !exp_card_device->parent_exp_card->devices[COLOGNE_DEV_ID])
    {
        printk(KERN_WARNING "%s device has no Sibling!\n",
               __func__);
        return(-EINVAL);
    }


    if(exp_card_device->parent_exp_card->devices[COLOGNE_DEV_ID]->device_type != DEV_COLOGNE)
        return(-ENODEV);


    if(exp_card_device->parent_exp_card->slot_number > MAX_NUMBER_OF_SLOTS)
    {
        printk(KERN_WARNING "%s: illegal slot number %d\n",
               __FUNCTION__,
               exp_card_device->parent_exp_card->slot_number);
        return(-EINVAL);
    }


    if(!get_device(dev))
        return(-ENODEV);


    printk(KERN_INFO "%s called successfully for device in slot %d\n",
           __func__,
           exp_card_device->parent_exp_card->slot_number);


    return(0);
}

static int bri_beta_remove(
            struct device *dev)
{
    struct fpga_exp_card_device *exp_card_device;

    printk(KERN_INFO "%s\n",
           __func__);

    if(!dev)
        return(-EINVAL);

    exp_card_device = device_to_fpga_exp_card_device(dev);
    if(!exp_card_device)
        return(-ENODEV);

    return(0);
}

DECLARE_FPGA_HL_DEVICE_DRIVER(drv_BRI_Beta,
                             "BRI-BETA",
                              DEV_EXP_FPGA,
                              MAX_NUMBER_OF_SLOTS,
                              NULL,
                             &bri_beta_probe,
                             &bri_beta_remove);
 */

static int __init  xhfc_fpga_init(void)
{
    int err;

    printk(KERN_INFO DRIVER_NAME ": %s driver %s, %s %s\n",
           __FUNCTION__, xhfc_su_rev, __DATE__, __TIME__);

    xmInitialise();

    sema_init(&xhfc_array_lock, 1);

    if((err = fpga_register_device_driver(&drv_xhfc)) < 0)
    {
        printk(KERN_WARNING "%s registration failed. Error %d\n",
               __func__,
               err);
        return(err);
    }
/*
    if((err = fpga_register_device_driver(&drv_BRI_Beta)) < 0)
    {
        printk(KERN_WARNING "%s registration failed. Error %d\n",
               __func__,
               err);
        return(err);
    }
 */
    printk(KERN_INFO "Registering to FPGA device driver returned %d\n", err);
    return(err);
}

static void __exit  xhfc_fpga_cleanup(void)
{
    printk(KERN_INFO DRIVER_NAME ": %s\n", __func__);
/*
    fpga_unregister_device_driver(&drv_BRI_Beta);
 */
    fpga_unregister_device_driver(&drv_xhfc);
}

module_init(xhfc_fpga_init);
module_exit(xhfc_fpga_cleanup);


/* EOF */
