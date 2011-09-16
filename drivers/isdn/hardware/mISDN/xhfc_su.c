/* xhfc_su.c
 * mISDN driver for Cologne Chip AG's XHFC
 *
 * (c) 2007,2008 Copyright Cologne Chip AG
 * Authors : Martin Bachem, Joerg Ciesielski
 * Contact : info@colognechip.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * module params
 *   debug=<n>, default=0, with n=0xHHHHGGGG
 *      H - l1 driver flags described in hfcs_usb.h
 *      G - common mISDN debug flags described at mISDNhw.h
 */
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/mISDNhw.h>
#include <asm/semaphore.h>
#include "xhfc_su.h"

#ifdef CONFIG_MISDN_XHFC_PCI2PI
#include <linux/pci.h>
#include "xhfc_pci2pi.h"
#endif

#ifdef CONFIG_MISDN_XHFC_FPGA
#include "xhfc_fpga.h"
#endif

const char *xhfc_su_rev = "Revision: 0.1.3 (socket), 2010-05-07";

/* modules params */
static unsigned int debug = 0;

/* driver globbls */
static int xhfc_cnt;

static int pcm = -1;

#ifdef MODULE
MODULE_AUTHOR("Martin Bachem");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif
module_param(debug, uint, S_IRUGO | S_IWUSR);
module_param(pcm, uint, S_IRUGO | S_IWUSR);
#endif

/* prototypes for static functions */
static int xhfc_l2l1B(struct mISDNchannel *ch, struct sk_buff *skb);
static int xhfc_l2l1D(struct mISDNchannel *ch, struct sk_buff *skb);
static int xhfc_bctrl(struct mISDNchannel *ch, u_int cmd, void *arg);
static int xhfc_dctrl(struct mISDNchannel *ch, u_int cmd, void *arg);
static int xhfc_setup_bch(struct bchannel *bch, int protocol);
static void xhfc_setup_dch(struct dchannel *dch);
static void xhfc_bh_handler(struct xhfc *);
static void xhfc_ph_state(struct dchannel *dch);
static void xhfc_f7_timer_expire(struct port *port);
/*
 * Physical S/U commands to control Line Interface
 */
const char *XHFC_PH_COMMANDS[] =
{
    "L1_ACTIVATE_TE",
    "L1_FORCE_DEACTIVATE_TE",
    "L1_ACTIVATE_NT",
    "L1_DEACTIVATE_NT",
    "L1_TESTLOOP_B1",
    "L1_TESTLOOP_B2",
    "L1_TESTLOOP_D",
    "L1_TESTLOOP_OFF"
};


static inline void  xhfc_waitbusy(
                     struct xhfc *xhfc)
{
    while(xhfc_read(xhfc, R_STATUS) & M_BUSY);
}


static inline void  xhfc_selfifo(
                    struct xhfc *xhfc,
                           __u8  fifo)
{
    xhfc_write(xhfc, R_FIFO, fifo);
    xhfc_waitbusy(xhfc);
}


static inline void  xhfc_inc_f(
                  struct xhfc *xhfc)
{
    xhfc_write(xhfc, A_INC_RES_FIFO, M_INC_F);
    xhfc_waitbusy(xhfc);
}


static inline void  xhfc_resetfifo(
                      struct xhfc *xhfc)
{
    xhfc_write(xhfc, A_INC_RES_FIFO, M_RES_FIFO | M_RES_FIFO_ERR);
    xhfc_waitbusy(xhfc);
}


void  xhfc_disable_interrupts(
                 struct xhfc *xhfc)
/*
 * disable all interrupts by disabling M_GLOB_IRQ_EN
 */
{
    if(debug & DEBUG_HW)
        printk(KERN_INFO "%s %s\n", xhfc->name, __FUNCTION__);
    SET_V_GLOB_IRQ_EN(xhfc->irq_ctrl, 0);
    xhfc_write(xhfc, R_IRQ_CTRL, xhfc->irq_ctrl);

    return;
}


void  xhfc_enable_interrupts(
                struct xhfc *xhfc)
/*
 * start interrupt and set interrupt mask
 */
{
    if(debug & DEBUG_HW)
        printk(KERN_INFO "%s %s\n", xhfc->name, __FUNCTION__);

    xhfc_write(xhfc, R_SU_IRQMSK, xhfc->su_irqmsk);

    /* use defined timer interval */
    xhfc_write(xhfc, R_TI_WD, xhfc->ti_wd);

    /* enable global interrupts */
    SET_V_GLOB_IRQ_EN(xhfc->irq_ctrl, 1);
    SET_V_FIFO_IRQ_EN(xhfc->irq_ctrl, 1);
    xhfc_write(xhfc, R_IRQ_CTRL, xhfc->irq_ctrl);

    return;
}


static void  xhfc_setup_su(
              struct xhfc *xhfc,
                     __u8  pt,
                     __u8  bc,
                     __u8  enable)
/*
 * Setup S/U interface, enable/disable B-Channels
 */
{
    struct port *port = &xhfc->port[pt];

    if(!((bc == 0) || (bc == 1)))
    {
        printk(KERN_INFO "%s %s: pt(%i) ERROR: bc(%i) unvalid!\n",
               xhfc->name, __FUNCTION__, pt, bc);
        return;
    }

    if(debug & DEBUG_HW)
        printk(KERN_INFO "%s %s %s pt(%i) bc(%i)\n",
               xhfc->name, __FUNCTION__,
               (enable) ? ("enable") : ("disable"), pt, bc);

    if(bc)
    {
        SET_V_B2_RX_EN(port->su_ctrl2, (enable ? 1 : 0));
        SET_V_B2_TX_EN(port->su_ctrl0, (enable ? 1 : 0));
    }
    else
    {
        SET_V_B1_RX_EN(port->su_ctrl2, (enable ? 1 : 0));
        SET_V_B1_TX_EN(port->su_ctrl0, (enable ? 1 : 0));
    }

    if(xhfc->port[pt].mode & PORT_MODE_NT)
        SET_V_SU_MD(xhfc->port[pt].su_ctrl0, 1);

    xhfc_write(xhfc, R_SU_SEL, pt);
    xhfc_write(xhfc, A_SU_CTRL0, xhfc->port[pt].su_ctrl0);
    xhfc_write(xhfc, A_SU_CTRL2, xhfc->port[pt].su_ctrl2);
}


static void  init_su(
        struct port *port)
/*
 * setup port (line interface) with SU_CRTLx
 */
{
    struct xhfc *xhfc = port->xhfc;

    if(debug & DEBUG_HW)
        printk(KERN_INFO "%s %s port(%i)\n", xhfc->name,
               __FUNCTION__, port->idx);

    xhfc_write(xhfc, R_SU_SEL, port->idx);

    if(port->mode & PORT_MODE_NT)
        SET_V_SU_MD(port->su_ctrl0, 1);

    if(port->mode & PORT_MODE_EXCH_POL)
        port->su_ctrl2 = M_SU_EXCHG;

    if(port->mode & PORT_MODE_UP0)
    {
        SET_V_ST_SEL(port->st_ctrl3, 1);
        xhfc_write(xhfc, A_MS_TX, 0x0F);
        SET_V_ST_SQ_EN(port->su_ctrl0, 1);
    }

    /* configure end of pulse control for ST mode (TE & NT) */
    if(port->mode & PORT_MODE_S0)
    {
        SET_V_ST_PU_CTRL(port->su_ctrl0, 1);
        port->st_ctrl3 = 0xf8;
    }

    if(debug & DEBUG_HW)
        printk(KERN_INFO "%s %s su_ctrl0(0x%02x) "
               "su_ctrl1(0x%02x) "
               "su_ctrl2(0x%02x) "
               "st_ctrl3(0x%02x)\n",
               xhfc->name, __FUNCTION__,
               port->su_ctrl0,
               port->su_ctrl1, port->su_ctrl2, port->st_ctrl3);

    xhfc_write(xhfc, A_ST_CTRL3, port->st_ctrl3);
    xhfc_write(xhfc, A_SU_CTRL0, port->su_ctrl0);
    xhfc_write(xhfc, A_SU_CTRL1, port->su_ctrl1);
    xhfc_write(xhfc, A_SU_CTRL2, port->su_ctrl2);

    if(port->mode & PORT_MODE_TE)
        xhfc_write(xhfc, A_SU_CLK_DLY, CLK_DLY_TE);
    else
        xhfc_write(xhfc, A_SU_CLK_DLY, CLK_DLY_NT);

    xhfc_write(xhfc, A_SU_WR_STA, 0);
}


static void  xhfc_setup_fifo(
                struct xhfc *xhfc,
                       __u8  fifo,
                       __u8  conhdlc,
                       __u8  subcfg,
                       __u8  fifoctrl,
                       __u8  enable)
/*
 * Setup Fifo using A_CON_HDLC, A_SUBCH_CFG, A_FIFO_CTRL
 */
{
    xhfc_selfifo(xhfc, fifo);
    xhfc_write(xhfc, A_CON_HDLC, conhdlc);
    xhfc_write(xhfc, A_SUBCH_CFG, subcfg);
    xhfc_write(xhfc, A_FIFO_CTRL, fifoctrl);

    if(enable)
        xhfc->fifo_irqmsk |= (1 << fifo);
    else
        xhfc->fifo_irqmsk &= ~(1 << fifo);

    xhfc_resetfifo(xhfc);
    xhfc_selfifo(xhfc, fifo);

    if(debug & DEBUG_HW)
    {
        printk(KERN_INFO
               "%s %s: fifo(%i) conhdlc(0x%02x) "
               "subcfg(0x%02x) fifoctrl(0x%02x)\n",
               xhfc->name, __FUNCTION__, fifo,
               xhfc_sread(xhfc, A_CON_HDLC),
               xhfc_sread(xhfc, A_SUBCH_CFG),
               xhfc_sread(xhfc, A_FIFO_CTRL)
            );
    }
}


