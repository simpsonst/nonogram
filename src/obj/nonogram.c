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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __TARGET_UNIXLIB__
#define __UNIXLIB_INTERNALS 1
#include <unixlib/local.h>
int __riscosify_control = __RISCOSIFY_NO_PROCESS;
#endif

#include <nonogram.h>

#include "options.h"
#include "context.h"
#include "display.h"

int print_usage(FILE *fp, const char *progname);

int main(int argc, const char *argv[])
{
  context ctx;
  int r;

  init_context(&ctx);
  if ((r = parse_args(&ctx, argc - 1, argv + 1, argv[0])) != 0) {
    /* print_usage(stderr, argv[0]); */
    exit(r);
  }

  return 0;
}
