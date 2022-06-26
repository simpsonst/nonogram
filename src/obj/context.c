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

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <nonogram.h>
#include <nonocache.h>
#include <nonogram_version.h>

#include "context.h"
#include "options.h"
#include "display.h"

const char *PTRCONST default_html_dot = "icons/dot.xbm";
const char *PTRCONST default_html_solid = "icons/solid.xbm";
const char *PTRCONST default_html_blank = "icons/blank.xbm";
const char *PTRCONST default_dot = "-";
const char *PTRCONST default_solid = "#";
const char *PTRCONST default_blank = " ";

nonogram_cell *loadgrid(size_t *width, size_t *height,
                        FILE *fp, char solid, char dot);

int create(context *c, const char *progname)
{
  FILE *fin, *fout;
  nonogram_puzzle puzzle;
  nonogram_cell *g;
  size_t w, h;

  if (c->ifile) {
    fin = fopen(c->ifile, "r");
    if (!fin) {
      fprintf(stderr, "%s: could not read\n", c->ifile);
      return -1;
    }
  } else {
    fin = stdin;
  }

  if (!(g = loadgrid(&w, &h, fin, *c->solid, *c->dot))) {
    if (c->ifile)
      fclose(fin);
    fprintf(stderr, "%s: error on input\n", progname);
    return -1;
  }
  if (fin && fin != stdin)
    fclose(fin);

  if (nonogram_makepuzzle(&puzzle, g, w, h) < 0) {
    fprintf(stderr, "%s: incomplete grid\n", progname);
    nonogram_freegrid(g);
    return -1;
  }
  nonogram_freegrid(g);

  if (c->title)
    nonogram_settitle(&puzzle, c->title);

  switch (c->ofiletype) {
  case SINGLE:
    fout = fopen(c->ofile, "w");
    if (!fout) {
      fprintf(stderr, "%s: can't write file\n", c->ofile);
      nonogram_freepuzzle(&puzzle);
      return -1;
    }
    break;
  case STDSTREAM:
    fout = stdout;
    break;
  case MULTIPLE:
    fprintf(stderr, "%s: inappropriate output (use -o, -os or +o)\n",
            progname);
    nonogram_freepuzzle(&puzzle);
    return -1;
  default:
    nonogram_freepuzzle(&puzzle);
    return 0;
  }

  nonogram_fprintpuzzle(&puzzle, fout);
  if (fout && fout != stdout)
    fclose(fout);

  nonogram_freepuzzle(&puzzle);
  return 0;
}

int verify(context *c, const char *progname)
{
  FILE *fin;
  nonogram_puzzle puzzle;
  int diff;

  if (c->ifile) {
    fin = fopen(c->ifile, "r");
    if (!fin) {
      fprintf(stderr, "%s: could not read\n", c->ifile);
      return -1;
    }
  } else {
    fin = stdin;
  }

  if (nonogram_fscanpuzzle(&puzzle, fin) < 0) {
    if (c->ifile)
      fclose(fin);
    fprintf(stderr, "%s: error on input\n", progname);
    return -1;
  }
  if (fin && fin != stdin)
    fclose(fin);

  diff = nonogram_verifypuzzle(&puzzle);
  if (c->verbose && diff)
    fprintf(stderr, "%s-heaviness: %d\n",
            diff > 0 ? "Row" : "Column", diff > 0 ? diff : -diff);
  printf("%d\n", diff);

  nonogram_freepuzzle(&puzzle);
  return 0;
}

enum { TITLE, WIDTH, HEIGHT };

int extractpart(context *c, const char *progname, int part)
{
  FILE *fin, *fout;
  nonogram_puzzle puzzle;
  const char *t;

  if (c->ifile) {
    fin = fopen(c->ifile, "r");
    if (!fin) {
      fprintf(stderr, "%s: could not read\n", c->ifile);
      return -1;
    }
  } else {
    fin = stdin;
  }

  if (nonogram_fscanpuzzle(&puzzle, fin) < 0) {
    if (c->ifile)
      fclose(fin);
    fprintf(stderr, "%s: error on input\n", progname);
    return -1;
  }
  if (fin && fin != stdin)
    fclose(fin);

  switch (c->ofiletype) {
  case SINGLE:
    fout = fopen(c->ofile, "w");
    if (!fout) {
      fprintf(stderr, "%s: can't write file\n", c->ofile);
      nonogram_freepuzzle(&puzzle);
      return -1;
    }
    break;
  case STDSTREAM:
    fout = stdout;
    break;
  case MULTIPLE:
    fprintf(stderr, "%s: inappropriate output (use -o, -os or +o)\n",
            progname);
    nonogram_freepuzzle(&puzzle);
    return -1;
  default:
    nonogram_freepuzzle(&puzzle);
    return 0;
  }

  switch (part) {
  case TITLE:
    t = nonogram_puzzletitle(&puzzle);
    if (t) fprintf(fout, "%s", t);
    break;
  case WIDTH:
    fprintf(fout, "%zu", puzzle.width);
    break;
  case HEIGHT:
    fprintf(fout, "%zu", puzzle.height);
    break;
  }

  if (fout && fout != stdout)
    fclose(fout);
  nonogram_freepuzzle(&puzzle);
  return 0;
}