static int  xhfc_set_pcm_tx_rx(
              struct bchannel *bch,
                          int  tx_slot,
                          int  rx_slot)
{
    struct port *port = bch->hw;
    struct xhfc *xhfc = port->xhfc;
    __u8 ch_idx = (port->idx * 4) + (bch->nr - 1);
    __u8 r_slot = 0;
    __u8 a_sl_cfg = 0;

    printk(KERN_INFO "%s: B%d set timeslots %d/%d --> %d/%d\n",
        __func__, bch->nr, bch->pcm_tx, bch->pcm_rx, tx_slot, rx_slot);
    if(tx_slot != MISDN_PCM_SLOT_IGNORE && tx_slot != bch->pcm_tx)
    {
        // If there is already a slot transmitting, disable it.
        if(bch->pcm_tx != MISDN_PCM_SLOT_DISABLE)
        {
            // Disable TX slot.
            SET_V_SL_DIR(r_slot, 0);    /* TX */
            SET_V_SL_NUM(r_slot, bch->pcm_tx);
            xhfc_write(xhfc, R_SLOT, r_slot);

            SET_V_CH_SDIR(a_sl_cfg, 0);
            SET_V_CH_SNUM(a_sl_cfg, ch_idx);
            SET_V_ROUT(a_sl_cfg, 0x00);    /* Disable TX timeslot */
            xhfc_write(xhfc, A_SL_CFG, a_sl_cfg);

            r_slot = 0;
            a_sl_cfg = 0;
        }
        if(tx_slot != MISDN_PCM_SLOT_DISABLE)
        {
            /* Setup TX timeslot */
            SET_V_SL_DIR(r_slot, 0);    /* TX */
            SET_V_SL_NUM(r_slot, tx_slot);
            xhfc_write(xhfc, R_SLOT, r_slot);

            SET_V_CH_SDIR(a_sl_cfg, 0);
            SET_V_CH_SNUM(a_sl_cfg, ch_idx);
            SET_V_ROUT(a_sl_cfg, 0x03);    /* TX */
            xhfc_write(xhfc, A_SL_CFG, a_sl_cfg);
        }
        bch->pcm_tx = tx_slot;
    }
    r_slot = 0;
    a_sl_cfg = 0;
    if(rx_slot != MISDN_PCM_SLOT_IGNORE && rx_slot != bch->pcm_rx)
    {
        // If there is already a active slot, disable it.
        if(bch->pcm_rx != MISDN_PCM_SLOT_DISABLE)
        {
            // Disable RX slot
            SET_V_SL_DIR(r_slot, 1);    /* RX */
            SET_V_SL_NUM(r_slot, bch->pcm_rx);
            xhfc_write(xhfc, R_SLOT, r_slot);

            SET_V_CH_SDIR(a_sl_cfg, 1);
            SET_V_CH_SNUM(a_sl_cfg, ch_idx);
            SET_V_ROUT(a_sl_cfg, 0x00);    /* Diable Rx timeslot */
            xhfc_write(xhfc, A_SL_CFG, a_sl_cfg);

            r_slot = 0;
            a_sl_cfg = 0;
        }

        /* Setup RX timeslot */
        SET_V_SL_DIR(r_slot, 1);    /* RX */
        SET_V_SL_NUM(r_slot, rx_slot);
        xhfc_write(xhfc, R_SLOT, r_slot);

        SET_V_CH_SDIR(a_sl_cfg, 1);
        SET_V_CH_SNUM(a_sl_cfg, ch_idx);
        SET_V_ROUT(a_sl_cfg, 0x03);    /* RX */
        xhfc_write(xhfc, A_SL_CFG, a_sl_cfg);
        if(rx_slot != MISDN_PCM_SLOT_DISABLE)
        {
            /* Setup RX timeslot */
            SET_V_SL_DIR(r_slot, 1);    /* RX */
            SET_V_SL_NUM(r_slot, rx_slot);
            xhfc_write(xhfc, R_SLOT, r_slot);

            SET_V_CH_SDIR(a_sl_cfg, 1);
            SET_V_CH_SNUM(a_sl_cfg, ch_idx);
            SET_V_ROUT(a_sl_cfg, 0x03);    /* RX */
            xhfc_write(xhfc, A_SL_CFG, a_sl_cfg);
        }
        bch->pcm_rx = rx_slot;
    }
    return 0;
}


static void xhfc_pcm_ch_setup(
                      xhfc_t *xhfc,
                        __u8  channel)
{
    __u8 a_con_hdlc = 0;

    /* SET Transparent mode */
    SET_V_HDLC_TRP(a_con_hdlc, 1);
    /* SET FIFO ON (mandatory) - IRQ off */
    SET_V_FIFO_IRQ(a_con_hdlc, 7);
    /* Connect ST/Up straight to PCM */
    SET_V_DATA_FLOW(a_con_hdlc, 0x6);
    /* B-TX Fifo */
    xhfc_setup_fifo(xhfc, (channel * 2), a_con_hdlc, 0, 0, 0);
    /* B-RX Fifo */
    xhfc_setup_fifo(xhfc, (channel * 2) + 1, a_con_hdlc, 0, 0, 0);
}


static int  xhfc_setup_bch(
          struct bchannel *bch,
                      int  protocol)
/*
 * init BChannel prototol
 */
{
    struct port *port = bch->hw;
    struct xhfc *xhfc = port->xhfc;
    __u8 channel = (port->idx * 4) + (bch->nr - 1);

    if(debug & DEBUG_HW)
        printk(KERN_INFO "channel(%i) protocol %x-->%x\n",
               channel, bch->state, protocol);

    switch(protocol)
    {
        case -1:
            bch->state = -1;
            /* fall trough */
        case ISDN_P_NONE:
            if(bch->state == ISDN_P_NONE)
                return 0;
            bch->state = ISDN_P_NONE;
            xhfc_setup_fifo(xhfc, (channel * 2), 4, 0, 0, 0);
            xhfc_setup_fifo(xhfc, (channel * 2) + 1, 4, 0, 0, 0);
            xhfc_setup_su(xhfc, port->idx, (channel % 4) ? 1 : 0,
                 0);
            clear_bit(FLG_HDLC, &bch->Flags);
            clear_bit(FLG_TRANSPARENT, &bch->Flags);
            break;
        case ISDN_P_B_PCM:
            xhfc_pcm_ch_setup(xhfc, channel);
            xhfc_setup_su(xhfc, port->idx, (channel % 4) ? 1 : 0, 1);
            bch->state = protocol;
            set_bit(FLG_TRANSPARENT, &bch->Flags);
            break;
        case ISDN_P_B_RAW:
            xhfc_setup_fifo(xhfc, (channel * 2), 6, 0, 0, 1);
            xhfc_setup_fifo(xhfc, (channel * 2) + 1, 6, 0, 0, 1);
            xhfc_setup_su(xhfc, port->idx, (channel % 4) ? 1 : 0, 1);
            bch->state = protocol;
            set_bit(FLG_TRANSPARENT, &bch->Flags);
            break;
        case ISDN_P_B_HDLC:
            xhfc_setup_fifo(xhfc, (channel * 2), 4, 0, M_FR_ABO, 1);
            xhfc_setup_fifo(xhfc, (channel * 2) + 1, 4, 0,
                   M_FR_ABO | M_FIFO_IRQMSK, 1);
            xhfc_setup_su(xhfc, port->idx, (channel % 4) ? 1 : 0,
                 1);
            bch->state = protocol;
            set_bit(FLG_HDLC, &bch->Flags);
            break;
        default:
            return -ENOPROTOOPT;
    }
    return 0;
}


static void  xhfc_setup_dch(
           struct dchannel *dch)
/*
 * init DChannel TX/RX fifos
 */
{
    struct port *port = dch->hw;
    struct xhfc *xhfc = port->xhfc;
    __u8 channel = (port->idx * 4) + 2;
    xhfc_setup_fifo(xhfc, (channel * 2),     5, 2, M_FR_ABO | M_FIFO_IRQMSK, 1);
    xhfc_setup_fifo(xhfc, (channel * 2) + 1, 5, 2, M_FR_ABO | M_FIFO_IRQMSK, 1);
}


static void  xhfc_setup_ech(
           struct dchannel *ech)
/*
 * init EChannel RX fifo
 */
{
    struct port *port = ech->hw;
    struct xhfc *xhfc = port->xhfc;
    __u8 channel = (port->idx * 4) + 3;
    xhfc_setup_fifo(xhfc, (channel * 2) + 1, 5, 2,
           M_FR_ABO | M_FIFO_IRQMSK, 1);
}


