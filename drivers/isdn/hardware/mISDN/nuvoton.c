/*
 *
 * Low level driver for Nuvoton Analog Interface
 *
 * Copyright 2012  by Andreas Eversberg <jolly@eversberg.eu>
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
 * Module options:
 *
 * debug:
 *	NOTE: only one poll value must be given for all cards
 *	See nuvoton.h for debug flags.
 *
 */

#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/mISDNhw.h>
#include <linux/slab.h>

#include "nuvoton.h"
#include "prox/nxapi.h"

#undef KERN_DEBUG
#define KERN_DEBUG KERN_INFO

void confirm_Bsend(struct bchannel *bch);
static void ring_on(nuvoton_inst_t *inst);

//#define CID_TEST

/*
 * module parameters:
 */

/* debugging flags */
static uint debug;
//static uint debug = DEBUG_NUVOTON_DTMF;
/* set to use u-law intead of a-law */
static int ulaw;
/* set to use V23 instead of Bell202 to generate FSK caller ID */
static int v23_cid;
/* set DTMF caller ID istead of FSK */
static int dtmf_cid;
/* send caller ID prior first ring, using LR-AS */
static int dtmf_lras = 1;
/* volume of DMTF tones (0 is to weak, 2 is maximum valume) */
static int dtmf_db = 1;
/* set to voltage when on hook */
static int voh = 60;
/* impedance */
static int impedance = IMZ_Germany_Legacy;
/* ring tone */
static int ring_tone = RING_GERMANY;
/* flash key support */
static int flash = 1;
/* gain */
static int txgain = -10, rxgain;
/* fskgain */
static int fskgain = -14;

MODULE_AUTHOR("Andreas Eversberg/Nuvoton");
MODULE_LICENSE("GPL");
module_param(debug, uint, S_IRUGO | S_IWUSR);
module_param(ulaw, int, S_IRUGO | S_IWUSR);
module_param(v23_cid, int, S_IRUGO | S_IWUSR);
module_param(dtmf_cid, int, S_IRUGO | S_IWUSR);
module_param(dtmf_lras, int, S_IRUGO | S_IWUSR);
module_param(dtmf_db, int, S_IRUGO | S_IWUSR);
module_param(voh, int, S_IRUGO | S_IWUSR);
module_param(impedance, int, S_IRUGO | S_IWUSR);
module_param(ring_tone, int, S_IRUGO | S_IWUSR);
module_param(flash, int, S_IRUGO | S_IWUSR);
module_param(txgain, int, S_IRUGO | S_IWUSR);
module_param(rxgain, int, S_IRUGO | S_IWUSR);
module_param(fskgain, int, S_IRUGO | S_IWUSR);

struct list_head nuvoton_list;

#define TIMEOUT_10MS		(HZ / 100)
#define HOOK_FLASH_MAX		(HZ * 310 / 1000) /* 310 ms */
#define HOOK_FLASH_MIN		(HZ * 80 / 1000) /* 80 ms */
#define PULSE_WAIT_MAX		(HZ * 300 / 1000) /* 300 ms */
#define DEBOUNCE		1 /* 20ms time to debounce */

#define INSTNAME		inst->name

struct timer_list nuvoton_timer;

static void nuvoton_bh(struct work_struct *work);
static void cw_off(nuvoton_inst_t *inst);
static int dch_indicate(struct dchannel *dch, uint32_t cont,
	uint8_t *data, int len);

/*
 * SPI register access
 */

NXFUNIMPSTR int
n68x386ReadReg(__chip_inst pInst, __chip_chnl chnl, int regAddr)
{
	nuvoton_inst_t *inst = container_of(pInst, nuvoton_inst_t, chip_inst);

	return inst->read_spi(inst, regAddr);
}

NXFUNIMPSTR int
n68x386WriteReg(__chip_inst pInst, __chip_chnl chnl, int regAddr, int regVal)
{
	nuvoton_inst_t *inst = container_of(pInst, nuvoton_inst_t, chip_inst);

	return inst->write_spi(inst, regAddr, regVal);
}

/* this function was taken from ProX API */
NXFUNIMPSTR int
n68x386WriteRegs(__chip_inst pInst, __chip_chnl chnl, int regStartAddr,
	const unsigned char *vals, int cnt)
{
	int i;

	for(i = 0; i < cnt; i++)
		n68x386WriteReg(pInst, chnl, regStartAddr + i, vals[i]);

	return 0;
}

/*
 * IRQ
 */

irqreturn_t
mISDN_nuvoton_isr(int irq, void *dev_id)
{
	printk(KERN_DEBUG "IRQ\n");
	return IRQ_HANDLED;
	return IRQ_NONE;
}

EXPORT_SYMBOL(mISDN_nuvoton_isr);

/*
 * reset RING/CW/FSK
 */

static void go_onhook(nuvoton_inst_t *inst)
{
	if (inst->state == POTS_STATE_RINGING) {
		if (debug & DEBUG_NUVOTON_RINGING)
			printk(KERN_DEBUG "%s: stop ringing\n", INSTNAME);
		nxRing(&inst->chip_inst, inst->chip_chnl, 0, 0, 0);
	}

	if (inst->fsk.send) {
		nxStopTones(&inst->chip_inst, inst->chip_chnl);
		if (inst->fsk.started) {
			nxFSKGenSetup(&inst->chip_inst, inst->chip_chnl,
                                PROX_FSKG_DISABLE);
		}
		inst->fsk.send = 0;
		if (inst->fsk.dtas == 2)
			nxDTMFDEnable(&inst->chip_inst, inst->chip_chnl, 0);
		inst->fsk.dtas = 0;
	}

	if (inst->dtmf.send) {
		nxStopTones(&inst->chip_inst, inst->chip_chnl);
		inst->dtmf.send = 0;
	}

	inst->state = POTS_STATE_ONHOOK;
	inst->line.mute = POTS_UNMUTE;
	cw_off(inst);
}