int extractnote(context *c, const char *progname, const char *name)
{
  FILE *fin, *fout;
  nonogram_puzzle puzzle;
  const char *t;

  if (c->ifile) {
    fin = fopen(c->ifile, "r");
    if (!fin) {
      fprintf(stderr, "%s: could not read\n", c->ifile);
      return -1;
    }
  } else {
    fin = stdin;
  }

  if (nonogram_fscanpuzzle(&puzzle, fin) < 0) {
    if (c->ifile)
      fclose(fin);
    fprintf(stderr, "%s: error on input\n", progname);
    return -1;
  }
  if (fin && fin != stdin)
    fclose(fin);

  t = nonogram_getnote(&puzzle, name);
  if (t) {
    switch (c->ofiletype) {
    case SINGLE:
      fout = fopen(c->ofile, "w");
      if (!fout) {
        fprintf(stderr, "%s: can't write file\n", c->ofile);
        nonogram_freepuzzle(&puzzle);
        return -1;
      }
      break;
    case STDSTREAM:
      fout = stdout;
      break;
    case MULTIPLE:
      fprintf(stderr, "%s: inappropriate output (use -o, -os or +o)\n",
              progname);
      nonogram_freepuzzle(&puzzle);
      return -1;
    default:
      nonogram_freepuzzle(&puzzle);
      return 0;
    }

    fprintf(fout, "%s", t);

    if (fout && fout != stdout)
      fclose(fout);
  }
  nonogram_freepuzzle(&puzzle);
  return t ? 0 : -1;
}

int deletenote(context *c, const char *progname, const char *name)
{
  FILE *fin, *fout;
  nonogram_puzzle puzzle;
  int t;

  if (c->ifile) {
    fin = fopen(c->ifile, "r");
    if (!fin) {
      fprintf(stderr, "%s: could not read\n", c->ifile);
      return -1;
    }
  } else {
    fin = stdin;
  }

  if (nonogram_fscanpuzzle(&puzzle, fin) < 0) {
    if (c->ifile)
      fclose(fin);
    fprintf(stderr, "%s: error on input\n", progname);
    return -1;
  }
  if (fin && fin != stdin)
    fclose(fin);

  t = nonogram_unsetnote(&puzzle, name);
  if (t == 0) {
    switch (c->ofiletype) {
    case SINGLE:
      fout = fopen(c->ofile, "w");
      if (!fout) {
        fprintf(stderr, "%s: can't write file\n", c->ofile);
        nonogram_freepuzzle(&puzzle);
        return -1;
      }
      break;
    case STDSTREAM:
      fout = stdout;
      break;
    case MULTIPLE:
      fprintf(stderr, "%s: inappropriate output (use -o, -os or +o)\n",
              progname);
      nonogram_freepuzzle(&puzzle);
      return -1;
    default:
      nonogram_freepuzzle(&puzzle);
      return 0;
    }

    nonogram_fprintpuzzle(&puzzle, fout);

    if (fout && fout != stdout)
      fclose(fout);
  }
  nonogram_freepuzzle(&puzzle);
  return t ? 0 : -1;
}

int replacenote(context *c, const char *progname,
                const char *name, const char *value, int safe)
{
  FILE *fin, *fout;
  nonogram_puzzle puzzle;
  int t;

  if (c->ifile) {
    fin = fopen(c->ifile, "r");
    if (!fin) {
      fprintf(stderr, "%s: could not read\n", c->ifile);
      return -1;
    }
  } else {
    fin = stdin;
  }

  if (nonogram_fscanpuzzle(&puzzle, fin) < 0) {
    if (c->ifile)
      fclose(fin);
    fprintf(stderr, "%s: error on input\n", progname);
    return -1;
  }
  if (fin && fin != stdin)
    fclose(fin);

  if (safe && nonogram_getnote(&puzzle, name)) {
    t = c->verbose ? -1 : 0;
    if (t < 0)
      fprintf(stderr, "%s: `%s' already set\n",
              c->ifile ? c->ifile : "(stdin)" , name);
  } else {
    t = nonogram_setnote(&puzzle, name, value);
  }

  if (t == 0) {
    switch (c->ofiletype) {
    case SINGLE:
      fout = fopen(c->ofile, "w");
      if (!fout) {
        fprintf(stderr, "%s: can't write file\n", c->ofile);
        nonogram_freepuzzle(&puzzle);
        return -1;
      }
      break;
    case STDSTREAM:
      fout = stdout;
      break;
    case MULTIPLE:
      fprintf(stderr, "%s: inappropriate output (use -o, -os or +o)\n",
              progname);
      nonogram_freepuzzle(&puzzle);
      return -1;
    default:
      nonogram_freepuzzle(&puzzle);
      return 0;
    }

    nonogram_fprintpuzzle(&puzzle, fout);

    if (fout && fout != stdout)
      fclose(fout);
  }
  nonogram_freepuzzle(&puzzle);
  return t ? 0 : -1;
}

