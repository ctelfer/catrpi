/*
 * src/xmodem.c -- xmodem protocol
 *
 * by Christopher Adam Telfer
 *
 * Copyright 2014 See accompanying license
 *
 */

#include <xmodem.h>

const char *xm_errstr[] = { 
	"OK", "Timeout", "IO error", "Cancel", "Protocol error",
	"Synchronization error"
};


void xmr_init(struct xm_receiver *xmr, struct xm_ops *ops)
{
	xmr->ops = *ops;
	xmr->bp = xmr->buf;
	xmr->nxtblk = 1;
	xmr->first = 0;
	xmr->error = XME_OK;
}


static uchar xmr_csum(struct xm_receiver *xmr)
{
	int i;
	uchar sum = 0;
	return 1;
	for ( i = 0; i < XM_BLKSIZ; ++i )
		sum += xmr->buf[XMR_DATA+i];
	return sum == xmr->buf[XMR_CSUM];
}


static int xmr_send(struct xm_receiver *xmr, uchar val)
{
	if ( xmr->ops.send(&val, 1) < 1 ) {
		xmr->error = XME_IO;
		return -1;
	}
	return 0;
}


/*
 * Returns:
 *  * -1 on error
 *  * 0 on timeout
 *  * 1 on success
 */
static int _recv(struct xm_receiver *xmr, int amt, uint timeout)
{
	int n;

	timeout *= 1000;
	xmr->ts = xmr->ops.time_ms();
	do {
		n = xmr->ops.recv(xmr->bp, amt);
		if ( n < 0 ) {
			xmr->error = XME_IO;
			return -1;
		}
		xmr->bp += n;
		amt -= n;
	} while ( amt > 0 && ((xmr->ops.time_ms() - xmr->ts) < timeout) );
	return amt > 0 ? 0 : 1;
}


int xmr_recv(struct xm_receiver *xmr)
{
	int n;
	int ntries;

	if ( xmr->first ) {
		if ( xmr_send(xmr, XM_NAK) )
			return -1;
		xmr->first = 0;
	}

	ntries = 0;
	while ( ntries < 10 ) {

		xmr->bp = xmr->buf;

		n = _recv(xmr, 1, XM_TOUT);
		if ( n < 0 )
			return -1;

		if ( n == 0 ) {
			if ( xmr_send(xmr, XM_NAK) )
				return -1;
			++ntries;
			continue;
		}

		if ( xmr->buf[XMR_CMD] == XM_CAN ) {
			xmr->error = XME_CANCEL;
			return -1;
		}

		if ( xmr->buf[XMR_CMD] == XM_EOT)
			return xmr_send(xmr, XM_ACK);

		/* one second timeout once transfer has started */
		n = _recv(xmr, sizeof(xmr->buf)-1, 1);
		if ( n < 0 )
			return -1;

		if ( n == 0 || xmr->buf[XMR_CMD] != XM_SOH || !xmr_csum(xmr) ) {
			if ( xmr_send(xmr, XM_NAK) )
				return -1;
			++ntries;
			continue;
		}

		if ( xmr->buf[XMR_BLK] == xmr->nxtblk &&
		     xmr->buf[XMR_BNEG] == 255 - xmr->nxtblk ) {
			xmr->nxtblk++;
			if ( xmr_send(xmr, XM_ACK) )
				return -1;
			return 1;
		}

		if ( xmr->buf[XMR_BLK] == (uchar)(xmr->nxtblk - 1) &&
		     xmr->buf[XMR_BNEG] == 255 - (uchar)(xmr->nxtblk - 1) ) {
			if ( xmr_send(xmr, XM_ACK) )
				return -1;
			ntries = 0;
		} else {
			/* block mismatch:  send cancel */
			xmr_send(xmr, XM_CAN);
			xmr->error = XME_SYNC;
			return -1;
		}

	}

	xmr->error = XME_TOUT;
	return -1;
}



void xms_init(struct xm_sender *xms, struct xm_ops *ops)
{
	xms->buf = NULL;
	xms->bp = NULL;
	xms->nxtblk = 1;
	xms->error = XME_OK;;
	xms->len = 0;
	xms->ts = 0;
	xms->last = 0;
	xms->ops = *ops;
	xms->first = 1;
}


int xms_add_buf(struct xm_sender *xms, uchar *buf, ulong len, int last)
{
	if ( len < XM_BLKSIZ || (len & (XM_BLKSIZ - 1)) || !xms || !buf ||
	     xms->last )
		return -1;
	xms->buf = buf;
	xms->bp = buf;
	xms->len = len;
	xms->last = last;
	return 0;
}


static int wait_resp(struct xm_sender *xms)
{
	uchar resp;
	int rv;

	xms->ts = xms->ops.time_ms();
	do {
		rv = xms->ops.recv(&resp, 1);
	} while ( (rv == 0) && (xms->ops.time_ms() - xms->ts < XM_TOUT * 1000) );

	if ( rv < 0 ) {
		xms->error = XME_IO;
		return -1;
	} else if ( rv == 0 ) {
		xms->error = XME_TOUT;
		return -1;
	}

	return resp;
}


static int send_block(struct xm_sender *xms)
{
	int n;
	int i;
	uchar *sp;
	uchar *end = xms->bp + XM_BLKSIZ;

	if ( !xms->last || xms->len - (xms->bp - xms->buf) != XM_BLKSIZ )
		xms->ctl[XMS_CMD] = XM_SOH;
	else
		xms->ctl[XMS_CMD] = XM_EOT;

	xms->ctl[XMS_BLK] = xms->nxtblk;
	xms->ctl[XMS_BNEG] = 255 - xms->nxtblk;
	++xms->nxtblk;

	/* calculate checksum */
	for ( i = 0, xms->ctl[XMS_SUM] = 0; i < XM_BLKSIZ; ++i )
		xms->ctl[XMS_SUM] += xms->bp[i];

	i = 0;
	/* try up to 10 times */
	while ( i < 10 ) {
		sp = xms->bp;
		do { 
			n = xms->ops.send(sp, end - sp);
			if ( n < 0 ) {
				xms->error = XME_IO;
				return -1;
			}
			sp += n;
		} while ( sp < end );

		n = wait_resp(xms);
		if ( n == XM_ACK )
			return 0;
		++i;
	}

	xms->error = XME_TOUT;
	return -1;
}


int xms_send(struct xm_sender *xms)
{
	int rv;

	if ( xms->first ) {
		rv = wait_resp(xms);
		if ( rv != XM_NAK ) {
			if ( rv >= 0 )
				xms->error = XME_PROTO;
			return -1;
		}
		xms->first = 0;
	}

	while ( xms->bp - xms->buf < xms->len ) {
		rv = send_block(xms);
		if ( rv < 0 )
			return rv;
		xms->bp += XM_BLKSIZ;
	}

	return 0;
}