/*
 * battery control
 */

static void
battery_off(nuvoton_inst_t *inst)
{
	go_onhook(inst);
	nxBATOn(&inst->chip_inst, inst->chip_chnl, 0);
}

static void
battery_on(nuvoton_inst_t *inst)
{
	nxBATOn(&inst->chip_inst, inst->chip_chnl, 1);
}

/*
 * FSK encoding
 */

static unsigned char seize_mark[60] =
{
	0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
	0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
	0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
	0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
	0xFA,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,
};


static uint8_t
*add_ie(uint8_t *p, uint8_t type, int len, uint8_t *val)
{
	*p++ = type;
	*p++ = len;
	memcpy(p, val, len);

	return p + len;
}

static void
set_fsk_cid(nuvoton_inst_t *inst, uint8_t *cid, int len, int knock)
{
	uint8_t *p;
	uint8_t par = 0;
	int i;
	uint32_t on_time;

	if (!len)
		return;

	/* add IE */
	p = inst->fsk.buff + 2;
	p = add_ie(p, 0x02, len, cid);
	len = p - inst->fsk.buff - 2;
	/* preceed type+length */
	inst->fsk.buff[0] = (knock) ? 0x82 : 0x80;
	inst->fsk.buff[1] = len;
	len += 2;
	/* append parity */
	p = inst->fsk.buff;
	for(i = 0; i < len; i++)
		par += *p++;
	par = (~par) + 1;
	*p++ = par;
	len++;

	inst->fsk.len = len;
	if (knock) {
		/* CW - 100ms - DT-AS - wait ack - FSK */
		on_time = 700; /* DTAS start 100ms after CW tone */
		inst->fsk.dtas = 1;
	} else {
		/* RING - 500ms - FSK - ... - 2nd RING */
		on_time = inst->fsk.on_time;
		if (on_time == 0)
			on_time = 1500; /* in case the API does not return
					 * on-time */
		on_time += 500; /* CID start 500ms after first ring */
	}
	if (debug & DEBUG_NUVOTON_RINGING)
		printk(KERN_DEBUG "%s: Start CID after %dms, len=%d\n",
			INSTNAME, on_time, inst->fsk.len);
	inst->fsk.jiffies = jiffies;
	inst->fsk.pos = (knock) ? 38 : 0;
	inst->fsk.started = 0;
	inst->fsk.send = HZ * on_time / 1000;
}

static inline void
send_fsk(nuvoton_inst_t *inst)
{
	int written;
	int digit;

	if (inst->fsk.dtas == 2) {
		digit = nxDTMFDGetData(&inst->chip_inst, inst->chip_chnl);
		if (digit >= 'A' && digit <= 'D') {
			if (debug & DEBUG_NUVOTON_FSK)
				printk(KERN_DEBUG "CID: stop DT-AS due to DTMF "
					"'%c' detection\n", digit);
			nxStopTones(&inst->chip_inst, inst->chip_chnl);
			inst->fsk.jiffies = jiffies;
			inst->fsk.send = HZ * 200 / 1000;
			inst->fsk.dtas = 0;
			nxDTMFDEnable(&inst->chip_inst, inst->chip_chnl, 0);
			return;
		}
	}

	if (!inst->fsk.send)
		return;
	if (((uint32_t)jiffies - inst->fsk.jiffies) < inst->fsk.send)
		return;

	if (inst->fsk.dtas == 1) {
		if (debug & DEBUG_NUVOTON_FSK)
			printk(KERN_DEBUG "CID: send DT-AS\n");
		inst->line.mute = POTS_MUTE;
		/* FIXME: -15 db */
		nxToneGen(&inst->chip_inst, inst->chip_chnl, 21959, 24323/64,
			15447, 28900/64, 100, 8000);
		inst->fsk.jiffies = jiffies;
		inst->fsk.send = HZ * 260 / 1000; /* 100 on + 160 off */
		inst->fsk.dtas = 2;
		nxDTMFDEnable(&inst->chip_inst, inst->chip_chnl, 1);
		return;
	}
	if (inst->fsk.dtas == 2) {
		if (debug & DEBUG_NUVOTON_FSK)
			printk(KERN_DEBUG "CID: stop DT-AS due to timeout\n");
		nxStopTones(&inst->chip_inst, inst->chip_chnl);
		inst->fsk.send = 0;
		inst->fsk.dtas = 0;
		nxDTMFDEnable(&inst->chip_inst, inst->chip_chnl, 0);
		inst->line.mute = POTS_UNMUTE_WAIT;
		return;
	}

	if (!inst->fsk.started) {
		if (debug & DEBUG_NUVOTON_FSK)
			printk(KERN_DEBUG "CID: setup FSK\n");
		nxFSKGenSetup(&inst->chip_inst, inst->chip_chnl,
			      PROX_FSKG_ENABLE
			    | ((v23_cid) ? PROX_FSKG_ITUTV23
							: PROX_FSKG_BELL202)
			    | PROX_FSKG_MODE_RAW
			    | PROX_FSKG_POL_NORMAL);
		nxFSKGenSetLevel(&inst->chip_inst, inst->chip_chnl,
			fskgain - txgain);
		inst->fsk.seize_mark = 1;
		inst->fsk.flush = 0;
	}

	/* when buffer is empty, we wait for next timer period, to be sure
	 * that the current shift register is completely sent
	 */
	if (inst->fsk.flush) {
		inst->fsk.flush = 0;
		if (inst->fsk.seize_mark) {
			nxFSKGenSetup(&inst->chip_inst, inst->chip_chnl,
				PROX_FSKG_1STOPBIT |
				PROX_FSKG_PARITY_NONE |
				PROX_FSKG_MODE_PACKED);
			inst->fsk.seize_mark = 0;
			inst->fsk.pos = 0;
		} else {
			nxFSKGenSetup(&inst->chip_inst, inst->chip_chnl,
				PROX_FSKG_DISABLE);
			inst->fsk.send = 0;
			inst->line.mute = POTS_UNMUTE_WAIT;
			return;
		}
	}

	if (inst->fsk.seize_mark)
		written = nxFSKGenWrite(&inst->chip_inst, inst->chip_chnl,
			seize_mark + inst->fsk.pos,
			sizeof(seize_mark) - inst->fsk.pos);
	else
		written = nxFSKGenWrite(&inst->chip_inst, inst->chip_chnl,
			inst->fsk.buff + inst->fsk.pos,
			inst->fsk.len - inst->fsk.pos);

	if (written < 0) {
		printk(KERN_NOTICE "CID: failed to write FIFO\n");
		nxFSKGenSetup(&inst->chip_inst, inst->chip_chnl,
				PROX_FSKG_DISABLE);
		inst->fsk.send = 0;
		return;
	}

	if (debug & DEBUG_NUVOTON_FSK) {
		if (inst->fsk.seize_mark && written > 0) {
			int i;
			printk(KERN_DEBUG "Written:");
			for (i = 0; i < written; i++)
				printk(" %02x", seize_mark[inst->fsk.pos + i]);
			printk("\n");
		}
		if (!inst->fsk.seize_mark && written > 0) {
			int i;
			printk(KERN_DEBUG "Written:");
			for (i = 0; i < written; i++)
				printk(" %02x",
					inst->fsk.buff[inst->fsk.pos + i]);
			printk("\n");
		}
	}

	inst->fsk.pos += written;

	if (!inst->fsk.seize_mark && inst->fsk.pos >= inst->fsk.len) {
		if (written == 0
		 && nxFSKGenIsBusy(&inst->chip_inst, inst->chip_chnl) <= 0) {
			if (debug & DEBUG_NUVOTON_FSK)
				printk(KERN_DEBUG "CID: data done, FIFO "
					"empty\n");
			inst->fsk.flush = 1;
		} else {
			if (debug & DEBUG_NUVOTON_FSK)
				printk(KERN_DEBUG "CID: done, wait for FIFO "
					"empty\n");
		}
	}
	if (inst->fsk.seize_mark && inst->fsk.pos >= sizeof(seize_mark)) {
		if (written == 0
		 && nxFSKGenIsBusy(&inst->chip_inst, inst->chip_chnl) <= 0) {
			if (debug & DEBUG_NUVOTON_FSK)
				printk(KERN_DEBUG "CID: seize/mark done, FIFO "
					"empty\n");
			inst->fsk.flush = 1;
		} else {
			if (debug & DEBUG_NUVOTON_FSK)
				printk(KERN_DEBUG "CID: done, wait for FIFO "
					"empty\n");
		}
	}

	if (!inst->fsk.started) {
		if (debug & DEBUG_NUVOTON_FSK)
			printk(KERN_DEBUG "CID: start FSK\n");
		nxFSKGenSetup(&inst->chip_inst, inst->chip_chnl,
			PROX_FSKG_START);
		inst->fsk.started = 1;
	}
}