int judge(context *c, const char *progname)
{
  FILE *fin;
  nonogram_puzzle puzzle;
  float rating;

  if (c->ifile) {
    fin = fopen(c->ifile, "r");
    if (!fin) {
      fprintf(stderr, "%s: could not read\n", c->ifile);
      return -1;
    }
  } else {
    fin = stdin;
  }

  if (nonogram_fscanpuzzle(&puzzle, fin) < 0) {
    if (c->ifile)
      fclose(fin);
    fprintf(stderr, "%s: error on input\n", progname);
    return -1;
  }
  if (fin && fin != stdin)
    fclose(fin);

  rating = nonogram_judgepuzzle(&puzzle);
  if (c->verbose)
    fprintf(stderr, "Puzzle difficulty: %.0f (%s)\n", rating,
            c->ifile ? c->ifile : "stdin");
  printf("%.0f\n", rating);

  nonogram_freepuzzle(&puzzle);
  return 0;
}

int encodepuzzle(context *c, const char *progname)
{
  FILE *fin;
  char *ptr, *buf = NULL;
  size_t buflen = 0;
  nonogram_puzzle puzzle;

  if (c->ifile) {
    fin = fopen(c->ifile, "r");
    if (!fin) {
      fprintf(stderr, "%s: could not read\n", c->ifile);
      return -1;
    }
  } else {
    fin = stdin;
  }

  if (nonogram_fscanpuzzle(&puzzle, fin) < 0) {
    if (c->ifile)
      fclose(fin);
    fprintf(stderr, "%s: error on input\n", progname);
    return -1;
  }

  if (fin && fin != stdin)
    fclose(fin);

  int rc = -1;
  if (nonocache_encodepuzzle(NULL, &buflen, &puzzle) == 0 &&
      (ptr = buf = malloc(buflen + 1)) != NULL &&
      nonocache_encodepuzzle(&ptr, &buflen, &puzzle) == 0) {
    *ptr = '\0';
    printf("%s\n", buf);
    rc = 0;
  }
  free(buf);

  nonogram_freepuzzle(&puzzle);
  return rc;
}

