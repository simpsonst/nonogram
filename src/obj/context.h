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
 *  Contact Steven Simpson <ss@comp.lancs.ac.uk>
 */

#ifndef CONTEXT_HEADER
#define CONTEXT_HEADER

#if __STDC__ != 1
#error "You need an ISO C compiler."
#endif

#include <stdio.h>

#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif

enum { MULTIPLE, SINGLE, STDSTREAM, NONE };

typedef struct {
  const char *html_dot;
  const char *html_solid;
  const char *html_blank;
  const char *dot;
  const char *solid;
  const char *blank;
  const char *ifile;
  const char *ofile;
  const char *logfile;
  const char *title;
  int ofiletype;
  int limit;
  unsigned loglevel;
  unsigned html : 1, table : 1, error : 1, executed : 1, visual : 1,
    countonerr : 1, countonout : 1, algo : 4, verbose : 1, logappend : 1;
} context;

int parse_args(context *c, int argc, const char *argv[], const char *pname);
int print_context(context *p, FILE *fp);
void init_context(context *p);

#endif
