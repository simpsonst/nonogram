// -*- c-basic-offset: 2; indent-tabs-mode: nil -*-

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

#include "display.h"

#ifndef NOVISUAL
#if defined(BBCGFX)
#include "bbcgfxcf.h"
#define XSGN ((BBCGFX_XSCALE > 0) ? 1 : -1)
#define YSGN ((BBCGFX_YSCALE > 0) ? 1 : -1)
#endif

#if defined(BBCGFX) || defined(BBCVDU)
#include <kernel.h>
#endif

const struct nonogram_display our_display = {
  &drawarea,
  &rowfocus,
  &colfocus,
  &rowmark,
  &colmark
};

const struct nonogram_client our_client = {
  &print_it
};

int printgrid(const nonogram_cell *grid, size_t width, size_t height,
              FILE *fp, const char *solid, const char *dot,
              const char *blank);
int printhtmlgrid(const nonogram_cell *grid, size_t width, size_t height,
                  FILE *fp, const char *solid, const char *dot,
                  const char *blank, int table);

void clear_screen(void)
{
#if defined(BBCGFX)
  _kernel_oswrch(18);
  _kernel_oswrch(0);
  _kernel_oswrch(128 + (BBCGFX_COL_BG));
  _kernel_oswrch(16);
#elif defined(BBCVDU)
  _kernel_oswrch(12);
#else
  fputc(0x1b, stderr);
  fputc('[', stderr);
  fputc('2', stderr);
  fputc('J', stderr);
#endif
}

#if defined(BBCGFX)
static void plot(int c, int x, int y)
{
  _kernel_oswrch(25);
  _kernel_oswrch(c);
  _kernel_oswrch(x & 0xff);
  _kernel_oswrch((x & 0xff00) >> 8);
  _kernel_oswrch(y & 0xff);
  _kernel_oswrch((y & 0xff00) >> 8);
}
#endif

void move_to(int x, int y)
{
#if defined(BBCGFX)
  plot(4, x * (BBCGFX_XSCALE) + (BBCGFX_XOFFSET),
          y * (BBCGFX_YSCALE) + (BBCGFX_YOFFSET));
#elif defined(BBCVDU)
  _kernel_oswrch(31);
  _kernel_oswrch(x);
  _kernel_oswrch(y);
#else
  char tmp[100];

  fputc(0x1b, stderr);
  fputc(0x5b, stderr);

  sprintf(tmp, "%d;%dH", y + 1, x + 1);
  fprintf(stderr, "%s", tmp);
#endif
}

#if defined(BBCGFX)
void draw_block(void)
{
  plot(97, (BBCGFX_XSCALE) - ((BBCGFX_XMARGIN) + 1) * XSGN,
           (BBCGFX_YSCALE) - ((BBCGFX_YMARGIN) + 1) * YSGN);
}
#endif

#if defined(BBCGFX)
void set_gcol(int c)
{
  _kernel_oswrch(18);
  _kernel_oswrch(0);
  _kernel_oswrch(c);
}
#endif

void drawarea(void *vh, const struct nonogram_rect *a)
{
  struct h_tag *h = vh;
  size_t x, y;

  for (x = a->min.x; x < a->max.x; x++)
    for (y = a->min.y; y < a->max.y; y++) {
      move_to(x, y);
      switch (h->grid[x + y * h->puzzle.width]) {
#ifdef BBCGFX
      case nonogram_BLANK: set_gcol(BBCGFX_COL_BLANK); break;
      case nonogram_DOT: set_gcol(BBCGFX_COL_DOT); break;
      case nonogram_SOLID: set_gcol(BBCGFX_COL_SOLID); break;
      default: set_gcol(BBCGFX_COL_UNKNOWN); break;
#else
      case nonogram_BLANK: fputc(' ', stderr); break;
      case nonogram_DOT: fputc('-', stderr); break;
      case nonogram_SOLID: fputc('#', stderr); break;
      default: fputc('?', stderr); break;
#endif
      }
#ifdef BBCGFX
      draw_block();
#endif
    }
  fflush(stderr);
}

void rowfocus(void *vh, size_t r, int s)
{
  struct h_tag *h = vh;
  move_to(h->puzzle.width + 1, r);
#ifdef BBCGFX
  set_gcol(s ? (BBCGFX_COL_ACTIVE) : (BBCGFX_COL_BG));
  draw_block();
#else
  fputc(s ? '!' : ' ', stderr);
  move_to(1, h->puzzle.height + 3);
#endif
  fflush(stderr);
}

void colfocus(void *vh, size_t c, int s)
{
  struct h_tag *h = vh;
  move_to(c, h->puzzle.height + 1);
#ifdef BBCGFX
  set_gcol(s ? (BBCGFX_COL_ACTIVE) : (BBCGFX_COL_BG));
  draw_block();
#else
  fputc(s ? '!' : ' ', stderr);
  move_to(1, h->puzzle.height + 3);
#endif
  fflush(stderr);
}

void rowmark1(void *vh, size_t r, int s)
{
  struct h_tag *h = vh;
  move_to(h->puzzle.width + 2, r);
#ifdef BBCGFX
  set_gcol(s ? (s == 2 ? (BBCGFX_COL_NOTDONE) :
                (BBCGFX_COL_HALFDONE)) : (BBCGFX_COL_BG));
  draw_block();
#else
  fputc(s + '0', stderr);
#endif
  fflush(stderr);
}