int solve(context *c, const char *progname)
{
  FILE *fin;
  struct h_tag handle;
  time_t nowtime;
  int tries, diff;

  time(&nowtime);

  nonogram_initsolver(&handle.solver);

  handle.logfp = NULL;
  if (c->logfile) {
    handle.logfp = fopen(c->logfile, c->logappend ? "a" : "w");
    if (!handle.logfp) {
      fprintf(stderr, "%s: could not open\n", c->logfile);
      return -1;
    }
  }

  if (handle.logfp) {
    fprintf(handle.logfp, "log started at %s\n", ctime(&nowtime));
    fflush(handle.logfp);
  }

  handle.c = c;
  if (c->ifile) {
    fin = fopen(c->ifile, "r");
    if (!fin) {
      fprintf(stderr, "%s: could not read\n", c->ifile);
      if (handle.logfp)
        fclose(handle.logfp);
      return -1;
    }
  } else {
    fin = stdin;
  }

  if (nonogram_fscanpuzzle(&handle.puzzle, fin) < 0) {
    if (c->ifile)
      fclose(fin);
    fprintf(stderr, "%s: error on input\n", progname);
    if (handle.logfp)
      fclose(handle.logfp);
    return -1;
  }

  if (fin && fin != stdin)
    fclose(fin);

  /* verify puzzle integrity */
  diff = nonogram_verifypuzzle(&handle.puzzle);
  if (diff && c->verbose)
    fprintf(stderr, "Warning: puzzle (%s) has imbalance: %d\n",
            c->ifile ? c->ifile : "stdin", diff);

  handle.fp = NULL;
  if (c->ofiletype == SINGLE) {
    handle.fp = fopen(c->ofile, "w");
    if (!handle.fp) {
      fprintf(stderr, "%s: can't write file\n", c->ofile);
      nonogram_freepuzzle(&handle.puzzle);
      if (handle.logfp)
        fclose(handle.logfp);
      return -1;
    }
  }

  if (!diff || c->visual) {
    handle.grid = nonogram_makegrid(nonogram_puzzlewidth(&handle.puzzle),
                                    nonogram_puzzleheight(&handle.puzzle));
    nonogram_cleargrid(handle.grid, nonogram_puzzlewidth(&handle.puzzle),
                       nonogram_puzzleheight(&handle.puzzle));
  }

#ifndef NOVISUAL
  if (c->visual) {
    clear_screen();
#ifdef BBCGFX
    draw_grid(nonogram_puzzlewidth(&handle.puzzle),
              nonogram_puzzleheight(&handle.puzzle));
#endif
  }
#endif

  handle.solno = 0;
  if (!diff || c->visual) {
    nonogram_setalgo(&handle.solver, c->algo);
    nonogram_setlog(&handle.solver, handle.logfp, 0, c->loglevel);
    nonogram_setclient(&handle.solver, &our_client, &handle);
  }

#ifndef NOVISUAL
  if (c->visual)
    nonogram_setdisplay(&handle.solver, &our_display, &handle);
#endif

  /* now load the puzzle after configuration */
  nonogram_load(&handle.solver, &handle.puzzle, handle.grid,
                nonogram_puzzlewidth(&handle.puzzle) *
                nonogram_puzzleheight(&handle.puzzle));

#ifndef NOVISUAL
  if (c->visual) {
    (*our_display.rowmark)(&handle, 0,
                           nonogram_puzzleheight(&handle.puzzle) - 1);
    (*our_display.colmark)(&handle, 0,
                           nonogram_puzzlewidth(&handle.puzzle) - 1);
  }
#endif

  if (!diff || c->visual) {
    handle.stop = false;
    while (!handle.stop &&
           nonogram_runsolver_n(&handle.solver,
                                (tries = 1, &tries)) != nonogram_FINISHED)
      ;
  }

#ifndef NOVISUAL
  if (c->visual)
    move_to(0, nonogram_puzzleheight(&handle.puzzle) + 6);
#endif

  if (!diff || c->visual) {
    nonogram_termsolver(&handle.solver);
    nonogram_freegrid(handle.grid);
  }

  nonogram_freepuzzle(&handle.puzzle);
  if (handle.fp)
    fclose(handle.fp);

  if (handle.logfp) {
    time(&nowtime);
    fprintf(handle.logfp, "log ended at %s\n", ctime(&nowtime));
    fclose(handle.logfp);
  }

  if (c->countonerr)
    fprintf(stderr, "%d\n", handle.solno);
  if (c->countonout)
    fprintf(stdout, "%d\n", handle.solno);
  return 0;
}

int print_usage(FILE *fp, const char *progname)
{
  int count = 0;

  count += fprintf(fp, "usage: %s [-o output] [-on out%%dput] [-os] [+o]\n",
                   progname);
  count += fprintf(fp, "\t"
#ifndef NOVISUAL
                   "[-/+v] "
#endif
                   "[-/+table] [-(n)log logfile] [+(n)log] [-ll level]\n");
  count += fprintf(fp, "\t[-html] [-dot str] [-solid str]"
                   " [-blank str] [+dot/solid/blank]\n");
  count += fprintf(fp, "\t[+html] [-dot str] [-solid str]"
                   " [-blank str] [+dot/solid/blank]\n"
                   "\t[-i input] [-is] [-s num] [+s] [-h] [-L] [-O]\n"
                   "\t[-/+CO] [-/+CE] [-/+q]\n"
                   "\t[-penc]\n"
                   "\t[-Affcomp] [-Afcomp] [-Afast] [-Anull] [-Acomplete]"
                   " [-Ahybrid]\n"
                   "\t[-Aolsak] [-Afastolsak]\n"
                   "\t[-Afastodd] [-Afastolsakcomplete] [-Afastoddcomplete]\n"
                   "\t[-title string]\n");
#ifndef OLDSWITCHES
  count += fprintf(fp, "\t[-p] [-x] [-c] [-g] [-j] [-T] [-W] [-H]\n");
  count += fprintf(fp, "\t[-Xname] [-Rname value] [-Dname]"
                   " [-Sname value]\n");
#endif
  return count;
}

int print_options(FILE *fp, const char *pname)
{
  int count = 0;

  count += fprintf(fp, "Program: %s\n", pname);
  count += fprintf(fp, "Program version: %s\n", VERSION);
  count += fprintf(fp, "Date compiled: %s\n", __DATE__);
  count += fprintf(fp, "Nonogram library version: %d.%d.%d\n",
                   nonogram_VERSION, nonogram_MINOR,
                   nonogram_PATCHLEVEL);
  count += fprintf(fp, "Nonogram library date compiled: %s\n",
                   nonogram_date);

#if defined(OLDSWITCHES)
  count += fprintf(fp, "OLDSWITCHES: use -i/is switches for -x\n");
  count += fprintf(fp, "   *** Deprecated ***\n");
#elif defined(NOAUTOEXEC)
  count += fprintf(fp, "NOAUTOEXEC: don't assume a trailing -x\n");
  count += fprintf(fp, "   *** Deprecated ***\n");
#endif

#if defined(NOVISUAL)
  count += fprintf(fp, "NOVISUAL: live visual display (-v) disabled\n");
#elif defined(BBCGFX)
  count += fprintf(fp, "BBCGFX: use BBC VDU plot codes for -v\n");
#elif defined(BBCVDU)
  count += fprintf(fp, "BBCVDU: use BBC VDU 31 codes for -v\n");
#endif

  return count;
}