static void  xhfc_ph_command(
                struct port *port,
                     u_char  command)
{
    struct xhfc *xhfc = port->xhfc;

    if(debug & DEBUG_HW)
    {
        printk(KERN_INFO "%s %s: %s (%i)\n",
               __FUNCTION__,
               port->name,
              (command < (sizeof(XHFC_PH_COMMANDS) / sizeof(char const *))) ? XHFC_PH_COMMANDS[command] : "XHFC_UNKNOWN",
               command);
    }

    switch(command)
    {
        case L1_ACTIVATE_TE:
            xhfc_write(xhfc, R_SU_SEL, port->idx);
            xhfc_write(xhfc, A_SU_WR_STA, STA_ACTIVATE);
            break;

        case L1_FORCE_DEACTIVATE_TE:
            xhfc_write(xhfc, R_SU_SEL, port->idx);
            xhfc_write(xhfc, A_SU_WR_STA, STA_DEACTIVATE);
            break;

        case L1_ACTIVATE_NT:
            xhfc_write(xhfc, R_SU_SEL, port->idx);
            xhfc_write(xhfc, A_SU_WR_STA,
                   STA_ACTIVATE | M_SU_SET_G2_G3);
            break;

        case L1_DEACTIVATE_NT:
            xhfc_write(xhfc, R_SU_SEL, port->idx);
            xhfc_write(xhfc, A_SU_WR_STA, STA_DEACTIVATE);
            break;

        case L1_TESTLOOP_B1:
            /* connect B1-SU RX with PCM TX */
            xhfc_setup_fifo(xhfc, port->idx * 8, 0xC6, 0, 0, 0);
            /* connect B1-SU TX with PCM RX */
            xhfc_setup_fifo(xhfc, port->idx * 8 + 1, 0xC6, 0, 0, 0);

            /* PCM timeslot B1 TX */
            xhfc_write(xhfc, R_SLOT, port->idx * 8);
            /* enable B1 TX timeslot on STIO1 */
            xhfc_write(xhfc, A_SL_CFG, port->idx * 8 + 0x80);

            /* PCM timeslot B1 RX */
            xhfc_write(xhfc, R_SLOT, port->idx * 8 + 1);
            /* enable B1 RX timeslot on STIO1 */
            xhfc_write(xhfc, A_SL_CFG,
                   port->idx * 8 + 1 + 0xC0);

            xhfc_setup_su(xhfc, port->idx, 0, 1);
            break;

        case L1_TESTLOOP_B2:
            /* connect B2-SU RX with PCM TX */
            xhfc_setup_fifo(xhfc, port->idx * 8 + 2, 0xC6, 0, 0, 0);
            /* connect B2-SU TX with PCM RX */
            xhfc_setup_fifo(xhfc, port->idx * 8 + 3, 0xC6, 0, 0, 0);

            /* PCM timeslot B2 TX */
            xhfc_write(xhfc, R_SLOT, port->idx * 8 + 2);
            /* enable B2 TX timeslot on STIO1 */
            xhfc_write(xhfc, A_SL_CFG,
                   port->idx * 8 + 2 + 0x80);

            /* PCM timeslot B2 RX */
            xhfc_write(xhfc, R_SLOT, port->idx * 8 + 3);
            /* enable B2 RX timeslot on STIO1 */
            xhfc_write(xhfc, A_SL_CFG,
                   port->idx * 8 + 3 + 0xC0);

            xhfc_setup_su(xhfc, port->idx, 1, 1);
            break;

        case L1_TESTLOOP_D:
            /* connect D-SU RX with PCM TX */
            xhfc_setup_fifo(xhfc, port->idx * 8 + 4, 0xC4, 2,
                   M_FR_ABO, 1);
            /* connect D-SU TX with PCM RX */
            xhfc_setup_fifo(xhfc, port->idx * 8 + 5, 0xC4, 2,
                   M_FR_ABO | M_FIFO_IRQMSK, 1);

            /* PCM timeslot D TX */
            xhfc_write(xhfc, R_SLOT, port->idx * 8 + 4);
            /* enable D TX timeslot on STIO1 */
            xhfc_write(xhfc, A_SL_CFG,
                   port->idx * 8 + 4 + 0x80);

            /* PCM timeslot D RX */
            xhfc_write(xhfc, R_SLOT, port->idx * 8 + 5);
            /* enable D RX timeslot on STIO1 */
            xhfc_write(xhfc, A_SL_CFG,
                   port->idx * 8 + 5 + 0xC0);
            break;
    }
}


static void xhfc_pcm_init(
                  xhfc_t *xhfc)
{
    // Task_7636 - Set C2IO to output clock at 2.048MHz
    SET_V_PCM_IDX(xhfc->pcm_md0, 0xA);
    SET_V_C2O_EN(xhfc->pcm_md2, 1);
    SET_V_C2I_EN(xhfc->pcm_md2, 0);
    xhfc_write(xhfc, R_PCM_MD0, xhfc->pcm_md0);
    xhfc_write(xhfc, R_PCM_MD2, xhfc->pcm_md2);
}


#define PLL_TIMEOUT    200
/*
 * initialise the XHFC ISDN controller
 * return 0 on success.
 */
static int  xhfc_hw_initialise(
                  struct xhfc *xhfc)
{
    int err = 0;
    int i, irq_cnt, timeout;
    __u8 chip_id;

    chip_id = xhfc_read(xhfc, R_CHIP_ID);
    switch(chip_id)
    {
        case CHIP_ID_1SU:
            xhfc->num_ports = 1;
            xhfc->channels = 4;
            xhfc->max_z = 0xFF;

            /* timer irq interval 16 ms */
            SET_V_EV_TS(xhfc->ti_wd, 6);

            xhfc_write(xhfc, R_FIFO_MD, 2);
            SET_V_SU0_IRQMSK(xhfc->su_irqmsk, 1);
            break;

        case CHIP_ID_2SU:
            xhfc->num_ports = 2;
            xhfc->channels = 8;
            xhfc->max_z = 0x7F;

            /* timer irq interval 8 ms */
            SET_V_EV_TS(xhfc->ti_wd, 5);

            xhfc_write(xhfc, R_FIFO_MD, 1);
            SET_V_SU0_IRQMSK(xhfc->su_irqmsk, 1);
            SET_V_SU1_IRQMSK(xhfc->su_irqmsk, 1);
            break;

        case CHIP_ID_2S4U:
            xhfc->num_ports = 4;
            xhfc->channels = 16;
            xhfc->max_z = 0x3F;

            /* timer irq interval 4 ms */
            SET_V_EV_TS(xhfc->ti_wd, 4);

            xhfc_write(xhfc, R_FIFO_MD, 0);
            SET_V_SU0_IRQMSK(xhfc->su_irqmsk, 1);
            SET_V_SU1_IRQMSK(xhfc->su_irqmsk, 1);
            SET_V_SU2_IRQMSK(xhfc->su_irqmsk, 1);
            SET_V_SU3_IRQMSK(xhfc->su_irqmsk, 1);
            break;

        case CHIP_ID_4SU:
            xhfc->num_ports = 4;
            xhfc->channels = 16;
            xhfc->max_z = 0x3F;

            /* timer irq interval 4 ms */
            SET_V_EV_TS(xhfc->ti_wd, 4);

            xhfc_write(xhfc, R_FIFO_MD, 0);
            SET_V_SU0_IRQMSK(xhfc->su_irqmsk, 1);
            SET_V_SU1_IRQMSK(xhfc->su_irqmsk, 1);
            SET_V_SU2_IRQMSK(xhfc->su_irqmsk, 1);
            SET_V_SU3_IRQMSK(xhfc->su_irqmsk, 1);
            break;

        default:
            err = -ENODEV;
    }

    if(err)
    {
        printk(KERN_ERR "%s %s: unkown Chip ID 0x%x\n",
               xhfc->name, __FUNCTION__, chip_id);
        return(err);
    }
    else
    {
        printk(KERN_INFO "%s ChipID: 0x%x\n",
               xhfc->name, chip_id);
    }


#ifdef CONFIG_LAKE_PLATFORM_osip_fc
    /* The following is an Email from Joerg (cologne engineer) on how to setup
       the chip to convert the 15.36MHz input clock to the correct internal clock
       required to run the chip.

       Start of mail:

       // enable CLK_OUT output so that you can check PLL clock at this pin
       write (R_CLK_CFG, V_CLKO_PLL)
       // enable PLL and set M=1
       write (R_PLL_CTRL, V_PLL_NRES | 0x40)
       write (R_PLL_P, 0x05)
       write (R_PLL_N, 0x18)
       write (R_PLL_S, 0x03)
       // poll R_PLL_STAT until PLL is locked
       i=0;
       while( (read (R_PLL_STA) & 0x80 == 0) && i<TIMEOUT ) i++;
       if(i==TIMEOUT) printf("ERROR! PLL not locked - this should never happen\n")
       // use PLL clock as system clock
       write (R_CLK_CFG, V_CLK_PLL | V_CLKO_PLL)

       End of mail.
     */
    xhfc_write(xhfc, R_PLL_CTRL, M_PLL_NRES | 0x40);
    xhfc_write(xhfc, R_PLL_P, 0x05);
    xhfc_write(xhfc, R_PLL_N, 0x18);
    xhfc_write(xhfc, R_PLL_S, 0x03);
#else
    xhfc_write(xhfc, R_PLL_CTRL, M_PLL_NRES | 0x40);
    xhfc_write(xhfc, R_PLL_P, 16);
    xhfc_write(xhfc, R_PLL_N, 72);
    xhfc_write(xhfc, R_PLL_S, 3);
#endif                /* CONFIG_LAKE_PLATFORM_osip_fc */

    // Wait for the PLL to lock.
    i = PLL_TIMEOUT;
    while(i--)
    {
        __u8 regVal = 0;

        regVal = xhfc_read(xhfc, R_PLL_STA);

        if(GET_V_PLL_LOCK(regVal))
        {
            if(debug & DEBUG_HW)
                printk(KERN_INFO "PLL clock is now locked (%d)\n", PLL_TIMEOUT - i);
            break;
        }
        mdelay(10);
    }

    if(!i)
    {
        printk(KERN_ERR
               "Failed to set the XHFC system clock to the PPL clock\n");
        return(-ENODEV);
    }

    xhfc_write(xhfc, R_CLK_CFG, M_CLK_PLL);

    /* software reset to enable R_FIFO_MD setting */
    xhfc_write(xhfc, R_CIRM, M_SRES);
    udelay(5);
    xhfc_write(xhfc, R_CIRM, 0);

