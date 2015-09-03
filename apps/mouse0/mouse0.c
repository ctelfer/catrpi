#include <rpi.h>
#include <ansi.h>
#include <stdio.h>
#include <cat/crypto.h>
#include <cat/str.h>

#define NROWS		20
#define NCOLS		40
#define MAXROWS		256
#define MAXCOLS		256

#define MAXCOST		1000
#define WALLCOST	1001
#define WALLPCT		30
#define CLKSPERTICK	(1024 * 50) /* should be about 25 ms */
#define START_ENERGY	1000

enum {
	PATH = ' ',
	WALL = '#',
	MOUSE= '@',
	CHEESE = 'V',
	HAPPY_MOUSE = '$',
	STARVED_MOUSE = '%',
};

enum {
	SEARCHING = 0,
	FOUND = 1,
	STARVED = 2,
};

char maze[MAXROWS][MAXCOLS];
uint crumbs[MAXROWS][MAXCOLS];
int mrow;
int mcol;
int energy;
int status;
struct arc4ctx rctx;

void seed_random(unsigned int seed)
{
	arc4_init(&rctx, &seed, sizeof(seed));
}


unsigned int random(void)
{
	unsigned int out;
	arc4_gen(&rctx, &out, sizeof(out));
	return out;
}


void find_empty_space(int *r, int *c)
{
	do {
		*r = random() % (NROWS - 2) + 1;
		*c = random() % (NCOLS - 2) + 1;
	} while ( maze[*r][*c] != PATH );
}


void init_maze(void)
{
	int r;
	int c;
	int nwalls;

	/* clear everything */
	for ( r = 0; r < NROWS; ++r ) {
		for ( c = 0; c < NCOLS; ++c ) {
			maze[r][c] = PATH;
			crumbs[r][c] = 0;
		}
	}

	/* reset status */
	status = SEARCHING;
	energy = START_ENERGY;

	/* Top and bottom walls */
	for ( c = 0; c < NCOLS; ++c ) {
		maze[0][c] = WALL;
		crumbs[0][c] = WALLCOST;
		maze[NROWS-1][c] = WALL;
		crumbs[NROWS-1][c] = WALLCOST;
	}

	/* Left and right walls */
	for ( r = 0; r < NROWS; ++r ) {
		maze[r][0] = WALL;
		crumbs[r][0] = WALLCOST;
		maze[r][NCOLS-1] = WALL;
		crumbs[r][NCOLS-1] = WALLCOST;
	}

	/* fill random walls */
	nwalls = (NCOLS - 2) * (NROWS - 2);
	nwalls = (nwalls * WALLPCT) / 100;
	while ( nwalls-- > 0 ) {
		find_empty_space(&r, &c);
		maze[r][c] = WALL;
		crumbs[r][c] = WALLCOST;
	}

	/* place mouse */
	find_empty_space(&mrow, &mcol);
	maze[mrow][mcol] = MOUSE;

	/* place cheese */
	find_empty_space(&r, &c);
	maze[r][c] = CHEESE;
}


static void putstr(char *s, size_t len)
{
	size_t nsent;
	while ( len > 0 ) {
		nsent = rpi_uart_send(s, len);
		s += nsent;
		len -= nsent;
	}
}


void status_line(void)
{
	char str[MAXCOLS+2];
	int len;
	term_goto(NROWS, 0);
	len = snprintf(str, sizeof(str),
		       "Mouse Position: (%2d,%2d)   Mouse Energy: %4d",
		       mrow, mcol, energy);
	putstr(str, len);
}


static void draw(int r, int c)
{
	term_goto(r, c);
	putstr(&maze[r][c], 1);
}


void draw_maze(void)
{
	int r;

	term_clear_screen();
	term_cursor_off();
	for ( r = 0; r < NROWS; ++r ) {
		term_goto(r, 0);
		putstr(maze[r], NCOLS);
	}
	status_line();
}


void drop_crumb(void)
{
	if ( crumbs[mrow][mcol] < MAXCOST )
		crumbs[mrow][mcol] += 1;
}


int min(int x, int y)
{
	return (x < y) ? x : y;
}


char crumb_char(void)
{
	int nc = crumbs[mrow][mcol];
	if ( nc < 1 )
		return ' ';
	else if ( nc < 3 ) 
		return '.';
	else if ( nc < 6 ) 
		return '+';
	else if ( nc < 8 ) 
		return '*';
	else if ( nc < 10 ) 
		return 'x';
	else
		return '0';
}


void move_mouse(int rd, int cd)
{
	energy = energy - 1;
	if ( energy == 0 ) {
		status = STARVED;
		maze[mrow][mcol] = STARVED_MOUSE;
	} else {
		maze[mrow][mcol] = crumb_char();
		draw(mrow, mcol);

		mrow += rd;
		mcol += cd;
		if ( maze[mrow][mcol] != CHEESE ) {
			maze[mrow][mcol] = MOUSE;
		} else {
			status = FOUND;
			maze[mrow][mcol] = HAPPY_MOUSE;
		}
	}
	draw(mrow, mcol);
	status_line();
}


void tick(void)
{
	int min_cost;
	int rd;
	int cd;

	drop_crumb();

	min_cost = min(crumbs[mrow - 1][mcol], crumbs[mrow + 1][mcol]);
	min_cost = min(min_cost, crumbs[mrow][mcol - 1]);
	min_cost = min(min_cost, crumbs[mrow][mcol + 1]);
	if ( crumbs[mrow][mcol - 1] == min_cost ) {
		rd = 0; cd = -1;
	} else if ( crumbs[mrow - 1][mcol] == min_cost ) {
		rd = -1;  cd = 0;
	} else if ( crumbs[mrow][mcol + 1] == min_cost ) {
		rd = 0; cd = 1;
	} else {
		rd = 1; cd = 0;
	}

	move_mouse(rd, cd);
}


void prompt_play_again(void)
{
	char str[MAXCOLS+2];
	int len;
	term_goto(NROWS, 0);

	if ( status == FOUND )
		len = snprintf(str, sizeof(str),
			       "Mouse found the cheese!  :)  "
			       "Press any key to restart!");
	else
		len = snprintf(str, sizeof(str),
			       "Mouse starved!  :(  "
			       "Press any key to restart!");
	putstr(str, len);

	while ( rpi_uart_recv(str, 1) < 1 )
		;
}


void main(void)
{
	unsigned int next_tick;

	irq_init();
	rpi_uart_init();
	irq_enable();

	for ( ;; ) {
		seed_random(systime());
		init_maze();
		draw_maze();

		next_tick = systime() + CLKSPERTICK;
		while ( status == SEARCHING ) {
			/* spin until time for the next tick */
			while ( (uint)(next_tick - systime()) <= CLKSPERTICK )
				;
			tick();
			next_tick += CLKSPERTICK;
		}

		prompt_play_again();
	}
}
