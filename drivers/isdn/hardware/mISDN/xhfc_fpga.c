
/*
 * Author: Brian J Forde
 */

#include <linux/module.h>
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




__u8  read_xhfc(
        xhfc_t *xhfc,
          __u8  reg_addr)
/*
 * This function reads a single byte from the xhfc device
 *
 * Two transactions required.
 *
 * 1st transaction - write the register value that we want to read.
 *
 * 2nd transaction - read the data. Control byte is written followed
 * by a read of one byte.
 *
 * Both transactions are done at the same time i.e. within the same
 * chip select.
 */
{
#ifdef XHFC_DEBUG
    unsigned  i;
#endif /* XHFC_DEBUG */
    unsigned char  Buffer[8];
    unsigned char *const buf = Buffer + 1;

#ifdef XHFC_DEBUG
    printk(KERN_INFO "%s 0x%02x\n", __FUNCTION__, reg_addr);
#endif /* XHFC_DEBUG */

    // Write the length into the buffer...
    buf[-1] = READ_BYTE_BUF_LEN;

    // Set-up the control byte to write to a single register address.
    buf[REG_CONTROL_BYTE] = SPI_WR | SPI_ADDR | SPI_NO_BROAD | XHFC_DEVICE_ID;

    // Set-up the register we want to read from.
    buf[REG_ADDR_BYTE] = reg_addr;

    // Now set-up the buffer with the new register value.
    buf[DATA_CONTORL_BYTE] = SPI_RD | SPI_DATA | SPI_NO_MULTI | XHFC_DEVICE_ID;

    buf[DATA_START_BYTE] = 0;


#ifdef XHFC_DEBUG
    printk(KERN_INFO "Sending Data: \n");
    // Only print the first bytes as the last byte is the buffer
    // for the return data.
    for(i = 0;
        i < READ_BYTE_BUF_LEN - 1;
        i++)
    {
        printk(KERN_INFO "Byte %d - 0x%02x\n", i + 1, buf[i]);
    }
#endif /* XHFC_DEBUG */

    // Now transmit the first 3 bytes in the buffer to the chip.
    // The 4th byte will be used to buffer the data returned from
    // the xhfc chip
    if(fpga_write_wait(xhfc->exp_card_device,
                       0,
                       Buffer,
                       READ_BYTE_BUF_LEN + 1) < 0)
        printk(KERN_WARNING "%s:%d: Oh Shite...\n", __FUNCTION__, __LINE__);
    else
    {
        if(fpga_read(xhfc->exp_card_device,
                     0,
                     Buffer,
                     8) < 0)
            printk(KERN_WARNING "%s:%d: fpga_read failed\n", __FUNCTION__, __LINE__);
    }


    return(buf[DATA_START_BYTE]);
}


void  write_xhfc(
         xhfc_t *xhfc,
           __u8  reg_addr,
           __u8  value)
/*
 * Two transactions required.
 *
 * 1st transaction - write the control byte followed by the
 * register address that we want to write to.
 * Example - reg address 0x13
 * 0100 0000 0001 0010
 * The first 8 bits are contol bits and the last 8 are the register
 * value.
 *
 * 2nd transaction - send the new register value with the
 * contol bits set to 0000 0000.
 *
 * Both transactions are done at the same time i.e. within the same
 * chip select.
 */
{
#ifdef XHFC_DEBUG
    unsigned  i;
#endif /* XHFC_DEBUG */
    unsigned char  Buffer[8];
    unsigned char *const buf = Buffer + 1;

#ifdef XHFC_DEBUG
    printk(KERN_INFO "%s reg:0x%02x val:0x%02x\n", __FUNCTION__, reg_addr, value);
#endif /* XHFC_DEBUG */

    // Write the length into the buffer...
    buf[-1] = WRITE_BYTE_BUF_LEN;

    // Set-up the control byte to write to a single register address.
    buf[REG_CONTROL_BYTE] = SPI_WR | SPI_ADDR | SPI_NO_BROAD | XHFC_DEVICE_ID;

    // Set the register value to write.
    buf[REG_ADDR_BYTE] = reg_addr;

    // Now set-up the buffer with the new register value.
    buf[DATA_CONTORL_BYTE] = SPI_WR | SPI_DATA | SPI_NO_MULTI | XHFC_DEVICE_ID;

    // Set the data to be written
    buf[DATA_START_BYTE] = value;

#ifdef XHFC_DEBUG
    printk(KERN_INFO "Sending Data:\n");
    // Print all data that will be sent to the xhfc chip.
    for(i = 0;
        i < WRITE_BYTE_BUF_LEN;
        i++)
    {
        printk(KERN_INFO "Byte %d - 0x%02x\n", i + 1, buf[i]);
    }
#endif /* XHFC_DEBUG */

    // Write the data
    if(fpga_write(xhfc->exp_card_device,
                  0,
                  Buffer,
                  WRITE_BYTE_BUF_LEN + 1) < 0)
        printk(KERN_WARNING "%s:%d: Oh Shite...\n", __FUNCTION__, __LINE__);


    return;
}