void colmark1(void *vh, size_t c, int s)
{
  struct h_tag *h = vh;
  move_to(c, h->puzzle.height + 2);
#ifdef BBCGFX
  set_gcol(s ? (s == 2 ? (BBCGFX_COL_NOTDONE) :
                (BBCGFX_COL_HALFDONE)) : (BBCGFX_COL_BG));
  draw_block();
#else
  fputc(s + '0', stderr);
#endif
  fflush(stderr);
}

void rowmark(void *vh, size_t f, size_t t)
{
  struct h_tag *h = vh;
  while (f < t) {
    rowmark1(h, f, nonogram_getrowmark(&h->solver, f));
    f++;
  }
}

void colmark(void *vh, size_t f, size_t t)
{
  struct h_tag *h = vh;
  while (f < t) {
    colmark1(h, f, nonogram_getcolmark(&h->solver, f));
    f++;
  }
}

#ifdef BBCGFX
void draw_grid(int w, int h)
{
  int line;
  _kernel_oswrch(18);
  _kernel_oswrch(0);
  _kernel_oswrch(5);
  for (line = 0; line <= h; line += 5) {
    plot(4,
         (BBCGFX_XOFFSET), (BBCGFX_YOFFSET) + line * (BBCGFX_YSCALE) - YSGN);
    plot(97, (BBCGFX_XSCALE) * w - XSGN, ((1 - BBCGFX_YMARGIN)) * YSGN);
  }
  for (line = 0; line <= w; line += 5) {
    plot(4,
         (BBCGFX_XOFFSET) + line * (BBCGFX_XSCALE) - XSGN, (BBCGFX_YOFFSET));
    plot(97, ((1 - BBCGFX_XMARGIN)) * XSGN, (BBCGFX_YSCALE) * h - YSGN);
  }
}
#endif

#endif /* NOVISUAL */

void print_it(void *vh)
{
  struct h_tag *h = vh;
  FILE *fp;
  char fname[400];

  h->solno++;
  if (h->c->limit > 0 && h->solno >= h->c->limit)
    h->stop = true;

  switch (h->c->ofiletype) {
  case STDSTREAM:
    fp = stdout;
    break;
  case NONE:
    return;
  case MULTIPLE:
    sprintf(fname, h->c->ofile, h->solno);
    fp = fopen(fname, "w");
    if (!fp) {
      if (h->c->visual)
        move_to(0, h->puzzle.height + 6);
      fprintf(stderr, "%s: can't write file\n", fname);
      return;
    }
    break;
  case SINGLE:
    fp = h->fp;
    break;
  default:
    fp = NULL;
    break;
  }

  if (h->c->html) {
    printhtmlgrid(h->grid, h->puzzle.width, h->puzzle.height, fp,
                  h->c->html_solid, h->c->html_dot, h->c->html_blank,
                  h->c->table);
    fprintf(fp, "<TR><TD COLSPAN=%zu><HR></TD></TR>\n",
            h->c->table ? h->puzzle.width : 1);
  } else {
    printgrid(h->grid, h->puzzle.width, h->puzzle.height, fp,
              h->c->solid, h->c->dot, h->c->blank);
    if (h->c->ofiletype != MULTIPLE)
      fprintf(fp, "\n");
  }

  switch (h->c->ofiletype) {
  case MULTIPLE:
    fclose(fp);
    break;
  }
}

int printgrid(const nonogram_cell *grid, size_t width, size_t height,
              FILE *fp, const char *solid, const char *dot,
              const char *blank)
{
  size_t row, col, count = 0;

  for (row = 0; row < height; row++) {
    for (col = 0; col < width; col++) {
      int c = grid[col + row * width];

      count += fprintf(fp, "%s", c == nonogram_SOLID ? solid :
                       c == nonogram_DOT ? dot : blank);
    }
    putc('\n', fp);
    count++;
  }
  return count;
}

int printhtmlgrid(const nonogram_cell *grid, size_t width, size_t height,
                  FILE *fp, const char *solid, const char *dot,
                  const char *blank, int table)
{
  int count = 0;
  size_t row;

  if (!table)
    count += fprintf(fp, "\n<TR><TD>");
  for (row = 0; row < height; row++) {
    size_t col;

    if (table)
      count += fprintf(fp, "<TR>");
    for (col = 0; col < width; col++) {
      nonogram_cell cell = grid[width * row + col];

      if (table)
        count += fprintf(fp, "<TD>");
      count += fprintf(fp, "<IMG SRC=\"%s\"",
                       cell == nonogram_DOT ? dot :
                       (cell == nonogram_SOLID ? solid : blank));
      count += fprintf(fp, " BORDER=0 HSPACE=0 VSPACE=0");
      count += fprintf(fp, " ALT=\"%s\">",
                       cell == nonogram_DOT ? "-" :
                       (cell == nonogram_SOLID ? "#" : "&nbsp;"));
      if (table)
        count += fprintf(fp, "%s</TD>", col == width - 1 ? "<BR>" : "");
    }
    if (table)
      count += fprintf(fp, "</TR>\n");
    else
      count += fprintf(fp, "%s\n", row == height - 1 ? "</TD></TR>" : "<BR>");
  }

  return count;
}
