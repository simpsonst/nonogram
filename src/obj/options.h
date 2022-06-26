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

/* define compilation options for main program (not library) here */

/* These should really be defined by compiler options. */

#ifndef OPTIONS_HEADER
#define OPTIONS_HEADER

/* backward-compatibility options */

/*
#define NOVISUAL
#define BBCVDU
#define NOAUTOEXEC
 */

#define PTRCONST const

#if defined(__riscos) || defined(__riscos__)
#define BBCGFX

/* a bug fix */
#ifdef __EASY_C
#undef PTRCONST
#define PTRCONST
#endif

#endif

#endif