void init_context(context *p)
{
  p->verbose = true;
  p->html = p->error = false;
  p->title = NULL;
  p->table = true;
  p->countonerr = p->countonout = false;
  p->html_dot = default_html_dot;
  p->html_solid = default_html_solid;
  p->html_blank = default_html_blank;
  p->dot = default_dot;
  p->solid = default_solid;
  p->blank = default_blank;
  p->visual = false;
  p->ofiletype = STDSTREAM;
  p->logfile = NULL;
  p->loglevel = 0;
  p->logappend = true;
  p->limit = 0;
  p->algo = nonogram_AFFCOMP;
#ifndef NOAUTOEXEC
  p->executed = true;
#endif
#ifndef OLDSWITCHES
  p->ifile = NULL;
#endif
}

int print_context(context *p, FILE *fp)
{
  int count = 0;

#ifndef OLDSWITCHES
  count += fprintf(fp, p->ifile ? "Input:\tfile %s\n" : "Input:\tstdin\n",
                   p->ifile);
#endif
  switch (p->ofiletype) {
  case NONE:
    count += fprintf(fp, "Output:\tnone\n");
    break;
  case STDSTREAM:
    count += fprintf(fp, "Output:\tstdout\n");
    break;
  case MULTIPLE:
    count += fprintf(fp, "Output:\tfiles %s\n", p->ofile);
    break;
  case SINGLE:
    count += fprintf(fp, "Output:\tfile %s\n", p->ofile);
    break;
  }
  count += fprintf(fp, "HTML:\t%sabled\n", p->html ? "en" : "dis");
  count += fprintf(fp, "Table:\t%sabled\n", p->table ? "en" : "dis");
  count += fprintf(fp, "Informational:\t%s\n",
                   p->verbose ? "verbose" : "silent");
  count += fprintf(fp, "Solution limit:\t");
  if (p->limit)
    count += fprintf(fp, "%d\n", p->limit);
  else
    count += fprintf(fp, "disabled\n");
  count += fprintf(fp, "Count on stdout:\t%sabled\n",
                   p->countonout ? "en" : "dis");
  count += fprintf(fp, "Count on stderr:\t%sabled\n",
                   p->countonerr ? "en" : "dis");
  count += fprintf(fp, "Visual:\t%s\n", p->visual ? "on" : "off");
  if (p->logfile)
    count += fprintf(fp, "Log:\tfile %s\n", p->logfile);
  else
    count += fprintf(fp, "Log:\toff\n");
  count += fprintf(fp, "Log level:\t%u\n", p->loglevel);
  count += fprintf
    (fp, "Line solver:\t%s\n",
     p->algo == nonogram_AFASTODDONES ? "fastodd" :
     p->algo == nonogram_AFASTOLSAK ? "fastolsak" :
     p->algo == nonogram_AFASTODDONESCOMPLETE ? "fastoddcomplete" :
     p->algo == nonogram_AFASTOLSAKCOMPLETE ? "fastolsakcomplete" :
     p->algo == nonogram_AOLSAK ? "olsak" :
     p->algo == nonogram_AHYBRID ? "hybrid" :
     p->algo == nonogram_AFAST ? "fast" :
     p->algo == nonogram_AFCOMP ? "fcomp" :
     p->algo == nonogram_AFFCOMP ? "ffcomp" :
     p->algo == nonogram_ANULL ? "null" :
     p->algo == nonogram_ACOMPLETE ? "complete" : "unknown");
  if (p->title)
    count += fprintf(fp, "Title: \"%s\"\n", p->title);
  else
    count += fprintf(fp, "Title: none\n");
  count += fprintf(fp, "Text output:\n");
  count += fprintf(fp, "Dot:\t%s\n", p->dot);
  count += fprintf(fp, "Solid:\t%s\n", p->solid);
  count += fprintf(fp, "Blank:\t%s\n", p->blank);
  count += fprintf(fp, "HTML output:\n");
  count += fprintf(fp, "Dot:\t%s\n", p->html_dot);
  count += fprintf(fp, "Solid:\t%s\n", p->html_solid);
  count += fprintf(fp, "Blank:\t%s\n", p->html_blank);
  return count;
}