    /* amplitude */
    xhfc_write(xhfc, R_PWM_MD, 0x80);
    xhfc_write(xhfc, R_PWM1, 0x18);

    xhfc_write(xhfc, R_FIFO_THRES, 0x11);

    timeout = 0x2000;
    while((xhfc_read(xhfc, R_STATUS) & (M_BUSY | M_PCM_INIT))
           && (timeout))
        timeout--;

    if(!(timeout))
    {
        if(debug & DEBUG_HW)
            printk(KERN_ERR
                   "%s %s: initialization sequence "
                   "not completed!\n",
                   xhfc->name, __FUNCTION__);
        return(-ENODEV);
    }

    /* set PCM master mode */
    SET_V_PCM_MD(xhfc->pcm_md0, 1);
    SET_V_C4_POL(xhfc->pcm_md0, 1);
    SET_V_F0_LEN(xhfc->pcm_md0, 1);
    xhfc_write(xhfc, R_PCM_MD0, xhfc->pcm_md0);

    /* set pll adjust */
    SET_V_PCM_IDX(xhfc->pcm_md0, 0x09);
    SET_V_PLL_ADJ(xhfc->pcm_md1, 3);
    xhfc_write(xhfc, R_PCM_MD0, xhfc->pcm_md0);
    xhfc_write(xhfc, R_PCM_MD1, xhfc->pcm_md1);
    // Set PCM mode if it has been select in the module paramters.
    // Default to PCM master mode even if nothing is passed in.
    xhfc->pcm = pcm;
    if(xhfc->pcm > -1)
        xhfc_pcm_init(xhfc);

    /* perfom short irq test */
    xhfc->testirq = 1;
    SET_V_TI_IRQMSK(xhfc->misc_irqmsk, 1);
    xhfc_write(xhfc, R_MISC_IRQMSK, xhfc->misc_irqmsk);
    xhfc_enable_interrupts(xhfc);
    xhfc->irq_cnt = 0;
    mdelay(1 << GET_V_EV_TS(xhfc->ti_wd));
    irq_cnt = xhfc->irq_cnt;
    xhfc_disable_interrupts(xhfc);
    SET_V_TI_IRQMSK(xhfc->misc_irqmsk, 0);
    xhfc_write(xhfc, R_MISC_IRQMSK, xhfc->misc_irqmsk);
    mdelay(1 << GET_V_EV_TS(xhfc->ti_wd));
    xhfc->testirq = 0;

    if(irq_cnt < 3)
    {
        printk(KERN_INFO
            "%s %s: ERROR getting IRQ (irq_cnt %i)\n",
            xhfc->name, __FUNCTION__, irq_cnt);

        return(-EIO);
    }

    printk(KERN_INFO
          "%s %s: IRQ Count: %d\n",
           xhfc->name, __FUNCTION__, irq_cnt);

    return(0);
}


int  xhfc_su_setup_instance(
               struct xhfc *xhfc,
             struct device *parent)
/*
 * init mISDN interface (called for each XHFC)
 */
{
    struct port *p;
    int err = 0, i, j;

    if(debug)
        printk(KERN_INFO "%s: %s %s\n", DRIVER_NAME, __func__, xhfc->name);

#ifdef XHFC_BH_TASKLET
    tasklet_init(&xhfc->tasklet,
                  xhfc_bh_handler,
   (unsigned long)xhfc);
#else /* !XHFC_BH_TASKLET */
    sema_init(&xhfc->lock, 1);
#endif /* XHFC_BH_TASKLET */
    err = xhfc_hw_initialise(xhfc);
    if(err)
        goto out;

    err = -ENOMEM;
    xhfc->port =
        kzalloc(sizeof(struct port) * xhfc->num_ports, GFP_KERNEL);
    for(i = 0; i < xhfc->num_ports; i++)
    {
        p = xhfc->port + i;
        p->idx = i;
        p->xhfc = xhfc;
        xhfc_port_lock_init(p);

        /* init D-Channel Interface */
        mISDN_initdchannel(&p->dch, MAX_DFRAME_LEN_L1, xhfc_ph_state);
        p->dch.dev.D.send = xhfc_l2l1D;
        p->dch.dev.D.ctrl = xhfc_dctrl;
        p->dch.debug = debug & 0xFFFF;
        p->dch.hw = p;
        p->dch.dev.Dprotocols =
            (1 << ISDN_P_TE_S0) |
            (1 << ISDN_P_NT_S0) |
            (1 << ISDN_P_TE_UP0) |
            (1 << ISDN_P_NT_UP0);

        /* init E-Channel Interface */
        mISDN_initdchannel(&p->ech, MAX_DFRAME_LEN_L1, NULL);
        p->ech.hw = p;
        p->ech.debug = debug & 0xFFFF;

        /* init B-Channel Interfaces */
        p->dch.dev.Bprotocols =
            (1 << (ISDN_P_B_PCM & ISDN_P_B_MASK)) |
            (1 << (ISDN_P_B_RAW & ISDN_P_B_MASK)) |
            (1 << (ISDN_P_B_HDLC & ISDN_P_B_MASK));
        p->dch.dev.nrbchan = 2;
        for(j = 0; j < 2; j++)
        {
            p->bch[j].nr = j + 1;
            set_channelmap(j + 1, p->dch.dev.channelmap);
            p->bch[j].debug = debug;
            mISDN_initbchannel(&p->bch[j], MAX_DATA_MEM);
            p->bch[j].hw = p;
            p->bch[j].ch.send = xhfc_l2l1B;
            p->bch[j].ch.ctrl = xhfc_bctrl;
            p->bch[j].ch.nr = j + 1;
            list_add(&p->bch[j].ch.list,
                 &p->dch.dev.bchannels);
        }

        /*
         * init F7 timer to delay ACTIVATE INDICATION
         */
        init_timer(&p->f7_timer);
        p->f7_timer.data = (long)p;
        p->f7_timer.function = (void *)xhfc_f7_timer_expire;

        snprintf(p->name, MISDN_MAX_IDLEN - 1, "%s.%d",
            xhfc->name, p->idx);
        printk(KERN_INFO "%s: registered as '%s'\n", DRIVER_NAME,
               p->name);

        err = mISDN_register_device(&p->dch.dev, parent, p->name);
        if(err)
        {
            mISDN_freebchannel(&p->bch[1]);
            mISDN_freebchannel(&p->bch[0]);
            mISDN_freedchannel(&p->dch);
        }
        else
        {
            xhfc_cnt++;
            xhfc_setup_dch(&p->dch);
            xhfc_enable_interrupts(xhfc);
        }
    }

      out:
    return err;
}


int  xhfc_release_instance(
              struct xhfc *hw)
{
    struct port *p;
    int i;

    if(debug)
        printk(KERN_INFO "%s: %s\n", DRIVER_NAME, __func__);

    for(i = 0; i < hw->num_ports; i++)
    {
        p = hw->port + i;
        /* TODO
           xhfc_setup_bch(&p->bch[0], ISDN_P_NONE);
           xhfc_setup_bch(&p->bch[1], ISDN_P_NONE);
         */
        if(p->mode & PORT_MODE_TE)
            l1_event(p->dch.l1, CLOSE_CHANNEL);

        mISDN_unregister_device(&p->dch.dev);
        mISDN_freebchannel(&p->bch[1]);
        mISDN_freebchannel(&p->bch[0]);
        mISDN_freedchannel(&p->dch);
    }

    if(hw)
    {
        if(hw->port)
            kfree(hw->port);
        kfree(hw);
    }
    return 0;
}


static int  xhfc_l1callback(
           struct dchannel *dch,
                     u_int  cmd)
/*
 * Layer 1 callback function
 */
{
    struct port *p = dch->hw;

    if(debug & DEBUG_HW)
        printk(KERN_INFO "%s: %s cmd 0x%x\n",
               p->name, __func__, cmd);

    switch(cmd)
    {
        case INFO3_P8:
        case INFO3_P10:
        case HW_RESET_REQ:
        case HW_POWERUP_REQ:
            break;

        case HW_DEACT_REQ:
            skb_queue_purge(&dch->squeue);
            if(dch->tx_skb)
            {
                dev_kfree_skb(dch->tx_skb);
                dch->tx_skb = NULL;
            }
            dch->tx_idx = 0;
            if(dch->rx_skb)
            {
                dev_kfree_skb(dch->rx_skb);
                dch->rx_skb = NULL;
            }
            test_and_clear_bit(FLG_TX_BUSY, &dch->Flags);
            break;
        case PH_ACTIVATE_IND:
            test_and_set_bit(FLG_ACTIVE, &dch->Flags);
            _queue_data(&dch->dev.D, cmd, MISDN_ID_ANY, 0,
                    NULL, GFP_ATOMIC);
            break;
        case PH_DEACTIVATE_IND:
            test_and_clear_bit(FLG_ACTIVE, &dch->Flags);
            _queue_data(&dch->dev.D, cmd, MISDN_ID_ANY, 0,
                    NULL, GFP_ATOMIC);
            break;
        default:
            if(dch->debug & DEBUG_HW)
                printk(KERN_INFO
                       "%s: %s: unknown cmd %x\n", p->name,
                       __func__, cmd);
            return -1;
    }
    return 0;
}


static void  deactivate_bchannel(
                struct bchannel *bch)
{
    struct port *p = bch->hw;

    if(bch->debug)
        printk(KERN_INFO "%s: %s: bch->nr(%i)\n",
               p->name, __func__, bch->nr);

    down_interruptible(&p->xhfc->lock);

    mISDN_clear_bchannel(bch);

    up(&p->xhfc->lock);

    xhfc_setup_bch(bch, ISDN_P_NONE);
}


static int  xhfc_l2l1B(
  struct mISDNchannel *ch,
       struct sk_buff *skb)