static void
xhfc_irq_callback(struct fpga_exp_card_device *exp_card_device)
{
	struct xhfc *xhfc;

	if (!exp_card_device)
		printk(KERN_WARNING "%s: No Device\n", __func__);
	else if (!exp_card_device->parent_exp_card)
		printk(KERN_WARNING "%s: Orphan Device\n", __func__);
	else if (!xhfc_array[exp_card_device->parent_exp_card->slot_number])
		printk(KERN_WARNING "%s: Device not registered\n",
			__func__);
	else {
		xhfc = xhfc_array[exp_card_device->parent_exp_card->slot_number];
		if (GET_V_GLOB_IRQ_EN(xhfc->irq_ctrl)
		    && (read_xhfc(xhfc, R_IRQ_OVIEW))) {
		        /* Call the handler */
			xhfc_su_irq_handler(xhfc);
		} else
			printk(KERN_WARNING "%s got spurious interrupt\n",
				xhfc->name);
	}
}

static int xhfc_probe(struct device *dev)
{
	struct fpga_exp_card_device *exp_card_device;
	xhfc_t *xhfc;
	char wq_name[20];

	exp_card_device = get_device_struct_ref_from_dev(dev);
	printk(KERN_INFO "%s called dev %p exp_card_device %p\n", __func__,
		dev, exp_card_device);
	if (!exp_card_device)
		return -1;

	if (exp_card_device->device_type != DEV_COLOGNE) {
		release_device_struct_ref(exp_card_device);
		return -1;
	}

	if (!exp_card_device->parent_exp_card) {
		printk(KERN_WARNING "%s called for orphan device!\n",
		       __func__);

		release_device_struct_ref(exp_card_device);
		return -1;
	}

	if (xhfc_array[exp_card_device->parent_exp_card->slot_number]) {
		printk(KERN_WARNING
		       "%s: already managing a device on slot %d\n",
		       __func__, exp_card_device->parent_exp_card->slot_number);

		release_device_struct_ref(exp_card_device);
		return -1;
	}

	xhfc = kzalloc(sizeof(xhfc_t), GFP_KERNEL);
	if (!xhfc) {
		printk(KERN_ERR "%s:%d No memory available\n", __func__,
		       __LINE__);

		release_device_struct_ref(exp_card_device);
		return -1;
	}

	snprintf(xhfc->name, sizeof(xhfc->name), "xhfc.%d", exp_card_device->parent_exp_card->slot_number);
	snprintf(wq_name,
		 sizeof(wq_name),
		 "xhfc-wq(%d)", exp_card_device->parent_exp_card->slot_number);
	xhfc->workqueue = create_singlethread_workqueue(wq_name);

	xhfc_array[exp_card_device->parent_exp_card->slot_number] = xhfc;
	xhfc->chipidx = exp_card_device->parent_exp_card->slot_number;
	xhfc->exp_card_device = exp_card_device;

	exp_card_device->irq_enable_reg = INT_ENABLE_REG1;
	exp_card_device->irq_enable_bit = 0xFF;
	fpga_irq_enable(exp_card_device);

	if (xhfc_su_setup_instance(xhfc, dev)) {
		printk(KERN_WARNING "%s: failed to set-up xhfc instance %s\n",
		       __func__, xhfc->name);
		xhfc_array[exp_card_device->parent_exp_card->slot_number] =
			NULL;
		kfree(xhfc);
		return -EIO;
	}

	printk(KERN_INFO "%s called successfully for device in slot %d\n",
	       __func__, exp_card_device->parent_exp_card->slot_number);

	return 0;
}

static int xhfc_remove(struct device *dev)
{
	struct fpga_exp_card_device *exp_card_device;
	xhfc_t *xhfc;

	if (!dev) {
		printk(KERN_INFO "%s called for no device!\n", __FUNCTION__);
		return -1;
	}

	exp_card_device = device_to_fpga_exp_card_device(dev);

	if (!exp_card_device->parent_exp_card) {
		printk(KERN_INFO "%s called for orphan device!\n",
		       __func__);
		return -1;
	}

	xhfc = xhfc_array[exp_card_device->parent_exp_card->slot_number];
	if (xhfc) {
		xhfc_array[exp_card_device->parent_exp_card->slot_number] =
		    NULL;
		destroy_workqueue(xhfc->workqueue);
		kfree(xhfc);
	} else
		printk(KERN_WARNING "%s:%d No memory associated device\n",
		       __func__, __LINE__);

	unblock_and_end_work(exp_card_device);
	release_device_struct_ref(exp_card_device);

	printk(KERN_INFO "%s called successfully for device in slot %d\n",
	       __func__, exp_card_device->parent_exp_card->slot_number);

	return 0;
}

DECLARE_FPGA_HL_DEVICE_DRIVER(drv_xhfc,
			      "XHFC-2SU",
			      DEV_COLOGNE,
			      MAX_EXP_CARD_DEVICES,
			      &xhfc_irq_callback, &xhfc_probe, &xhfc_remove);


static int __init
xhfc_fpga_init(void)
{
	int err;

	printk(KERN_INFO DRIVER_NAME " driver Rev. %s\n",
	       xhfc_su_rev);

	err = fpga_register_device_driver(&drv_xhfc);

	printk(KERN_INFO "Register of FPGA device driver return %d\n", err);
	return err;
}

static void __exit
xhfc_fpga_cleanup(void)
{
	printk(KERN_INFO DRIVER_NAME ": %s\n", __func__);

	fpga_unregister_device_driver(&drv_xhfc);
}

module_init(xhfc_fpga_init);
module_exit(xhfc_fpga_cleanup);


/* EOF */
