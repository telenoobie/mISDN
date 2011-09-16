
/*
 * Author: Brian J Forde
 */

#include <linux/module.h>
#include "xhfc_fpga.h"
#include "xhfc_su.h"

#include "fpgaDriverEx/drv_fpga_common.h"
#include "fpgaDriverEx/drv_fpga_hl_device_driver.h"
#include "fpgaDriverEx/drv_fpga_registration.h"
#include "fpgaDriverEx/drv_fpga_irq.h"
#include "fpgaDriverEx/drv_fpga_irq_defs.h"
#include "fpgaDriverEx/drv_fpga_exp_card.h"
#include "fpgaDriverEx/drv_fpga_exp_card_device.h"
#include "fpgaDriverEx/drv_fpga_readwrite.h"
#include "fpgaDriverEx/fpga_access.h"



#define NUM_SLOTS  16

static unsigned  gvSyncSlot;
static unsigned  gvActive[NUM_SLOTS];





#define PDM_DAC_1 0x4B
#define PDM_DAC_2 0x4C
#define PDM_DAC_3 0x4D

/*
The normal sequence of events will be;
1.	After reset/power-up the 10-bit DAC is fixed at the centre code of 0x200
2.	When software is informed that a valid NTR signal is available, it selects
    a card to drive the NTR lines.
3.	Software un-freezes" the control loop by writing 0x0000000 to the "PDM DAC
    Setting 1" register at address 0x004b. The loop is now in frequency tracking mode.
4.	When software is informed that the chosen NTR signal is no longer available,
    it must put the loop back in to "Freeze" state by writing 0x0010000 to the
   "PDM DAC Setting 1" register at address 0x004b.  If this write occurs within
    4ms of the loss of the network clock then the loop cannot drift by more 1ppm
    of the output frequency even at the maximum adjustment rate.
5.	The control then resumes at step (2).
*/

inline void  fpgaClockReSet(
                  unsigned  ivSlot)
/*
  freezes the control loop
 */
{
    __fpga_writel(1 << 16,
                  PDM_DAC_1);

    printk(KERN_DEBUG "%s(%u)\n",
           __func__,
           ivSlot);

    return;
}


inline void  fpgaClockSet(
                unsigned  ivSlot)
/*
  un-freezes the control loop
 */
{
    __fpga_writel(0,
                  PDM_DAC_1);

    printk(KERN_DEBUG "%s(%u)\n",
           __func__,
           ivSlot);

    return;
}


void  xmStateChange(
 const struct xhfc *xhfc,
    const unsigned  ivPortsUp)
{
    unsigned  ivSlot;
    unsigned  lvSlot;

    BUG_ON(!xhfc ||
           !xhfc->hw ||
           !xhfc->hw->parent_exp_card);

    ivSlot = xhfc->hw->parent_exp_card->slot_number;

    BUG_ON(ivSlot >= NUM_SLOTS);


    printk(KERN_DEBUG "%s(%u, %u)\n",
           __func__,
           ivSlot, ivPortsUp);


    lvSlot = ivSlot;
    gvActive[ivSlot] = ivPortsUp;


    if((gvSyncSlot == ivSlot) && !ivPortsUp)
    /* our clock source has gone... */
    {
        fpgaClockReSet(gvSyncSlot);
        xhfc_NTR_Stop(xhfc);
        gvSyncSlot = ~0;

        do
        /* ...so search for an alternative */
        {
            lvSlot = (lvSlot + 1) % NUM_SLOTS;
            if(gvActive[lvSlot])
                break;
        }
        while(lvSlot != ivSlot);
    }


    if((gvSyncSlot == ~0) && gvActive[lvSlot])
    /* Not currently Synced, and clock available, so Sync */
    {
        xhfc_NTR_Start(xhfc_array[lvSlot]);
        fpgaClockSet((gvSyncSlot = lvSlot));
    }


    return;
}


void  xmInitialise(void)
{
    memset(gvActive,
           sizeof(gvActive),
           0);

    gvSyncSlot = ~0;


    return;
}

/* EOF */