/*
 * Layer2 -> Layer 1 Bchannel data
 */
{
    struct bchannel *bch = container_of(ch, struct bchannel, ch);
    struct port *p = bch->hw;
    int ret = -EINVAL;
    struct mISDNhead *hh = mISDN_HEAD_P(skb);

    if(debug & DEBUG_HW)
        printk(KERN_INFO "%s: %s\n", p->name, __func__);

    switch(hh->prim)
    {
        case PH_DATA_REQ:
            down_interruptible(&p->xhfc->lock);

            ret = bchannel_senddata(bch, skb);

            up(&p->xhfc->lock);

            if(ret > 0)
            {
                ret = 0;
                queue_ch_frame(ch, PH_DATA_CNF, hh->id, NULL);
            }
            return ret;
        case PH_ACTIVATE_REQ:
            if(!test_and_set_bit(FLG_ACTIVE, &bch->Flags))
            {
                ret = xhfc_setup_bch(bch, ch->protocol);
            }
            else
                ret = 0;
            if(!ret)
                _queue_data(ch, PH_ACTIVATE_IND,
                        MISDN_ID_ANY, 0, NULL,
                        GFP_KERNEL);
            break;
        case PH_DEACTIVATE_REQ:
            deactivate_bchannel(bch);
            _queue_data(ch, PH_DEACTIVATE_IND, MISDN_ID_ANY,
                    0, NULL, GFP_KERNEL);
            ret = 0;
            break;
    }
    if(!ret)
        dev_kfree_skb(skb);
    return ret;
}


static int  xhfc_l2l1D(
  struct mISDNchannel *ch,
       struct sk_buff *skb)
/*
 * Layer2 -> Layer 1 Dchannel data
 */
{
    struct mISDNdevice *dev = container_of(ch, struct mISDNdevice, D);
    struct dchannel *dch = container_of(dev, struct dchannel, dev);
    struct mISDNhead *hh = mISDN_HEAD_P(skb);
    struct port *p = dch->hw;
    int ret = -EINVAL;

    switch(hh->prim)
    {
        case PH_DATA_REQ:
            if(debug & DEBUG_HW)
                printk(KERN_INFO "%s: %s: PH_DATA_REQ\n",
                       p->name, __func__);

            down_interruptible(&p->xhfc->lock);

            ret = dchannel_senddata(dch, skb);

            up(&p->xhfc->lock);

            if(ret > 0)
            {
                ret = 0;
                queue_ch_frame(ch, PH_DATA_CNF, hh->id,
                           NULL);
            }
            xhfc_bh_handler(p->xhfc);
            return ret;

        case PH_ACTIVATE_REQ:
            if(debug & DEBUG_HW)
                printk(KERN_INFO
                       "%s: %s: PH_ACTIVATE_REQ %s\n",
                       p->name, __func__,
                       (p->mode & PORT_MODE_NT) ?
                       "NT" : "TE");

            if(p->mode & PORT_MODE_NT)
            {
                ret = 0;
                if(test_bit(FLG_ACTIVE, &dch->Flags))
                {
                    _queue_data(&dch->dev.D,
                            PH_ACTIVATE_IND,
                            MISDN_ID_ANY, 0, NULL,
                            GFP_ATOMIC);
                }
                else
                {
                    xhfc_ph_command(p, L1_ACTIVATE_NT);
                    test_and_set_bit(FLG_L2_ACTIVATED,
                             &dch->Flags);
                }
            }
            else
            {
                xhfc_ph_command(p, L1_ACTIVATE_TE);
                ret = l1_event(dch->l1, hh->prim);
            }
            break;

        case PH_DEACTIVATE_REQ:
            if(debug & DEBUG_HW)
                printk(KERN_INFO
                       "%s: %s: PH_DEACTIVATE_REQ\n",
                       p->name, __func__);
            test_and_clear_bit(FLG_L2_ACTIVATED, &dch->Flags);

            if(p->mode & PORT_MODE_NT)
            {
                down_interruptible(&p->xhfc->lock);

                xhfc_ph_command(p, L1_DEACTIVATE_NT);
                skb_queue_purge(&dch->squeue);
                if(dch->tx_skb)
                {
                    dev_kfree_skb(dch->tx_skb);
                    dch->tx_skb = NULL;
                }
                dch->tx_idx = 0;
                if(dch->rx_skb)
                {
                    dev_kfree_skb(dch->rx_skb);
                    dch->rx_skb = NULL;
                }
                test_and_clear_bit(FLG_TX_BUSY,
                           &dch->Flags);

                up(&p->xhfc->lock);

                ret = 0;
            }
            else
            {
                ret = l1_event(dch->l1, hh->prim);
            }
            break;
    }
    return ret;
}


static int  channel_bctrl(
         struct bchannel *bch,
   struct mISDN_ctrl_req *cq)
/*
 * Layer 1 B-channel hardware access
 */
{
    int ret = 0;

    switch(cq->op)
    {
        case MISDN_CTRL_GETOP:
            cq->op = MISDN_CTRL_FILL_EMPTY |
                 MISDN_CTRL_GET_PCM_SLOTS |
                 MISDN_CTRL_SET_PCM_SLOTS;
            break;
        case MISDN_CTRL_GET_PCM_SLOTS:
            cq->p1 = bch->nr;
            cq->p2 = bch->pcm_tx;
            cq->p3 = bch->pcm_rx;
            break;
        case MISDN_CTRL_SET_PCM_SLOTS:
            if(cq->p1 != bch->nr)
            {
                printk(KERN_WARNING
                    "%s: requested BC%d, but bch nr %d\n",
                    __func__, cq->p1, bch->nr);
                ret = -EINVAL;
            }
            else
                ret = xhfc_set_pcm_tx_rx(bch, cq->p2, cq->p3);
            break;
        case MISDN_CTRL_FILL_EMPTY:
            test_and_set_bit(FLG_FILLEMPTY, &bch->Flags);
            if(debug & DEBUG_HW_OPEN)
                printk(KERN_INFO
                       "%s: FILL_EMPTY request (nr=%d "
                       "off=%d)\n", __func__, bch->nr,
                       !!cq->p1);
            break;
        default:
            printk(KERN_WARNING "%s: unknown Op %x\n",
                   __func__, cq->op);
            ret = -EINVAL;
            break;
    }
    return ret;
}


static int  xhfc_bctrl(
  struct mISDNchannel *ch,
                 u_int cmd,
                 void *arg)
/*
 * Layer 1 B-channel hardware access
 */
{
    struct bchannel *bch = container_of(ch, struct bchannel, ch);
    int ret = -EINVAL;

    if(bch->debug & DEBUG_HW)
        printk(KERN_INFO "%s: cmd:%x %p\n", __func__, cmd, arg);

    switch(cmd)
    {
        case HW_TESTRX_RAW:
        case HW_TESTRX_HDLC:
        case HW_TESTRX_OFF:
            ret = -EINVAL;
            break;

        case CLOSE_CHANNEL:
            test_and_clear_bit(FLG_OPEN, &bch->Flags);
            if(test_bit(FLG_ACTIVE, &bch->Flags))
                deactivate_bchannel(bch);
            ch->protocol = ISDN_P_NONE;
            ch->peer = NULL;
            module_put(THIS_MODULE);
            ret = 0;
            break;
        case CONTROL_CHANNEL:
            ret = channel_bctrl(bch, arg);
            break;
        default:
            printk(KERN_WARNING "%s: unknown prim(%x)\n",
                   __func__, cmd);
    }
    return ret;
}


static int  open_dchannel(
             struct port *p,
     struct mISDNchannel *ch,
      struct channel_req *rq)
{
    int err = 0;

    if(debug & DEBUG_HW_OPEN)
        printk(KERN_INFO "%s: %s: dev(%d) open from %p\n",
               p->name, __func__, p->dch.dev.id,
               __builtin_return_address(0));
    if(rq->protocol == ISDN_P_NONE)
        return -EINVAL;

    test_and_clear_bit(FLG_ACTIVE, &p->dch.Flags);
    test_and_clear_bit(FLG_ACTIVE, &p->ech.Flags);
    p->dch.state = 0;

    /* E-Channel logging */
    if(rq->adr.channel == 1)
    {
        xhfc_setup_ech(&p->ech);
        set_bit(FLG_ACTIVE, &p->ech.Flags);
        _queue_data(&p->ech.dev.D, PH_ACTIVATE_IND,
                 MISDN_ID_ANY, 0, NULL, GFP_ATOMIC);
    }

    if(!p->initdone)
    {
        if(IS_ISDN_P_S0(rq->protocol))
            p->mode |= PORT_MODE_S0;
        else
        {
            if(IS_ISDN_P_UP0(rq->protocol))
                p->mode |= PORT_MODE_UP0;
        }

        if(IS_ISDN_P_TE(rq->protocol))
        {
            p->mode |= PORT_MODE_TE;
            err = create_l1(&p->dch, xhfc_l1callback);
            if(err)
                return err;
        }
        else
        {
            p->mode |= PORT_MODE_NT;
        }
        init_su(p);
        ch->protocol = rq->protocol;
        p->initdone = 1;
    }
    else
    {
        if(rq->protocol != ch->protocol)
            return -EPROTONOSUPPORT;
    }

    /* force initial layer1 statechanges */
    p->xhfc->su_irq = p->xhfc->su_irqmsk;

    if(((ch->protocol == ISDN_P_NT_S0) && (p->dch.state == 3)) ||
        ((ch->protocol == ISDN_P_TE_S0) && (p->dch.state == 7)))
        _queue_data(ch, PH_ACTIVATE_IND, MISDN_ID_ANY,
                0, NULL, GFP_KERNEL);

