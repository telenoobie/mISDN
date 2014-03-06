#ifndef NUVOTON_H_
#define NUVOTON_H_

#include <linux/mISDNhw.h>
#include "prox/nxapi.h"

#define DEBUG_NUVOTON_LINE	0x00010000
#define DEBUG_NUVOTON_RINGING	0x00020000
#define DEBUG_NUVOTON_CW	0x00040000
#define DEBUG_NUVOTON_FSK	0x00080000
#define DEBUG_NUVOTON_DTMF	0x00100000

typedef struct nuvoton_hw_info {
	void			*priv;
	struct module		*module;
	uint8_t			has_fxs, has_fxo;	/* interface type */
	int			pcmopts;		/* PCM IF options */
	uint16_t		rx_valbits, tx_valbits;	/* PCM slot config */
} nuvoton_hw_info_t;

enum pots_state {
	POTS_STATE_ONHOOK,
	POTS_STATE_OFFHOOK,
	POTS_STATE_RINGING,
};

enum pots_mute {
	POTS_UNMUTE,
	POTS_MUTE,
	POTS_UNMUTE_WAIT,
};

typedef struct nuvoton_inst {
	struct list_head	list;
	int			count;
	char			name[MISDN_MAX_IDLEN];
	ProXChip		chip_inst;		/* chip instance */
	__chip_chnl		chip_chnl;		/* channel number */
	struct dchannel		dch;
	struct bchannel		bch;
	nuvoton_hw_info_t	hw_info;
	spinlock_t		lock;
	struct work_struct	workq;
	enum pots_state		state;
	struct {	/* hook/flash/keypulse detection states */
		int		status;
		int		wait_onhook; /* wait for on-hook */
		uint32_t	hook_jiffies; /* timeout for pulse/flash/hook */
		int		pulse; /* detected number of pulses */
		uint32_t	pulse_jiffies; /* timeout for complete digit */
		enum pots_mute	mute; /* mute state, to mute during pulses */
	} line;
	struct {	/* FSK states */
		int		on_time; /* time to send after ringer start */
		int		dtas; /* currently sending DT-AS */
		int		send; /* enabled if FSK is scheduled */
		uint32_t	jiffies; /* time when FSK was scheduled */
		int		started; /* ongoing FSK transmission */
		int		seize_mark; /* set, when sending seizure/mark */
		int		flush; /* wait for flush */
		int 		pos; /* current byte position */
		uint8_t		buff[2+255+1]; /* fsk data buffer */
		int 		len; /* length of buffer */
	} fsk;
	struct {	/* DTMF states */
		int		on_time; /* time to send after ringer start */
		int		send; /* enabled if DTMF is scheduled */
		uint32_t	jiffies; /* time when DTMF was scheduled */
		int 		pos; /* current byte position */
		uint8_t		buff[1+255+1+1]; /* DTMF data buffer */
		int 		len; /* length of buffer */
		int		init; /* state: wait for line state */
		int		done; /* state: wait for next ring */
	} dtmf;
	struct {	/* call waiting states */
		int		enabled; /* cw tone is enabled */
		int		on; /* tone is currently generated */
		int		count; /* counter of current tone cycle */
	} cw;
	/* callback functions for hardware driver */
	int			(*tx_fifo)(struct nuvoton_inst *inst,
						struct sk_buff *skb);
	int			(*write_spi)(struct nuvoton_inst *inst, int reg,
						int value);
	int			(*read_spi)(struct nuvoton_inst *inst, int reg);
	int			(*ph_activate)(struct nuvoton_inst *inst);
	int			(*ph_deactivate)(struct nuvoton_inst *inst);

} nuvoton_inst_t;

nuvoton_inst_t *mISDN_nuvoton_register(nuvoton_hw_info_t *info,
	int (*tx_fifo)(nuvoton_inst_t *inst, struct sk_buff *skb),
	int (*write_spi)(nuvoton_inst_t *inst, int reg, int value),
	int (*read_spi)(nuvoton_inst_t *inst, int reg),
	int (*ph_activate)(nuvoton_inst_t *inst),
	int (*ph_deactivate)(nuvoton_inst_t *inst));
void mISDN_nuvoton_unregister(nuvoton_inst_t *inst);
int mISDN_nuvoton_rx_fifo(nuvoton_inst_t *inst, struct sk_buff *skb, int irq,
	uint32_t id);
irqreturn_t mISDN_nuvoton_isr(int irq, void *dev_id);

#endif /* NUVOTON_H_ */