int parse_args(context *c, int argc, const char *argv[], const char *pname)
{
  int arg = 0;

  while (arg < argc) {
    if (argv[arg][0] == '-') {
      if (!strcmp(argv[arg] + 1, "html")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        c->html = true;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "table")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        c->table = true;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "v")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        c->visual = true;
#ifdef NOVISUAL
        if (c->verbose)
          fprintf(stderr, "%s: warning: -v disabled\n", pname);
#endif
        arg++;
      } else if (!strcmp(argv[arg] + 1, "os")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        c->ofiletype = STDSTREAM;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "h")) {
        print_usage(stderr, pname);
        arg++;
      } else if (!strcmp(argv[arg] + 1, "q")) {
        c->verbose = false;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "O")) {
        print_options(stderr, pname);
        arg++;
      } else if (!strcmp(argv[arg] + 1, "Afast")) {
        c->algo = nonogram_AFAST;
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        arg++;
      } else if (!strcmp(argv[arg] + 1, "Afcomp")) {
        c->algo = nonogram_AFCOMP;
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        arg++;
      } else if (!strcmp(argv[arg] + 1, "Affcomp")) {
        c->algo = nonogram_AFFCOMP;
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        arg++;
      } else if (!strcmp(argv[arg] + 1, "Acomplete")) {
        c->algo = nonogram_ACOMPLETE;
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        arg++;
      } else if (!strcmp(argv[arg] + 1, "Anull")) {
        c->algo = nonogram_ANULL;
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        arg++;
      } else if (!strcmp(argv[arg] + 1, "Ahybrid")) {
        c->algo = nonogram_AHYBRID;
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        arg++;
      } else if (!strcmp(argv[arg] + 1, "Afastolsak")) {
        c->algo = nonogram_AFASTOLSAK;
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        arg++;
      } else if (!strcmp(argv[arg] + 1, "Afastodd")) {
        c->algo = nonogram_AFASTODDONES;
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        arg++;
      } else if (!strcmp(argv[arg] + 1, "Afastolsakcomplete")) {
        c->algo = nonogram_AFASTOLSAKCOMPLETE;
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        arg++;
      } else if (!strcmp(argv[arg] + 1, "Afastoddcomplete")) {
        c->algo = nonogram_AFASTODDONESCOMPLETE;
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        arg++;
      } else if (!strcmp(argv[arg] + 1, "Aolsak")) {
        c->algo = nonogram_AOLSAK;
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        arg++;
      } else if (!strcmp(argv[arg] + 1, "CO")) {
        c->countonout = true;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "CE")) {
        c->countonerr = true;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "Gdot") ||
                 !strcmp(argv[arg] + 1, "Gsolid")) {
        if (c->verbose)
          fprintf(stderr, "warning: %s ignored\n", argv[arg]);
        arg++;
      } else if (!strcmp(argv[arg] + 1, "is")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        c->ifile = NULL;
#ifdef OLDSWITCHES
        if (solve(c, pname))
          return EXIT_FAILURE;
#endif
        arg++;
      } else if (!strcmp(argv[arg] + 1, "title")) {
        if (arg + 1 >= argc) {
          c->error = true;
          fprintf(stderr, "%s: switch %s needs string argument\n",
                  pname, argv[arg]);
          arg = argc;
        } else {
          c->title = argv[arg + 1];
          arg += 2;
        }
      } else if (!strcmp(argv[arg] + 1, "log")) {
        if (arg + 1 >= argc) {
          c->error = true;
          fprintf(stderr, "%s: switch %s needs string argument\n",
                  pname, argv[arg]);
          arg = argc;
        } else {
          c->logfile = argv[arg + 1];
          c->logappend = true;
          arg += 2;
        }
      } else if (!strcmp(argv[arg] + 1, "nlog")) {
        if (arg + 1 >= argc) {
          c->error = true;
          fprintf(stderr, "%s: switch %s needs string argument\n",
                  pname, argv[arg]);
          arg = argc;
        } else {
          c->logfile = argv[arg + 1];
          c->logappend = false;
          arg += 2;
        }
      } else if (!strcmp(argv[arg] + 1, "ll")) {
        unsigned nlev;
        char *rem;
        if (arg + 1 >= argc ||
            (nlev = strtoul(argv[arg + 1], &rem, 10), *rem)) {
          c->error = true;
          fprintf(stderr, "%s: switch %s needs whole number argument\n",
                  pname, argv[arg]);
          arg = argc;
        } else {
          c->loglevel = nlev;
          arg += 2;
        }
      } else if (!strcmp(argv[arg] + 1, "s")) {
        if (arg + 1 >= argc) {
          c->error = true;
          fprintf(stderr, "%s: switch %s needs a number argument\n",
                  pname, argv[arg]);
          arg = argc;
        } else {
          c->limit = atoi(argv[arg + 1]);
          if (!c->limit && c->verbose)
            fprintf(stderr, "%s: warning - limit disabled\n", pname);
          arg += 2;
        }
      } else if (!strcmp(argv[arg] + 1, "i")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        if (arg + 1 >= argc) {
          c->error = true;
          fprintf(stderr, "%s: switch %s needs string argument\n",
                  pname, argv[arg]);
          arg = argc;
        } else {
          c->ifile = argv[arg + 1];
#ifdef OLDSWITCHES
          if (solve(c, pname) < 0)
            return EXIT_FAILURE;
#endif
          arg += 2;
        }
#ifndef OLDSWITCHES
      } else if (!strcmp(argv[arg] + 1, "x")) {
#ifndef NOAUTOEXEC
        c->executed = true;
#endif
        if (solve(c, pname) < 0)
          return EXIT_FAILURE;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "penc")) {
#ifndef NOAUTOEXEC
        c->executed = true;
#endif
        if (encodepuzzle(c, pname) < 0)
          return EXIT_FAILURE;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "T")) {
#ifndef NOAUTOEXEC
        c->executed = true;
#endif
        if (extractpart(c, pname, TITLE) < 0)
          return EXIT_FAILURE;
        arg++;
      } else if (argv[arg][1] == 'X') {
#ifndef NOAUTOEXEC
        c->executed = true;
#endif
        if (extractnote(c, pname, argv[arg] + 2) < 0)
          if (arg + 1 == argc)
            return EXIT_FAILURE;
        arg++;
      } else if (argv[arg][1] == 'D') {
#ifndef NOAUTOEXEC
        c->executed = true;
#endif
        if (deletenote(c, pname, argv[arg] + 2) < 0)
          if (arg + 1 == argc)
            return EXIT_FAILURE;
        arg++;
      } else if (argv[arg][1] == 'R') {
        if (arg + 1 == argc) {
          if (c->verbose)
            fprintf(stderr, "usage: %s -Rname value\n", pname);
          return EXIT_FAILURE;
        } else {
#ifndef NOAUTOEXEC
          c->executed = true;
#endif
          if (replacenote(c, pname, argv[arg] + 2, argv[arg + 1], false) < 0)
            if (arg + 2 == argc)
              return EXIT_FAILURE;
          arg += 2;
        }
      } else if (argv[arg][1] == 'S') {
        if (arg + 1 == argc) {
          if (c->verbose)
            fprintf(stderr, "usage: %s -Sname value\n", pname);
          return EXIT_FAILURE;
        } else {
#ifndef NOAUTOEXEC
          c->executed = true;
#endif
          if (replacenote(c, pname, argv[arg] + 2, argv[arg + 1], true) < 0)
            if (arg + 2 == argc)
              return EXIT_FAILURE;
          arg += 2;
        }
      } else if (!strcmp(argv[arg] + 1, "W")) {
#ifndef NOAUTOEXEC
        c->executed = true;
#endif
        if (extractpart(c, pname, WIDTH) < 0)
          return EXIT_FAILURE;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "H")) {
#ifndef NOAUTOEXEC
        c->executed = true;
#endif
        if (extractpart(c, pname, HEIGHT) < 0)
          return EXIT_FAILURE;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "g")) {
#ifndef NOAUTOEXEC
        c->executed = true;
#endif
        if (create(c, pname) < 0)
          return EXIT_FAILURE;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "c")) {
#ifndef NOAUTOEXEC
        c->executed = true;
#endif
        if (verify(c, pname) < 0)
          return EXIT_FAILURE;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "j")) {
#ifndef NOAUTOEXEC
        c->executed = true;
#endif
        if (judge(c, pname) < 0)
          return EXIT_FAILURE;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "p")) {
#ifndef NOAUTOEXEC
        c->executed = true;
#endif
        print_context(c, stderr);
        arg++;
#endif
      } else if (!strcmp(argv[arg] + 1, "o")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        if (arg + 1 >= argc) {
          c->error = true;
          fprintf(stderr, "%s: switch %s needs string argument\n",
                  pname, argv[arg]);
          arg = argc;
        } else {
          c->ofile = argv[arg + 1];
          c->ofiletype = SINGLE;
          arg += 2;
        }
      } else if (!strcmp(argv[arg] + 1, "on")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        if (arg + 1 >= argc) {
          c->error = true;
          fprintf(stderr, "%s: switch %s needs string argument\n",
                  pname, argv[arg]);
          arg = argc;
        } else {
          c->ofile = argv[arg + 1];
          c->ofiletype = MULTIPLE;
          arg += 2;
        }
      } else if (!strcmp(argv[arg] + 1, "dot")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        if (arg + 1 >= argc) {
          c->error = true;
          fprintf(stderr, "%s: switch %s needs string argument\n",
                  pname, argv[arg]);
          arg = argc;
        } else {
          if (c->html)
            c->html_dot = argv[arg + 1];
          else
            c->dot = argv[arg + 1];
          arg += 2;
        }
      } else if (!strcmp(argv[arg] + 1, "solid")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        if (arg + 1 >= argc) {
          c->error = true;
          fprintf(stderr, "%s: switch %s needs string argument\n",
                  pname, argv[arg]);
          arg = argc;
        } else {
          if (c->html)
            c->html_solid = argv[arg + 1];
          else
            c->solid = argv[arg + 1];
          arg += 2;
        }
      } else if (!strcmp(argv[arg] + 1, "blank")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        if (arg + 1 >= argc) {
          c->error = true;
          fprintf(stderr, "%s: switch %s needs string argument\n",
                  pname, argv[arg]);
          arg = argc;
        } else {
          if (c->html)
            c->html_blank = argv[arg + 1];
          else
            c->blank = argv[arg + 1];
          arg += 2;
        }
      } else {
        c->error = true;
        fprintf(stderr, "%s: unrecognised switch %s\n", pname, argv[arg]);
        arg = argc;
      }
    } else if (argv[arg][0] == '+') {
      if (!strcmp(argv[arg] + 1, "html")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        c->html = false;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "CO")) {
        c->countonout = false;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "CE")) {
        c->countonerr = false;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "s")) {
        c->limit = 0;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "table")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        c->table = false;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "v")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        c->visual = false;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "q")) {
        c->verbose = true;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "o")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        c->ofiletype = NONE;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "log")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        c->logfile = NULL;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "nlog")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        c->logfile = NULL;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "title")) {
        c->title = NULL;
        arg++;
      } else if (!strcmp(argv[arg] + 1, "dot")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        if (c->html)
          c->html_dot = default_html_dot;
        else
          c->dot = default_dot;
        arg += 2;
      } else if (!strcmp(argv[arg] + 1, "solid")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        if (c->html)
          c->html_solid = default_html_solid;
        else
          c->solid = default_solid;
        arg += 2;
      } else if (!strcmp(argv[arg] + 1, "blank")) {
#ifndef NOAUTOEXEC
        c->executed = false;
#endif
        if (c->html)
          c->html_blank = default_html_blank;
        else
          c->blank = default_blank;
        arg += 2;
      } else {
        c->error = true;
        fprintf(stderr, "%s: unrecognised switch %s\n", pname, argv[arg]);
        arg = argc;
      }
    } else {
      c->error = true;
      fprintf(stderr, "%s: unrecognised argument %s\n", pname, argv[arg]);
      arg = argc;
    }
  }
  if (c->error) {
    print_usage(stderr, pname);
    return EXIT_FAILURE;
  }