    rq->ch = ch;
    if(!try_module_get(THIS_MODULE))
        printk(KERN_WARNING "%s: %s: cannot get module\n",
               p->name, __func__);
    return 0;
}


static struct bchannel *get_bchannel4number(
                               struct port *p,
                                       int  nr)
{
    if(nr < 1 || nr > 2)
        return NULL;
    return &p->bch[nr - 1];
}


static int  open_bchannel(
             struct port *p,
      struct channel_req *rq)
{
    struct bchannel *bch;

    if(rq->protocol == ISDN_P_NONE)
        return -EINVAL;

    if(debug & DEBUG_HW)
        printk(KERN_INFO "%s: %s B%i\n",
               p->name, __func__, rq->adr.channel);

    bch = get_bchannel4number(p, rq->adr.channel);
    if(!bch)
        return -EINVAL;

    if(test_and_set_bit(FLG_OPEN, &bch->Flags))
        return -EBUSY;    /* b-channel can be only open once */
    test_and_clear_bit(FLG_FILLEMPTY, &bch->Flags);
    bch->ch.protocol = rq->protocol;
    rq->ch = &bch->ch;

    if(!try_module_get(THIS_MODULE))
        printk(KERN_WARNING "%s: %s:cannot get module\n",
               p->name, __func__);
    return 0;
}


static int  channel_ctrl(
            struct port *p,
  struct mISDN_ctrl_req *cq)
{
    int ret = 0;
    struct bchannel *bch;

    if(debug)
        printk(KERN_INFO "%s: %s op(0x%x) channel(0x%x)\n",
               p->name, __func__, (cq->op), (cq->channel));

    switch(cq->op)
    {
        case MISDN_CTRL_GETOP:
            cq->op = MISDN_CTRL_LOOP |
                MISDN_CTRL_CONNECT |
                MISDN_CTRL_DISCONNECT |
                MISDN_CTRL_GET_PCM_SLOTS |
                MISDN_CTRL_SET_PCM_SLOTS;
            break;
        case MISDN_CTRL_GET_PCM_SLOTS:
            bch = get_bchannel4number(p, cq->p1);
            if(bch)
            {
                cq->p2 = bch->pcm_tx;
                cq->p3 = bch->pcm_rx;
            }
            else
                ret -EINVAL;
            break;
        case MISDN_CTRL_SET_PCM_SLOTS:
            bch = get_bchannel4number(p, cq->p1);
            if(!bch)
            {
                printk(KERN_WARNING
                    "%s: requested invalid BC%d\n",
                    __func__, cq->p1);
                ret = -EINVAL;
            }
            else
                ret = xhfc_set_pcm_tx_rx(bch, cq->p2, cq->p3);
            break;
        case MISDN_CTRL_LOOP:
            if(debug)
                printk(KERN_INFO "CTRL_LOOP %d\n",
                    cq->channel);
            switch(cq->channel)
            {
            case 1:
                xhfc_ph_command(p, L1_TESTLOOP_B1);
                break;
            case 2:
                xhfc_ph_command(p, L1_TESTLOOP_B2);
                break;
            case 3:
                xhfc_ph_command(p, L1_TESTLOOP_D);
                break;
            default:
                xhfc_ph_command(p, L1_TESTLOOP_OFF);
            }
            break;
        default:
            printk(KERN_WARNING "%s: %s: unknown Op %x\n",
                   p->name, __func__, cq->op);
            ret = -EINVAL;
            break;
    }
    return ret;
}


static int  xhfc_dctrl(
  struct mISDNchannel *ch,
                u_int  cmd,
                 void *arg)
/*
 * device control function
 */
{
    struct mISDNdevice *dev = container_of(ch, struct mISDNdevice, D);
    struct dchannel *dch = container_of(dev, struct dchannel, dev);
    struct port *p = dch->hw;
    struct channel_req *rq;
    int err = 0;

    if(dch->debug & DEBUG_HW)
        printk(KERN_INFO "%s: %s: cmd:%x %p\n",
               p->name, __func__, cmd, arg);
    switch(cmd)
    {
        case OPEN_CHANNEL:
            rq = arg;
            if((IS_ISDN_P_S0(rq->protocol)) ||
               (IS_ISDN_P_UP0(rq->protocol)))
                err = open_dchannel(p, ch, rq);
            else
                err = open_bchannel(p, rq);
            break;
        case CLOSE_CHANNEL:
            if(debug & DEBUG_HW_OPEN)
                printk(KERN_INFO
                       "%s: %s: dev(%d) close from %p\n",
                       p->name, __func__, p->dch.dev.id,
                       __builtin_return_address(0));
            module_put(THIS_MODULE);
            break;
        case CONTROL_CHANNEL:
            err = channel_ctrl(p, arg);
            break;
        default:
            if(dch->debug & DEBUG_HW)
                printk(KERN_INFO
                       "%s: %s: unknown command %x\n",
                       p->name, __func__, cmd);
            return -EINVAL;
    }
    return err;
}


static void  xhfc_f7_timer_expire(
                     struct port *port)
/*
 * send ACTIVATE INDICATION to l2
 */
{
    l1_event(port->dch.l1, XHFC_L1_F7);
}


static void  xhfc_ph_state_te(
             struct dchannel *dch)
/*
 * S0 TE state change event handler
 */
{
    struct port *p = dch->hw;

    if(debug)
        printk(KERN_INFO "%s: %s: TE F%d\n",
               p->name, __func__, dch->state);

    if((dch->state != 7) && timer_pending(&p->f7_timer))
        del_timer(&p->f7_timer);

    switch(dch->state)
    {
        case 0:
            l1_event(dch->l1, XHFC_L1_F0);
            break;
        case 3:
            l1_event(dch->l1, XHFC_L1_F3);
            break;
        case 5:
        case 8:
            l1_event(dch->l1, ANYSIGNAL);
            break;
        case 6:
            l1_event(dch->l1, INFO2);
            break;
        case 7:
            /* delay ACTIVATE INDICATION for 1ms */
            if(!(timer_pending(&p->f7_timer)))
            {
                p->f7_timer.expires = jiffies + (HZ / 1000);
                add_timer(&p->f7_timer);
            }
            break;
    }
}


static void  xhfc_ph_state_nt(
             struct dchannel *dch)
/*
 * S0 NT state change event handler
 */
{
    struct port *p = dch->hw;

    if(debug & DEBUG_HW)
        printk(KERN_INFO "%s: %s: NT G%d\n",
               p->name, __func__, dch->state);

    switch(dch->state)
    {
        case(1):
            test_and_clear_bit(FLG_ACTIVE, &dch->Flags);
            test_and_clear_bit(FLG_L2_ACTIVATED, &dch->Flags);
            p->nt_timer = 0;
            p->timers &= ~NT_ACTIVATION_TIMER;
            break;
        case(2):
            if (p->nt_timer < 0)
            {
                p->nt_timer = 0;
                p->timers &= ~NT_ACTIVATION_TIMER;
            }
            else
            {
                p->timers |= NT_ACTIVATION_TIMER;
                p->nt_timer = NT_T1_COUNT;
                xhfc_write(p->xhfc, R_SU_SEL, p->idx);
                xhfc_write(p->xhfc, A_SU_WR_STA, M_SU_SET_G2_G3);
            }
            break;
        case(3):
            p->nt_timer = 0;
            p->timers &= ~NT_ACTIVATION_TIMER;
            test_and_set_bit(FLG_ACTIVE, &dch->Flags);
            _queue_data(&dch->dev.D, PH_ACTIVATE_IND,
                    MISDN_ID_ANY, 0, NULL, GFP_ATOMIC);
            break;
        case(4):
            p->nt_timer = 0;
            p->timers &= ~NT_ACTIVATION_TIMER;
            return;
        default:
            break;
    }
}


static void  xhfc_ph_state(
          struct dchannel *dch)
{
    struct port *p = dch->hw;
    if(p->mode & PORT_MODE_NT)
        xhfc_ph_state_nt(dch);
    else
    {
        if(p->mode & PORT_MODE_TE)
            xhfc_ph_state_te(dch);
    }
}


static void  xhfc_write_fifo(
                struct xhfc *xhfc,
                       __u8  channel)
