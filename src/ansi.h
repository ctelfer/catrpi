/*
 * include/ansi.h -- API header for ANSI terminal manipulation
 *
 * by Christopher Adam Telfer
 *
 * Copyright 2015 See accompanying license
 *
 */

#ifndef __ANSI_H
#define __ANSI_H

#include <stddef.h>
#include <stdint.h>

#define ESC 0x1B

enum {
	K_NONE,
	K_UNKNOWN,
	K_UP,
	K_DOWN,
	K_RIGHT,
	K_LEFT,
	K_GOTO,
	K_CLRSCR,
};


struct keyval {
	uchar special;
	char value;
	uchar v1;
	uchar v2;
};


struct keyval key_get(void);

void key_put(struct keyval kv);

void term_goto(uint r, uint c);

void term_up(void);

void term_down(void);

void term_right(void);

void term_left(void);

void term_up_by(uint amt);

void term_down_by(uint amt);

void term_left_by(uint amt);

void term_right_by(uint amt);

void term_clear_screen(void);

void term_cursor_off(void);

void term_cursor_on(void);

#endif /* __ANSI_H */
