/*
 * src/ansi.c -- API for ANSI terminal manipulation
 *
 * by Christopher Adam Telfer
 *
 * Copyright 2015 See accompanying license
 *
 */

#include <ansi.h>
#include <rpi.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>


static int getc(int block)
{
	uchar c;
	if ( rpi_uart_recv(&c, 1) >= 1 ) {
		return c;
	} else if ( !block ) {
		return -1;
	} else {
		while ( rpi_uart_recv(&c, 1) < 1 );
		return c;
	}
}


static uint getnum(char digit, uchar *val)
{
	*val = digit - '0';
	while ( isdigit(digit = getc(1)) )
		*val = (*val * 10) + digit - '0';
	return digit;
}


struct keyval key_get(void)
{
	int c;
	struct keyval kv;

	kv.special = 1;
	kv.value = K_NONE;
	kv.v1 = 0;
	kv.v2 = 0;

	c = getc(0);
	if ( c < 0 )
		return kv;

	if ( c != ESC ) {
		kv.special = 0;
		kv.value = c;
		return kv;
	}

	kv.value = K_UNKNOWN;
	c = getc(1);
	if ( c != '[' )
		return kv;

	if ( isdigit(c) ) {
		c = getnum(c, &kv.v1);
		if ( c == ';' ) {
			c = getc(1);
			if ( !isdigit(c) )
				return kv;
			c = getnum(c, &kv.v2);
		}
	}

	switch (c) {
	case 'A': kv.value = K_UP; break;
	case 'B': kv.value = K_DOWN; break;
	case 'C': kv.value = K_RIGHT; break;
	case 'D': kv.value = K_LEFT; break;
	case 'J':
		  if ( kv.v1 != 2 )
			  return kv;
		  kv.value = K_CLRSCR;
		  break;
	}

	return kv;
}


static void putstr(const char *s, size_t len)
{
	size_t nsent;
	while ( len > 0 ) {
		nsent = rpi_uart_send(s, len);
		s += nsent;
		len -= nsent;
	}
}


void key_put(struct keyval kv)
{
	char s[16];
	size_t len;
	char code;

	if ( !kv.special ) {
		putstr(&kv.value, 1);
	} else {
		switch (kv.value) {
		case K_UP:
		case K_DOWN:
		case K_LEFT:
		case K_RIGHT:
			code = 'A' + (kv.value - K_UP);
			if ( kv.v1 <= 0 ) {
				len = snprintf(s, sizeof(s), "\x1b[%c", code);
			} else {
				len = snprintf(s, sizeof(s), "\x1b[%u%c",
					       kv.v1, code);
			}
			putstr(s, len);
			break;
		case K_GOTO:
			len = snprintf(s, sizeof(s), "\x1b[%u;%uH", kv.v1,
				       kv.v2);
			putstr(s, len);
			break;
		case K_CLRSCR:
			putstr("\x1b[2J", 4);
			break;
		}
	}
}


void term_goto(uint r, uint c)
{
	char s[32];
	int len;
	len = snprintf(s, sizeof(s), "\x1b[%u;%uH", r + 1, c + 1);
	putstr(s, len);
}


static void term_motion(int dir, uint amt)
{
	char s[32];
	char code = 'A' + (dir - K_UP);
	int len;
	if ( amt <= 0 ) {
		len = snprintf(s, sizeof(s), "\x1b[%c", code);
	} else {
		len = snprintf(s, sizeof(s), "\x1b[%u%c", amt, code);
	}
	putstr(s, len);
}


void term_up(void)           { putstr("\x1b[A", 3);       }
void term_down(void)         { putstr("\x1b[B", 3);       }
void term_right(void)        { putstr("\x1b[C", 3);       }
void term_left(void)         { putstr("\x1b[D", 3);       }
void term_up_by(uint amt)    { term_motion(K_UP, amt);    }
void term_down_by(uint amt)  { term_motion(K_DOWN, amt);  }
void term_left_by(uint amt)  { term_motion(K_LEFT, amt);  }
void term_right_by(uint amt) { term_motion(K_RIGHT, amt); }


void term_clear_screen(void)
{
	putstr("\x1b[2J", 4);
}


void term_cursor_off(void)
{
	putstr("\x1b[?25l", 6);
}


void term_cursor_on(void)
{
	putstr("\x1b[?25h", 6);
}