/*
 * fill fifo with TX data
 */
{
    __u8 fcnt, tcnt, i, free, f1, f2, fstat, *data;
    int remain, tx_busy = 0;
    int hdlc = 0;
    int *tx_idx = NULL;
    struct sk_buff **tx_skb = NULL;
    struct port *port = xhfc->port + (channel / 4);
    struct bchannel *bch = NULL;
    struct dchannel *dch = NULL;


    /* protect against layer2 down processes like l2l1D, l2l1B, etc */
    xhfc_port_lock(port);

    switch(channel % 4)
    {
        case 0:
        case 1:
            bch = &port->bch[channel % 4];
            tx_skb = &bch->tx_skb;
            tx_idx = &bch->tx_idx;
            tx_busy = test_bit(FLG_TX_BUSY, &bch->Flags);
            hdlc = test_bit(FLG_HDLC, &bch->Flags);
            break;
        case 2:
            dch = &port->dch;
            tx_skb = &dch->tx_skb;
            tx_idx = &dch->tx_idx;
            tx_busy = test_bit(FLG_TX_BUSY, &dch->Flags);
            hdlc = 1;
            break;
        case 3:
        default:
            xhfc_port_unlock(port);
            return;
    }
    if(!*tx_skb || !tx_busy)
    {
        xhfc_port_unlock(port);
        return;
    }

      send_buffer:
    remain =(*tx_skb)->len - *tx_idx;
    if(remain <= 0)
    {
        xhfc_port_unlock(port);
        return;
    }

    xhfc_selfifo(xhfc, (channel * 2));

    fstat = xhfc_read(xhfc, A_FIFO_STA);
    free = (xhfc->max_z - (xhfc_read(xhfc, A_USAGE)));
    tcnt = (remain < free) ? remain : free;

    f1 = xhfc_read(xhfc, A_F1);
    f2 = xhfc_read(xhfc, A_F2);

    fcnt = 0x07 - ((f1 - f2) & 0x07); /* free frame count in tx fifo */

    if(debug & DEBUG_HFC_FIFO_STAT)
    {
        printk(KERN_INFO
               "%s channel(%i) len(%i) idx(%i) f1(%i) f2(%i) fcnt(%i) "
               "tcnt(%i) free(%i) fstat(%i)\n",
               __FUNCTION__, channel, (*tx_skb)->len, *tx_idx,
               f1, f2, fcnt, tcnt, free, fstat);
    }

    /* check for fifo underrun during frame transmission */
    fstat = xhfc_read(xhfc, A_FIFO_STA);
    if(fstat & M_FIFO_ERR)
    {
        if(debug & DEBUG_HFC_FIFO_ERR)
        {
            printk(KERN_INFO
                   "%s transmit fifo channel(%i) underrun idx(%i),"
                   "A_FIFO_STA(0x%02x)\n",
                   __FUNCTION__, channel, *tx_idx, fstat);
        }

        xhfc_write(xhfc, A_INC_RES_FIFO, M_RES_FIFO_ERR);

        /* restart frame transmission */
        if(hdlc && *tx_idx)
        {
            *tx_idx = 0;
            goto send_buffer;
        }
    }

    if(free && fcnt && tcnt)
    {
        data = (*tx_skb)->data + *tx_idx;
        *tx_idx += tcnt;

        if(debug & DEBUG_HFC_FIFO_DATA)
        {
            printk("%s channel(%i) writing: ",
                   xhfc->name, channel);

            i = 0;
            while(i < tcnt)
                printk("%02x ", *(data + (i++)));
            printk("\n");
        }

        /* write data to FIFO */
        i = 0;
        while((i + 4) <= tcnt)
        {
            xhfc_write32(xhfc,
                         A_FIFO_DATA,
                         data + i);
            i += 4;
        }
        while((i + 1) <= tcnt)
        {
            xhfc_write(xhfc,
                       A_FIFO_DATA,
                       data[i]);
            i += 1;
        }

        /* skb data complete */
        if(*tx_idx == (*tx_skb)->len)
        {
            if(hdlc)
                xhfc_inc_f(xhfc);
            else
                xhfc_selfifo(xhfc, (channel * 2));

            /* check for fifo underrun during frame transmission */
            fstat = xhfc_read(xhfc, A_FIFO_STA);
            if(fstat & M_FIFO_ERR)
            {
                if(debug & DEBUG_HFC_FIFO_ERR)
                {
                    printk(KERN_INFO
                           "%s transmit fifo channel(%i) "
                           "underrun during transmission, "
                           "A_FIFO_STA(0x%02x)\n",
                           __FUNCTION__,
                           channel, fstat);
                }
                xhfc_write(xhfc, A_INC_RES_FIFO,
                       M_RES_FIFO_ERR);

                if(hdlc)
                {
                    // restart frame transmission
                    *tx_idx = 0;
                    goto send_buffer;
                }
            }

            dev_kfree_skb(*tx_skb);
            *tx_skb = NULL;
            if(bch)
            {
                if(get_next_bframe(bch) && !hdlc)
                    confirm_Bsend(bch);
            }
            if(dch)
                get_next_dframe(dch);

            if(*tx_skb)
            {
                if(debug & DEBUG_HFC_FIFO_DATA)
                    printk(KERN_INFO
                           "channel(%i) has next_tx_frame\n",
                           channel);
                if((free - tcnt) > 8)
                {
                    if(debug & DEBUG_HFC_FIFO_DATA)
                        printk(KERN_INFO
                               "channel(%i) continue "
                               "B-TX immediatetly\n",
                               channel);
                    goto send_buffer;
                }
            }

        }
        else
        {
            /* tx buffer not complete, but fifo filled to maximum */
            xhfc_selfifo(xhfc, (channel * 2));
        }
    }
    xhfc_port_unlock(port);
}


static void  xhfc_read_fifo(
               struct xhfc *xhfc,
                      __u8  channel)
/*
 * read RX data out of fifo
 */
{
    __u8 f1 = 0, f2 = 0, z1 = 0, z2 = 0;
    __u8 fstat = 0;
    int i;
    int rcnt; /* read rcnt bytes out of fifo */
    __u8 *data; /* new data pointer */
    struct sk_buff **rx_skb = NULL;    /* data buffer for upper layer */
    struct port *port = xhfc->port + (channel / 4);
    struct bchannel *bch = NULL;
    struct dchannel *dch = NULL;
    struct dchannel *ech = NULL;
    int maxlen = 0, hdlc = 0;


    /* protect against layer2 down processes like l2l1D, l2l1B, etc */
    xhfc_port_lock(port);

    switch(channel % 4)
    {
        case 0:
        case 1:
            bch = &port->bch[channel % 4];
            rx_skb = &bch->rx_skb;
            hdlc = test_bit(FLG_HDLC, &bch->Flags);
            maxlen = bch->maxlen;
            break;
        case 2:
            dch = &port->dch;
            rx_skb = &dch->rx_skb;
            hdlc = 1;
            maxlen = dch->maxlen;
            break;
        case 3:
            ech = &port->ech;
            rx_skb = &ech->rx_skb;
            maxlen = ech->maxlen;
            hdlc = 1;
            break;
        default:
            xhfc_port_unlock(port);
            return;
    }

      receive_buffer:
    xhfc_selfifo(xhfc, (channel * 2) + 1);

    fstat = xhfc_read(xhfc, A_FIFO_STA);
    if(fstat & M_FIFO_ERR)
    {
        if(debug & DEBUG_HFC_FIFO_ERR)
            printk(KERN_INFO
                   "RX fifo overflow channel(%i), "
                   "A_FIFO_STA(0x%02x) f0cnt(%i)\n",
                   channel, fstat, xhfc->f0_akku);
        xhfc_write(xhfc, A_INC_RES_FIFO, M_RES_FIFO_ERR);
    }

    if(hdlc)
    {
        /* hdlc rcnt */
        f1 = xhfc_read(xhfc, A_F1);
        f2 = xhfc_read(xhfc, A_F2);
        z1 = xhfc_read(xhfc, A_Z1);
        z2 = xhfc_read(xhfc, A_Z2);

        rcnt = (z1 - z2) & xhfc->max_z;
        if(f1 != f2)
            rcnt++;

    }
    else
    {
        /* transparent rcnt */
        rcnt = xhfc_read(xhfc, A_USAGE) - 1;
    }

    if(debug & DEBUG_HFC_FIFO_STAT)
    {
        if(*rx_skb)
            i = (*rx_skb)->len;
        else
            i = 0;
        printk(KERN_INFO "reading %i bytes channel(%i) "
               "irq_cnt(%i) fstat(%i) idx(%i) f1(%i) f2(%i) "
               "z1(%i) z2(%i)\n",
               rcnt, channel, xhfc->irq_cnt, fstat, i, f1, f2, z1,
               z2);
    }

    if(rcnt > 0)
    {
        if(!(*rx_skb))
        {
            *rx_skb = mI_alloc_skb(maxlen, GFP_ATOMIC);
            if(!(*rx_skb))
            {
                printk(KERN_INFO "%s: No mem for rx_skb\n",
                       __FUNCTION__);
                xhfc_port_unlock(port);
                return;
            }
        }
        data = skb_put(*rx_skb, rcnt);

        /* read data from FIFO */
        i = 0;
        while((i + 4) <= rcnt)
        {
            xhfc_read32(xhfc,
                        A_FIFO_DATA,
                        data + i);
            i += 4;
        }
        while((i + 1) <= rcnt)
        {
            data[i] = xhfc_read(xhfc,
                                A_FIFO_DATA);
            i += 1;
        }
    }
    else
    {
        xhfc_port_unlock(port);
        return;
    }

    if(hdlc)
    {
        if(f1 != f2)
        {
            xhfc_inc_f(xhfc);

            if(debug & DEBUG_HFC_FIFO_DATA)
            {
                printk(KERN_INFO
                       "channel(%i) new RX len(%i): ",
                       channel, (*rx_skb)->len);
                i = 0;
                printk("  ");
                while(i < (*rx_skb)->len)
                    printk("%02x ",
                           (*rx_skb)->data[i++]);
                printk("\n");
            }

            /* check minimum frame size */
            if((*rx_skb)->len < 4)
            {
                if(debug & DEBUG_HFC_FIFO_ERR)
                    printk(KERN_INFO
                           "%s: frame in channel(%i) "
                           "< minimum size\n",
                           __FUNCTION__, channel);
                goto read_exit;
            }

            /* check crc */
            if((*rx_skb)->data[(*rx_skb)->len - 1])
            {
                if(debug & DEBUG_HFC_FIFO_ERR)
                    printk(KERN_INFO
                           "%s: channel(%i) CRC-error\n",
                           __FUNCTION__, channel);
                goto read_exit;
            }

            /* remove cksum */
            skb_trim(*rx_skb, (*rx_skb)->len - 3);

            /* send PH_DATA_IND */
            if(bch)
                recv_Bchannel(bch, MISDN_ID_ANY);
            if(dch)
                recv_Dchannel(dch);
            if(ech)
                recv_Echannel(ech, &port->dch);

              read_exit:
            if(*rx_skb)
                skb_trim(*rx_skb, 0);
            if(xhfc_read(xhfc, A_USAGE) > 8)
            {
                if(debug & DEBUG_HFC_FIFO_DATA)
                    printk(KERN_INFO
                           "%s: channel(%i) continue \n",
                           __FUNCTION__, channel);
                goto receive_buffer;
            }
            xhfc_port_unlock(port);
            return;


        }
        else
        {
            xhfc_selfifo(xhfc, (channel * 2) + 1);
        }
    }
    else
    {
        xhfc_selfifo(xhfc, (channel * 2) + 1);
        if(bch && ((*rx_skb)->len >= 128))
            recv_Bchannel(bch, MISDN_ID_ANY);
    }
    xhfc_port_unlock(port);
}


