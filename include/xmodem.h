#ifndef __xmodem_h
#define __xmodem_h

#include <stddef.h>
#include <stdint.h>

#define XM_SOH	0x01
#define XM_EOT	0x04
#define XM_ACK	0x06
#define XM_NAK	0x15
#define XM_CAN	0x18
#define XM_C	0x43
#define XM_BLKSIZ 128
#define XM_TOUT	10

struct xm_ops {
	/* returns # of bytes receives or -1 on error */
	int (*recv)(void *p, int maxlen);
	int (*send)(const void *p, int len);
	uint (*time_ms)(void);
};

struct xm_receiver {
	struct xm_ops ops;
	uchar buf[132];

	uchar nxtblk;
	uchar first;
	ushort error;

	uchar *bp;
	uint ts;
};


enum {
	XMR_CMD = 0,
	XMR_BLK = 1,
	XMR_BNEG = 2,
	XMR_DATA = 3,
	XMR_BLKSIZ = 128,
	XMR_CSUM = 131,
};


enum xmr_err {
	XME_OK = 0,
	XME_TOUT,	/* Timeout */
	XME_IO,		/* I/O error */
	XME_CANCEL,	/* I/O cancelled */
	XME_PROTO,	/* Protocol error */
	XME_SYNC,	/* Sync error */
};


extern const char *xm_errstr[];


void xmr_init(struct xm_receiver *xmr, struct xm_ops *ops);

/*
 * Returns:
 *  1 - reading a successful non-end block;
 *  0 - reading the last block
 * -1 - on error.  (error in xmr->error)
 */
int xmr_recv(struct xm_receiver *xmr);

#define xmr_data(_xmr) ((void *)(&(_xmr)->buf[XMR_DATA]))


struct xm_sender {
	struct xm_ops ops;
	uchar *buf;
	uchar *bp;
	uchar ctl[4];
	uchar nxtblk;
	uchar pad;
	ushort error;
	ulong len;
	uint ts;
	int last;
	int first;
};


enum {
	XMS_CMD = 0,
	XMS_BLK = 1,
	XMS_BNEG = 2,
	XMS_SUM = 3,
};


void xms_init(struct xm_sender *xms, struct xm_ops *ops);

/* len must be a multiple of 128 */
int xms_add_buf(struct xm_sender *xms, uchar *buf, ulong len, int last);

/*
 * Returns 
 *  0 - on a successful send.
 * -1 - on an error. (errro in xms->error)
 */
int xms_send(struct xm_sender *xms);

#endif /* __xmodem_h */
