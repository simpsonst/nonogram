/*
 *  Nonogram - Terminal-based Nonogram solver
 *  Copyright (C) 2001,2005-7,2012  Steven Simpson
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact Steven Simpson <https://github.com/simpsonst>
 */

#ifndef DISPLAY_HEADER
#define DISPLAY_HEADER

#include <nonogram.h>

#include "options.h"
#include "context.h"

struct h_tag {
  nonogram_puzzle puzzle;
  nonogram_cell *grid;
  nonogram_solver solver;
  context *c;
  int solno;
  unsigned stop : 1;
  FILE *fp, *logfp;
};

void print_it(void *h);

#ifndef NOVISUAL
void clear_screen(void);
void move_to(int x, int y);
void drawarea(void *h, const struct nonogram_rect *a);
void rowfocus(void *h, size_t r, int s);
void colfocus(void *h, size_t c, int s);
void rowmark(void *h, size_t f, size_t t);
void colmark(void *h, size_t f, size_t t);

extern const struct nonogram_display our_display;
extern const struct nonogram_client our_client;

#ifdef BBCGFX
void draw_grid(int w, int h);
#endif

#endif /* NOVISUAL */

#endif /* __DISPLAY_H__ */