/*
 * DTMF encoding
 */

static void
set_dtmf_cid(nuvoton_inst_t *inst, uint8_t *cid, int len)
{
	uint32_t on_time;

	if (!len)
		return;

	inst->dtmf.buff[0] = 'D';
	inst->dtmf.buff[len + 1] = 'C';
	memcpy(inst->dtmf.buff + 1, cid, len);
	inst->dtmf.len = len + 2;

	if (dtmf_lras) {
		/* LR-AS - 300ms - DTMF - 350ms - RING */
		on_time = 300;
		nxSetLineStatus(&inst->chip_inst, inst->chip_chnl,
			LS_REVERSE_ONHOOKTRANS);
	} else {
		/* RING - 500ms - DTMF - 1s - RING */
		on_time = inst->dtmf.on_time;
		if (on_time == 0)
			on_time = 1500; /* in case the API does not return on-time */
		on_time += 200; /* CID start 200ms after first ring (later wait another 300 ms) */
		inst->dtmf.init = 1;
	}
	if (debug & DEBUG_NUVOTON_RINGING)
		printk(KERN_DEBUG "%s: Start CID after %dms, len=%d\n",
			INSTNAME, on_time, inst->dtmf.len);
	inst->dtmf.jiffies = jiffies;
	inst->dtmf.pos = 0;
	inst->dtmf.send = HZ * on_time / 1000;
	inst->dtmf.done = 0;
}