#if !defined(NOAUTOEXEC) && !defined(OLDSWITCHES)
  if (!c->executed) {
    c->executed = true;
    if (solve(c, pname) < 0)
      return EXIT_FAILURE;
  }
#endif
  return EXIT_SUCCESS;
}

nonogram_cell *loadgrid(size_t *width, size_t *height,
                          FILE *fp, char solid, char dot)
{
  struct line {
    struct line *prev;
    nonogram_cell *data;
  };

  char temp[300];
  nonogram_cell *result;
  size_t len, i;
  struct line *bottom;

  if (!width || !height) return NULL;
  bottom = NULL;
  *width = 0;
  *height = 0;

  while (fgets(temp, sizeof temp, fp)) {
    struct line *next;

    len = strlen(temp);
    if (len > 0 && temp[len - 1] == '\n')
      temp[--len] = '\0';

    if (*width < 1) {
      *width = len;
    } else if (len != *width) {
      goto finished;
    }

    next = malloc(sizeof(struct line));
    if (!next)
      goto failure;
    next->data = malloc(*width * sizeof(nonogram_cell));
    if (!next->data)
      goto failure;
    for (i = 0; i < *width; i++)
      next->data[i] = temp[i] == dot ? nonogram_DOT :
      (temp[i] == solid ? nonogram_SOLID : nonogram_BLANK);
    next->prev = bottom;
    bottom = next;
    (*height)++;
  }

finished:
  result = nonogram_makegrid(*width, *height);
  if (!result)
    goto failure;

  for (i = *height; i > 0; ) {
    struct line *prev = bottom->prev;
    i--;
    memcpy(result + i * *width, bottom->data, *width);
    free(bottom->data);
    free(bottom);
    bottom = prev;
  }

  return result;

failure:
  while (bottom) {
    struct line *prev = bottom->prev;
    free(bottom->data);
    free(bottom);
    bottom = prev;
  }

  return NULL;
}
