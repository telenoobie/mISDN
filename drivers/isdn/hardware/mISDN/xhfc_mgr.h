/*
 * Author: Brian J Forde
 */
#ifndef __XHFC_MGR_H__
#define __XHFC_MGR_H__

extern void  xmStateChange(
        const struct xhfc *xhfc,
           const unsigned  ivPortsUp,
           unsigned priSync);

extern void  xmInitialise(void);

#endif /* __XHFC_MGR_H__ */