static inline void
send_dtmf(nuvoton_inst_t *inst)
{
	int digit;

	if (!inst->dtmf.send)
		return;
	if (((uint32_t)jiffies - inst->dtmf.jiffies) < inst->dtmf.send)
		return;

	if (inst->dtmf.done) {
		/* done waiting, so we continue ringing */
		if (debug & DEBUG_NUVOTON_DTMF)
			printk(KERN_DEBUG "CID: continue to ring\n");
		inst->dtmf.send = 0;
		nxRing(&inst->chip_inst, inst->chip_chnl, 0, 0, 0);
		ring_on(inst);
		return;
	}

	/* not required for LR-AS, because line state is already set */
	if (inst->dtmf.init) {
		/* change to a state that allows to send DTMF */
		if (debug & DEBUG_NUVOTON_DTMF)
			printk(KERN_DEBUG "CID: change line state\n");
		nxRing(&inst->chip_inst, inst->chip_chnl, 0, 0, 0);
		nxSetLineStatus(&inst->chip_inst, inst->chip_chnl,
			LS_FORWARD_ONHOOKTRANS);
		inst->dtmf.init = 0;
		inst->dtmf.jiffies = jiffies;
		inst->dtmf.send = HZ * 300 / 1000; /* 300ms before start */
		inst->dtmf.pos++;
		return;
	}

	if (inst->dtmf.pos == inst->dtmf.len) {
		/* we are done, so wait some time and continue ringing */
		if (debug & DEBUG_NUVOTON_DTMF)
			printk(KERN_DEBUG "CID: DTMF done, wait for "
				"(next) ring\n");
		nxStopTones(&inst->chip_inst, inst->chip_chnl);
		inst->dtmf.done = 1;
		inst->dtmf.jiffies = jiffies;
		if (dtmf_lras)
			inst->dtmf.send = HZ * 280 / 1000; /* ring 350-70 ms later */
		else
			inst->dtmf.send = HZ * 1;
		return;
	}

	/* send (next) digit */
	digit = inst->dtmf.buff[inst->dtmf.pos];
	if (debug & DEBUG_NUVOTON_DTMF)
		printk(KERN_DEBUG "CID: Send DTMF digit '%c'\n", digit);
	nxDTMFGen(&inst->chip_inst, inst->chip_chnl, digit, 70, 8000, dtmf_db);
	inst->dtmf.jiffies = jiffies;
	inst->dtmf.send = HZ * 140 / 1000; /* 70 on, 70 off */
	inst->dtmf.pos++;
}

/*
 * CW tone
 */

static void
cw_on(nuvoton_inst_t *inst)
{
	if (inst->state != POTS_STATE_OFFHOOK)
		return;
	if (debug & DEBUG_NUVOTON_CW)
		printk(KERN_DEBUG "%s: enable CW tone\n", INSTNAME);

	inst->cw.enabled = 1;
	inst->cw.on = 0;
	inst->cw.count = 0;
}

static void
cw_off(nuvoton_inst_t *inst)
{
	inst->cw.enabled = 0;
	if (inst->cw.on) {
		if (debug & DEBUG_NUVOTON_CW)
			printk(KERN_DEBUG "%s: abort CW tone\n", INSTNAME);
		nxStopTones(&inst->chip_inst, inst->chip_chnl);
	}
}

static inline void
send_cw(nuvoton_inst_t *inst)
{
	/* call waiting is disabled */
	if (!inst->cw.enabled)
		return;

	/* complete loop is 200ms+200ms+200ms+5000ms */
	if (inst->cw.count == 0 || inst->cw.count == 40) {
		if (debug & DEBUG_NUVOTON_CW)
			printk(KERN_DEBUG "%s: start CW tone\n", INSTNAME);
		nxToneGen(&inst->chip_inst, inst->chip_chnl, 32314, 0x400,
			32314, 0x0000, 200, 1000);
		inst->cw.on = 1;
	/* stop tone 650ms after knocking. if dtas is set, fsk process will
	 * stop tone. */
	} else if (inst->cw.count == 65 && !inst->fsk.dtas) {
		if (debug & DEBUG_NUVOTON_CW)
			printk(KERN_DEBUG "%s: stop CW tone\n", INSTNAME);
		nxStopTones(&inst->chip_inst, inst->chip_chnl);
		inst->cw.on = 0;
	}
	if (++inst->cw.count >= 560)
		inst->cw.count = 0;
}

/*
 * RING control
 */

static void ring_on(nuvoton_inst_t *inst)
{
	if (inst->state != POTS_STATE_ONHOOK
	 && inst->state != POTS_STATE_RINGING)
		return;

	/* setup ring interval. this overwrites last tone interval */
	inst->fsk.on_time = nxSetupRingtone(&inst->chip_inst,
		inst->chip_chnl, ring_tone);
	inst->dtmf.on_time = inst->fsk.on_time;

	nxRing(&inst->chip_inst, inst->chip_chnl, 1, 0, 0);
	inst->state = POTS_STATE_RINGING;
}

/*
 * on-hook/off-hook/flash/keypulse detection
 */

static inline void
loopdetect(nuvoton_inst_t *inst, int nls)
{
	if (inst->line.wait_onhook
	 && ((uint32_t)jiffies - inst->line.hook_jiffies)
	 			> ((flash) ? HOOK_FLASH_MAX : HOOK_FLASH_MIN)) {
		if (debug & DEBUG_NUVOTON_LINE)
			printk(KERN_DEBUG "%s: on-hook\n", INSTNAME);
		dch_indicate(&inst->dch, POTS_ON_HOOK, NULL, 0);
		inst->line.wait_onhook = 0;
		inst->line.pulse = 0;
		go_onhook(inst); /* in case we are knocking */
	}
	if (inst->line.pulse
	 && ((uint32_t)jiffies - inst->line.pulse_jiffies) > PULSE_WAIT_MAX) {
		int c = POTS_KP_VAL;
		if (inst->line.pulse <= 9)
			c+= '0' + inst->line.pulse;
		else if (inst->line.pulse == 10)
			c+= '0';
		else if (inst->line.pulse == 11)
			c+= '*';
		else if (inst->line.pulse == 12)
			c+= '#';
		else if (inst->line.pulse <= 38)
			c+= 'A' + inst->line.pulse - 13;
		else
			c+= '-';
		if (debug & DEBUG_NUVOTON_LINE)
			printk(KERN_DEBUG "%s: pulse-to: %c\n", INSTNAME,
				c & 0xff);
		inst->line.pulse = 0;
		inst->line.mute = POTS_UNMUTE_WAIT;
		dch_indicate(&inst->dch, c, NULL, 0);
	}
	if (nls == inst->line.status)
		return;
	inst->line.status = nls;
	if ((nls & RTLC_LCD)) {
		if (inst->line.wait_onhook) {
			if (flash
			 && ((uint32_t)jiffies - inst->line.hook_jiffies)
							> HOOK_FLASH_MIN) {
				dch_indicate(&inst->dch,
					POTS_HOOK_FLASH, NULL, 0);
				if (debug & DEBUG_NUVOTON_LINE)
					printk(KERN_DEBUG "%s: flash\n",
						INSTNAME);
				inst->line.pulse = 0;
				inst->line.mute = POTS_UNMUTE_WAIT;
			} else {
				inst->line.pulse_jiffies = jiffies;
				inst->line.pulse++;
			}
		} else {
			go_onhook(inst); /* in case we are ringing */
			dch_indicate(&inst->dch, POTS_OFF_HOOK, NULL, 0);
			inst->state = POTS_STATE_OFFHOOK;
			if (debug & DEBUG_NUVOTON_LINE)
				printk(KERN_DEBUG "%s: off-hook\n", INSTNAME);
			inst->line.mute = POTS_UNMUTE_WAIT;
		}
		inst->line.wait_onhook = 0;
	} else {
		inst->line.mute = POTS_MUTE;
		inst->line.wait_onhook = 1;
		inst->line.hook_jiffies = jiffies;
	}
}