static void  xhfc_bh_handler(
#ifdef XHFC_BH_TASKLET
              unsigned long  ul_hw)
#else
                struct xhfc *xhfc)
#endif /* XHFC_BH_TASKLET */
/*
 * IRQ work bottom half
 */
{
#ifdef XHFC_BH_TASKLET
    struct xhfc *xhfc = (struct xhfc *)ul_hw;
#else /* !XHFC_BH_TASKLET */
    queue_work(xhfc->workqueue,
              &xhfc->bh_handler_workstructure);
    return;
}


void  xhfc_bh_handler_workfunction(
               struct work_struct *ipWork)
{
    xhfc_t *xhfc = container_of(ipWork,
                                xhfc_t,
                                bh_handler_workstructure);
#endif /* XHFC_BH_TASKLET */
    int i;
    __u8 su_state;
    struct dchannel *dch;

    down_interruptible(&xhfc->lock);


    /* timer interrupt */
    if(GET_V_TI_IRQ(xhfc->misc_irq))
    {
        xhfc->misc_irq &= (__u8) (~M_TI_IRQ);

        /* handle NT Timer */
        for(i = 0; i < xhfc->num_ports; i++)
        {
            if((xhfc->port[i].mode & PORT_MODE_NT)
                && (xhfc->port[i].
                timers & NT_ACTIVATION_TIMER))
            {
                if((--xhfc->port[i].nt_timer) < 0)
                    xhfc_ph_state(&xhfc->port[i].dch);
            }
        }
    }

    /* Handle tx Fifos */
    for(i = 0; i < xhfc->channels; i++)
    {
        if(!(xhfc->fifo_irqmsk & (1 << (i * 2))))
            continue;

        xhfc->fifo_irq &= ~(1 << (i * 2));

        xhfc_write_fifo(xhfc, i);
    }

    /* Handle rx Fifos */
    for(i = 0; i < xhfc->channels; i++)
    {
        if(!(xhfc->fifo_irq & (1 << (i * 2 + 1)) &
             xhfc->fifo_irqmsk))
            continue;

        xhfc->fifo_irq &= ~(1 << (i * 2 + 1));

        xhfc_read_fifo(xhfc, i);
    }

    /* su interrupt */
    if(xhfc->su_irq & xhfc->su_irqmsk)
    {
        xhfc->su_irq = 0;
        for(i = 0; i < xhfc->num_ports; i++)
        {
            xhfc_write(xhfc, R_SU_SEL, i);
            su_state = xhfc_read(xhfc, A_SU_RD_STA);
            dch = &xhfc->port[i].dch;
            if(GET_V_SU_STA(su_state) != dch->state)
            {
                dch->state = GET_V_SU_STA(su_state);
                xhfc_ph_state(dch);
            }
        }
    }

    up(&xhfc->lock);

    return;
}


irqreturn_t  xhfc_su_irq_handler(
                    struct xhfc *xhfc)
/*
 * Interrupt handler
 */
{
    __u8 j;
#ifdef USE_F0_COUNTER
    __u32 f0_cnt;
#endif



enum
{
    R_IRQ_OVIEW_OFFSET,
    R_MISC_IRQ_OFFSET,
    R_SU_IRQ_OFFSET,
    R_FIFO_BL0_IRQ_OFFSET,
    R_FIFO_BL1_IRQ_OFFSET
};

__u8 const  laRegAddr[] =
{
    R_IRQ_OVIEW,
    R_MISC_IRQ,
    R_SU_IRQ,
    R_FIFO_BL0_IRQ,
    R_FIFO_BL1_IRQ
};

__u8  laValues[sizeof(laRegAddr) * 4];



/*
    laValues[0] = 0xA0;
    laValues[1] = 0xA1;
    laValues[2] = 0xA2;
    laValues[3] = 0xA3;

    xhfc_write32(xhfc,
                 A_FIFO_DATA,
                 laValues);

    xhfc_array_read(xhfc,
                    R_IRQ_OVIEW,
                    laValues,
                    1);
 */
    if(xhfc_reg_read(xhfc,
                     laRegAddr,
                     laValues,
                     5))
        return(0);



    if(!laValues[R_IRQ_OVIEW_OFFSET] ||
       !GET_V_GLOB_IRQ_EN(xhfc->irq_ctrl))
    {
        printk(KERN_WARNING "%s got spurious interrupt\n",
               xhfc->name);
        return(0);
    }

    xhfc->misc_irq |= laValues[R_MISC_IRQ_OFFSET];
    xhfc->su_irq |= laValues[R_SU_IRQ_OFFSET];

    /* get fifo IRQ states in bundle */
    for(j = 0; j < xhfc->num_ports; j++)
    {
        xhfc->fifo_irq |= laValues[R_FIFO_BL0_IRQ_OFFSET + j] << (j * 8);
    }

    /* call bottom half at events
     *   - Timer Interrupt (or other misc_irq sources)
     *   - SU State change
     *   - Fifo FrameEnd interrupts (only at rx fifos enabled)
     */
    if((xhfc->misc_irq & xhfc->misc_irqmsk) ||
       (xhfc->su_irq & xhfc->su_irqmsk) ||
       (xhfc->fifo_irq & xhfc->fifo_irqmsk))
    {

        /* queue bottom half */
        if(!(xhfc->testirq))
#ifdef XHFC_BH_TASKLET
            tasklet_schedule(&xhfc->tasklet);
#else
            xhfc_bh_handler(xhfc);
#endif
        /* count irqs */
        xhfc->irq_cnt++;

#ifdef USE_F0_COUNTER
        /* akkumulate f0 counter diffs */
        f0_cnt = xhfc_read(xhfc, R_F0_CNTL);
        f0_cnt += xhfc_read(xhfc, R_F0_CNTH) << 8;
        xhfc->f0_akku += (f0_cnt - xhfc->f0_cnt);
        if((f0_cnt - xhfc->f0_cnt) < 0)
            xhfc->f0_akku += 0xFFFF;
        xhfc->f0_cnt = f0_cnt;
#endif
        return IRQ_HANDLED;
    }
    else
        return IRQ_NONE;
}
#if 0
/*
 * Interrupt handler
 */
irqreturn_t
xhfc_interrupt(int intno, void *dev_id)
{
    struct xhfc_pi *pi = dev_id;
    struct xhfc *xhfc = NULL;
    __u8 i, j;
    __u32 xhfc_irqs;
#ifdef USE_F0_COUNTER
    __u32 f0_cnt;
#endif

    xhfc_irqs = 0;
    for(i = 0; i < pi->driver_data.num_xhfcs; i++)
    {
        xhfc = &pi->xhfc[i];
        if(GET_V_GLOB_IRQ_EN(xhfc->irq_ctrl)
            && (xhfc_read(xhfc, R_IRQ_OVIEW)))
            /* mark this xhfc possibly had irq */
            xhfc_irqs |= (1 << i);
    }
    if(!xhfc_irqs)
    {
        if(debug & DEBUG_HFC_IRQ)
            printk(KERN_INFO
                   "%s %s NOT M_GLOB_IRQ_EN or R_IRQ_OVIEW \n",
                   xhfc->name, __FUNCTION__);
        return IRQ_NONE;
    }

    xhfc_irqs = 0;
    for(i = 0; i < pi->driver_data.num_xhfcs; i++)
    {
        xhfc = &pi->xhfc[i];

        xhfc->misc_irq |= xhfc_read(xhfc, R_MISC_IRQ);
        xhfc->su_irq |= xhfc_read(xhfc, R_SU_IRQ);

        /* get fifo IRQ states in bundle */
        for(j = 0; j < 4; j++)
            xhfc->fifo_irq |=
                (xhfc_read(xhfc, R_FIFO_BL0_IRQ + j) <<
                 (j * 8));

        /* call bottom half at events
         *   - Timer Interrupt (or other misc_irq sources)
         *   - SU State change
         *   - Fifo FrameEnd interrupts (only at rx fifos enabled)
         */
        if((xhfc->misc_irq & xhfc->misc_irqmsk)
            || (xhfc->su_irq & xhfc->su_irqmsk)
            || (xhfc->fifo_irq & xhfc->fifo_irqmsk))
        {

            /* mark this xhfc really had irq */
            xhfc_irqs |= (1 << i);

            /* queue bottom half */
            if(!(xhfc->testirq))
                tasklet_schedule(&xhfc->tasklet);

            /* count irqs */
            xhfc->irq_cnt++;

#ifdef USE_F0_COUNTER
            /* akkumulate f0 counter diffs */
            f0_cnt = xhfc_read(xhfc, R_F0_CNTL);
            f0_cnt += xhfc_read(xhfc, R_F0_CNTH) << 8;
            xhfc->f0_akku += (f0_cnt - xhfc->f0_cnt);
            if((f0_cnt - xhfc->f0_cnt) < 0)
                xhfc->f0_akku += 0xFFFF;
            xhfc->f0_cnt = f0_cnt;
#endif
        }
    }

    return((xhfc_irqs) ? IRQ_HANDLED : IRQ_NONE);
}
#endif

/* EOF */