/*
 * mISDN interface
 */

static void
ph_state(struct dchannel *dch)
{
}

static int
hfc_l1callback(struct dchannel *dch, u_int cmd)
{
	return 0;
}

static void
deactivate_bchannel(struct bchannel *bch)
{
	nuvoton_inst_t *inst = bch->hw;
	u_long flags;

	spin_lock_irqsave(&inst->lock, flags);
	test_and_clear_bit(FLG_OPEN, &bch->Flags);
	mISDN_clear_bchannel(bch);
	bch->ch.protocol = ISDN_P_NONE;
	bch->ch.peer = NULL;
	inst->ph_deactivate(inst);
	spin_unlock_irqrestore(&inst->lock, flags);
}

static int
channel_bctrl(struct bchannel *bch, struct mISDN_ctrl_req *cq)
{
	int	ret = 0;

	switch (cq->op) {
	case MISDN_CTRL_GETOP:
		cq->op = 0;
		break;
	default:
		printk(KERN_WARNING "%s: unknown Op %x\n", __func__, cq->op);
		ret = -EINVAL;
		break;
	}
	return ret;
}

static int
nuvoton_bctrl(struct mISDNchannel *ch, u_int cmd, void *arg)
{
	struct bchannel *bch = container_of(ch, struct bchannel, ch);
	nuvoton_inst_t *inst = bch->hw;
	int ret = -EINVAL;

	if (bch->debug & DEBUG_HW)
		printk(KERN_DEBUG "%s: cmd:%x %p\n", __func__, cmd, arg);

	switch (cmd) {
	case CLOSE_CHANNEL:
		deactivate_bchannel(bch);
		if (inst->hw_info.module)
			module_put(inst->hw_info.module);
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

static int
nuvoton_l2l1_ctrl(nuvoton_inst_t *inst, uint32_t cont, uint8_t *data,
	uint8_t len)
{
	int			ret = -EINVAL;

	switch (cont) {
	case POTS_RING_ON:
		if (debug & DEBUG_NUVOTON_RINGING)
			printk(KERN_DEBUG "%s: received RING_ON control\n",
				INSTNAME);
		if (inst->state == POTS_STATE_ONHOOK) {
			cw_off(inst);
			if (!dtmf_lras)
				ring_on(inst);
			if (!dtmf_cid)
				set_fsk_cid(inst, data, len, 0);
			else
				set_dtmf_cid(inst, data, len);
		}
		ret = 0;
		break;
	case POTS_RING_OFF:
		if (debug & DEBUG_NUVOTON_RINGING)
			printk(KERN_DEBUG "%s: received RING_OFF control\n",
				INSTNAME);
		go_onhook(inst);
		ret = 0;
		break;
	case POTS_CW_ON:
		if (debug & DEBUG_NUVOTON_CW)
			printk(KERN_DEBUG "%s: received CW_ON control\n",
				INSTNAME);
		cw_on(inst);
		if (!dtmf_cid)
			set_fsk_cid(inst, data, len, 1);
		ret = 0;
		break;
	case POTS_CW_OFF:
		if (debug & DEBUG_NUVOTON_CW)
			printk(KERN_DEBUG "%s: received CW_OFF control\n",
				INSTNAME);
		cw_off(inst);
		ret = 0;
		break;
	}

	return ret;
}

static int
nuvoton_l2l1_d(struct mISDNchannel *ch, struct sk_buff *skb)
{
	struct mISDNdevice	*dev = container_of(ch, struct mISDNdevice, D);
	struct dchannel		*dch = container_of(dev, struct dchannel, dev);
	nuvoton_inst_t		*inst = dch->hw;
	int			ret = -EINVAL;
	struct mISDNhead	*hh = mISDN_HEAD_P(skb);

	switch (hh->prim) {
	case PH_DATA_REQ:
		break;
	case PH_ACTIVATE_REQ:
		battery_on(inst);
		ret = 0;
		break;
	case PH_DEACTIVATE_REQ:
		battery_off(inst);
		ret = 0;
		break;
	case PH_CONTROL_REQ:
		if (skb->len < 4)
			break;
		ret = nuvoton_l2l1_ctrl(inst, *((uint32_t *)skb->data),
			skb->data + 4, skb->len - 4);
		break;
	}
	if (!ret)
		dev_kfree_skb(skb);

	return ret;
}

static int
nuvoton_l2l1_b(struct mISDNchannel *ch, struct sk_buff *skb)
{
	struct bchannel		*bch = container_of(ch, struct bchannel, ch);
	nuvoton_inst_t		*inst = bch->hw;
	int			ret = -EINVAL;
	struct mISDNhead	*hh = mISDN_HEAD_P(skb);
	u_long			flags;

	switch (hh->prim) {
	case PH_DATA_REQ:
		spin_lock_irqsave(&inst->lock, flags);
		ret = bchannel_senddata(bch, skb);
		if (ret > 0) { /* direct TX */
next_t_frame:
			confirm_Bsend(bch);
			/* skb is freed here: */
			inst->tx_fifo(inst, bch->tx_skb);
			/* bch->tx_skb is overwritten/cleared here: */
			if (get_next_bframe(bch))
				goto next_t_frame;
			return 0;
		}
		spin_unlock_irqrestore(&inst->lock, flags);
		return ret;
	case PH_ACTIVATE_REQ:
		inst->ph_activate(inst);
		_queue_data(ch, PH_ACTIVATE_IND, MISDN_ID_ANY, 0, NULL,
			GFP_KERNEL);
		ret = 0;
		break;
	case PH_DEACTIVATE_REQ:
		inst->ph_deactivate(inst);
		_queue_data(ch, PH_DEACTIVATE_IND, MISDN_ID_ANY, 0, NULL,
			GFP_KERNEL);
		ret = 0;
		break;
	}
	if (!ret)
		dev_kfree_skb(skb);

	return ret;
}

static int
open_dchannel(nuvoton_inst_t *inst, struct mISDNchannel *ch,
    struct channel_req *rq)
{
	int err = 0;

	if (debug & DEBUG_HW_OPEN)
		printk(KERN_DEBUG "%s: dev(%d) open from %p\n", __func__,
		    inst->dch.dev.id, __builtin_return_address(0));

	if (rq->protocol == ISDN_P_NONE)
		return -EINVAL;
	if (rq->adr.channel != 0)
		return -EINVAL;

	if (rq->protocol == ISDN_P_FXS_POTS) {
		err = create_l1(&inst->dch, hfc_l1callback);
		if (err)
			return err;
	}
	ch->protocol = rq->protocol;

	rq->ch = ch;
	if (inst->hw_info.module) {
		if (!try_module_get(inst->hw_info.module))
			printk(KERN_WARNING "%s:cannot get module\n", __func__);
	}

	return 0;
}

static int
open_bchannel(nuvoton_inst_t *inst, struct channel_req *rq)
{
	struct bchannel		*bch;

	if (rq->adr.channel > 1)
		return -EINVAL;
	if (rq->protocol == ISDN_P_NONE)
		return -EINVAL;
	bch = &inst->bch;
	if (test_and_set_bit(FLG_OPEN, &bch->Flags))
		return -EBUSY; /* b-channel can be only open once */
	test_and_clear_bit(FLG_FILLEMPTY, &bch->Flags);
	bch->ch.protocol = rq->protocol;
	rq->ch = &bch->ch;
	if (!try_module_get(inst->hw_info.module))
		printk(KERN_WARNING "%s:cannot get module\n", __func__);

	return 0;
}

static int
nuvoton_dctrl(struct mISDNchannel *ch, u_int cmd, void *arg)
{
	struct mISDNdevice	*dev = container_of(ch, struct mISDNdevice, D);
	struct dchannel		*dch = container_of(dev, struct dchannel, dev);
	nuvoton_inst_t		*inst = dch->hw;
	struct channel_req	*rq;
	int			err = 0;

	if (dch->debug & DEBUG_HW)
		printk(KERN_DEBUG "%s: cmd:%x %p\n",
		    __func__, cmd, arg);

	switch (cmd) {
	case OPEN_CHANNEL:
		rq = arg;
		if ((rq->protocol == ISDN_P_FXS_POTS) ||
		    (rq->protocol == ISDN_P_FXO_POTS))
			err = open_dchannel(inst, ch, rq);
		else
			err = open_bchannel(inst, rq);
		break;
	case CLOSE_CHANNEL:
		if (debug & DEBUG_HW_OPEN)
			printk(KERN_DEBUG "%s: dev(%d) close from %p\n",
			    __func__, inst->dch.dev.id,
			    __builtin_return_address(0));
		battery_off(inst);
		module_put(inst->hw_info.module);
		break;
	case CONTROL_CHANNEL:
		err = -ENOTSUPP;
		break;
	default:
		if (debug & DEBUG_HW)
			printk(KERN_DEBUG "%s: unknown command %x\n",
			    __func__, cmd);
		err = -EINVAL;
	}

	return err;
}

static int
dch_indicate(struct dchannel *dch, uint32_t cont,
	uint8_t *data, int len)
{
	struct sk_buff *skb;
	struct mISDNhead *hh;

	skb = mI_alloc_skb(len + 4, GFP_ATOMIC);
	if (!skb)
		return -ENOMEM;
	hh = mISDN_HEAD_P(skb);
	hh->prim = PH_CONTROL_IND;
	hh->id = MISDN_ID_ANY;
	memcpy(skb_put(skb, 4), &cont, 4);
	if (data && len)
		memcpy(skb_put(skb, len), data, len);

	skb_queue_tail(&dch->rqueue, skb);
	schedule_event(dch, FLG_RECVQUEUE);

	return 0;
}

/*
 * instance handling
 */

nuvoton_inst_t
*mISDN_nuvoton_register(nuvoton_hw_info_t *info,
	int (*tx_fifo)(nuvoton_inst_t *inst, struct sk_buff *skb),
	int (*write_spi)(nuvoton_inst_t *inst, int reg, int value),
	int (*read_spi)(nuvoton_inst_t *inst, int reg),
	int (*ph_activate)(nuvoton_inst_t *inst),
	int (*ph_deactivate)(nuvoton_inst_t *inst))
{
	int err;
	nuvoton_inst_t *inst;
	int nuvoton_cnt = 0;

	list_for_each_entry(inst, &nuvoton_list, list) {
		if (inst->count > nuvoton_cnt)
			nuvoton_cnt = inst->count;
	}

	if (info->has_fxo) {
		printk(KERN_WARNING "FXO is not supported yet\n");
		info->has_fxo = 0;
	}
	if (!info->has_fxs && !info->has_fxo) {
		printk(KERN_WARNING "no supported mode requested\n");
		return NULL;
	}

	info->pcmopts &= ~(PROX_PCM_ULAW | PROX_PCM_ALAW);
	if (ulaw)
		info->pcmopts |= PROX_PCM_ULAW;
	else
		info->pcmopts |= PROX_PCM_ALAW;

	inst = kzalloc(sizeof(nuvoton_inst_t), GFP_KERNEL);
	if (!inst)
		return NULL;

	memcpy(&inst->hw_info, info, sizeof(nuvoton_hw_info_t));
	inst->count = nuvoton_cnt + 1;
	inst->state = POTS_STATE_ONHOOK;
	inst->tx_fifo = tx_fifo;
	inst->write_spi = write_spi;
	inst->read_spi = read_spi;
	inst->ph_activate = ph_activate;
	inst->ph_deactivate = ph_deactivate;

	inst->chip_inst.channels[0].bat_type = BAT_INDUCTOR;
	inst->chip_inst.channels[0].last_ls = LS_FORWARD_IDLE;
	inst->chip_inst.channels[0].chk_flash = flash;
	inst->chip_inst.channels[0].imz = impedance;

	printk(KERN_DEBUG "%s: reset nuvoton chip (impedance=%d, "
		"check_flash=%d)\n", __func__, inst->chip_inst.channels[0].imz,
		inst->chip_inst.channels[0].chk_flash);
	err = nxHWReset(&inst->chip_inst, inst->chip_chnl);
	if (err < 0) {
		printk(KERN_WARNING "%s: failed to reset nuvoton chip\n",
			__func__);
		goto free;
	}

	err = nxInit(&inst->chip_inst, inst->chip_chnl);
	if (err < 0) {
		printk(KERN_WARNING "%s: failed to init nuvoton chip\n",
			__func__);
		goto free;
	}

	nxSetPCM(&inst->chip_inst, inst->chip_chnl, info->pcmopts, NULL);
	if (err < 0) {
		printk(KERN_WARNING "%s: failed to set PCM\n", __func__);
		goto free;
	}
	nxSetTxTimeSlot(&inst->chip_inst, inst->chip_chnl, info->tx_valbits);
	nxSetRxTimeSlot(&inst->chip_inst, inst->chip_chnl, info->rx_valbits);

	nxIntEnable(&inst->chip_inst, inst->chip_chnl,
		PROX_INT_LC | PROX_INT_GKD);

	nxSetVGM(&inst->chip_inst, inst->chip_chnl, 4500);
	nxSetVOV(&inst->chip_inst, inst->chip_chnl, 4500);
	nxSetVOH(&inst->chip_inst, inst->chip_chnl, voh * 1000);
	nxSetVBLV(&inst->chip_inst, inst->chip_chnl, 24000);
	nxSetLoopClosureDetect(&inst->chip_inst, inst->chip_chnl, 0, 24000, 20);

	printk(KERN_DEBUG "do calibration\n");
	nxBATOn(&inst->chip_inst, inst->chip_chnl, 1);
	nxCaliberate(&inst->chip_inst, inst->chip_chnl);
#ifndef CID_TEST
	nxBATOn(&inst->chip_inst, inst->chip_chnl, 0);
#endif
	printk(KERN_DEBUG "done calibration\n");

	nxDTMFDSetInput(&inst->chip_inst, inst->chip_chnl, 1);
	nxDTMFDSetFreqError(&inst->chip_inst, inst->chip_chnl, 0);
	nxDTMFDSetDecSpeed(&inst->chip_inst, inst->chip_chnl, 5);
	nxDTMFDSetSigLevel(&inst->chip_inst, inst->chip_chnl, -350);
	nxDTMFDSetTimes(&inst->chip_inst, inst->chip_chnl, 30, 30, 50);
		/* FIXME: use correct values for real DTMF detection */

	/* be sure that fskgain is in range -24..0 */
	if (fskgain < -24) {
		fskgain = -24;
		printk(KERN_NOTICE "nuvoton: force %d as lowest fskgain\n",
			fskgain);
	}
	if (fskgain > 0) {
		fskgain = 0;
		printk(KERN_NOTICE "nuvoton: force %d as highest fskgain\n",
			fskgain);
	}
	/* be sure not to fall below FSK gain, because FSK engine cannot
	 * go above 0 db */
	if (txgain < fskgain) {
		txgain = fskgain;
		printk(KERN_NOTICE "nuvoton: force %d as lowest txgain\n",
			txgain);
	}
	/* be sure not to raise gain too high, because FSK engine cannot
	 * go below -24 db */
	if (txgain > 24 + fskgain) {
		txgain = 24 + fskgain;
		printk(KERN_NOTICE "nuvoton: force %d as highest txgain\n",
			txgain);
	}
#ifdef GAIN_STEP_0DOT5
	nxSetTxDACGain(&inst->chip_inst, inst->chip_chnl, txgain * 10);
	nxSetRxADCGain(&inst->chip_inst, inst->chip_chnl, rxgain * 10);
#else
	nxSetTxDACGain(&inst->chip_inst, inst->chip_chnl, txgain);
	nxSetRxADCGain(&inst->chip_inst, inst->chip_chnl, rxgain);
#endif

	inst->dch.debug = debug;
	spin_lock_init(&inst->lock);
	mISDN_initdchannel(&inst->dch, MAX_DFRAME_LEN_L1, ph_state);
	inst->dch.hw = inst;
	if (info->has_fxs)
		inst->dch.dev.Dprotocols |= (1 << ISDN_P_FXS_POTS);
	if (info->has_fxo)
		inst->dch.dev.Dprotocols |= (1 << ISDN_P_FXO_POTS);
	inst->dch.dev.Bprotocols = (1 << (ISDN_P_B_RAW & ISDN_P_B_MASK));
	inst->dch.dev.D.send = nuvoton_l2l1_d;
	inst->dch.dev.D.ctrl = nuvoton_dctrl;
	inst->dch.dev.nrbchan = 1;
	inst->bch.nr = 1;
	set_channelmap(1, inst->dch.dev.channelmap);
	inst->bch.debug = debug;
	mISDN_initbchannel(&inst->bch, MAX_DATA_MEM, -1);
	inst->bch.hw = inst;
	inst->bch.ch.send = nuvoton_l2l1_b;
	inst->bch.ch.ctrl = nuvoton_bctrl;
	inst->bch.ch.nr = 1;
	list_add(&inst->bch.ch.list, &inst->dch.dev.bchannels);

	INIT_WORK(&inst->workq, (void *)nuvoton_bh);

	snprintf(inst->name, MISDN_MAX_IDLEN - 1, "nuvoton.%d",
		nuvoton_cnt + 1);
	err = mISDN_register_device(&inst->dch.dev, NULL, inst->name);
	if (err)
		goto error;
	nuvoton_cnt++;
	printk(KERN_INFO "mISDN: port %d installed\n", nuvoton_cnt);

	list_add_tail(&inst->list, &nuvoton_list);

	if (!try_module_get(THIS_MODULE))
		printk(KERN_WARNING "%s:cannot get module\n", __func__);

#ifdef CID_TEST
	if (nuvoton_cnt == 2) {
		set_current_state(TASK_UNINTERRUPTIBLE);
		printk(KERN_INFO "1\n");
		mdelay(500);
		printk(KERN_INFO "2\n");
		ring_on(inst);
		if (0) {
			inst->line.status = RTLC_LCD;
			if (!dtmf_cid)
				set_fsk_cid(inst, "08154711", 8, 1);
			else
				set_dtmf_cid(inst, "08154711", 8);
		} else {
			if (!dtmf_cid)
				set_fsk_cid(inst, "08154711", 8, 0);
			else
				set_dtmf_cid(inst, "08154711", 8);
		}
	}
#endif

	return inst;

error:
	mISDN_freebchannel(&inst->bch);
	mISDN_freedchannel(&inst->dch);
free:
	kfree(inst);
	return NULL;
}

void
mISDN_nuvoton_unregister(nuvoton_inst_t *inst)
{
	printk(KERN_INFO "mISDN: dropping '%s'\n", INSTNAME);

	/* disable ringing, if ringing */
	go_onhook(inst);

	/* turn off battery */
	nxBATOn(&inst->chip_inst, inst->chip_chnl, 0);

	mISDN_unregister_device(&inst->dch.dev);
	mISDN_freebchannel(&inst->bch);
	mISDN_freedchannel(&inst->dch);
	cancel_work_sync(&inst->workq);
	module_put(THIS_MODULE);
	list_del(&inst->list);
	kfree(inst);
}

EXPORT_SYMBOL(mISDN_nuvoton_register);
EXPORT_SYMBOL(mISDN_nuvoton_unregister);

/*
 * FIFO receive function
 */

int
mISDN_nuvoton_rx_fifo(nuvoton_inst_t *inst, struct sk_buff *skb, int irq,
	uint32_t id)
{
	u_long flags;
	struct bchannel *bch;

	/* mute during pulse/flash */
	if (inst->line.mute != POTS_UNMUTE)
		memset(skb->data, (ulaw) ? 0xff : 0x2a, skb->len);
	if (inst->line.mute == POTS_UNMUTE_WAIT)
		inst->line.mute = POTS_UNMUTE;

	spin_lock_irqsave(&inst->lock, flags);

	bch = &inst->bch;

	if (!skb->len) {
		dev_kfree_skb(skb);
		spin_unlock_irqrestore(&inst->lock, flags);
		return -EINVAL;
	}

	/* drop data if bchannel is not open */
	if (bch->ch.protocol == ISDN_P_NONE) {
		dev_kfree_skb(skb);
		spin_unlock_irqrestore(&inst->lock, flags);
		return -EIO;
	}

	bch->rx_skb = skb;
	if (irq)
		recv_Bchannel(bch, id, false);
	else
		queue_ch_frame(&bch->ch, PH_DATA_IND, id, skb);

	spin_unlock_irqrestore(&inst->lock, flags);
	return 0;
}

EXPORT_SYMBOL(mISDN_nuvoton_rx_fifo);

/*
 * timer
 */

static void
nuvoton_bh(struct work_struct *work)
{
	nuvoton_inst_t *inst = container_of(work, nuvoton_inst_t, workq);
	int nls;

	nls = nxGetRTLCStatus(&inst->chip_inst, inst->chip_chnl);
	loopdetect(inst, nls & RTLC_LCD);
	send_fsk(inst);
	send_dtmf(inst);
	send_cw(inst);
}

static void
nuvoton_timeout(void *data)
{
	nuvoton_inst_t *inst;

	nuvoton_timer.expires = jiffies + TIMEOUT_10MS;
	add_timer(&nuvoton_timer);

	list_for_each_entry(inst, &nuvoton_list, list)
		schedule_work(&inst->workq);
}

/*
 * init/exit
 */

static int __init
nuvoton_init(void)
{
	printk(KERN_INFO "Nuvoton ProX analog driver\n");

	INIT_LIST_HEAD(&nuvoton_list);

	init_timer(&nuvoton_timer);
	nuvoton_timer.expires = jiffies + TIMEOUT_10MS;
	nuvoton_timer.function = (void *)nuvoton_timeout;
	add_timer(&nuvoton_timer);

	return 0;
}

static void __exit
nuvoton_exit(void)
{
	if (timer_pending(&nuvoton_timer))
		del_timer(&nuvoton_timer);
}

module_init(nuvoton_init);
module_exit(nuvoton_exit);

