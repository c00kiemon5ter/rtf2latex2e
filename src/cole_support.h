/*
   Support - Provides some big and little endian abstraction functions.
   Copyright (C) 1999  Roberto Arturo Tena Sanchez  

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */
/*
   Arturo Tena <arturo@directmail.org>
 */
/*
  Some code was from Caolan, but I have replaced all the code,
  now all code here is mine, so I changed copyright announce in cole-1.0.0.
     Arturo Tena
 */


#ifndef COLE_SUPPORT_H
#define COLE_SUPPORT_H

#include <ctype.h>
#include <stdio.h>
#include <stdint.h>

#define U8  uint8_t
#define U16 uint16_t
#define U32 uint32_t
#define F32 float
#define F64 double

U16 fil_sreadU16 (U8 * in);
U32 fil_sreadU32 (U8 * in);
F64 fil_sreadF64 (U8 * in);

void fil_swriteU16 (U8 * dest, U16 * src);
void fil_swriteU32 (U8 * dest, U32 * src);

void __cole_dump (void *_m, void *_start, int length, char *msg);

#define verbose_return()               \
  {                                    \
    verbose_wonl ("returning from ");  \
    verbose_wonl (__FILE__);           \
    verbose_wonl (":");                \
    verbose_d (__LINE__);              \
  }

#define test(t,retval)    \
  {                       \
    if (!(t))             \
    {                     \
      verbose_return ();  \
      return retval;      \
    }                     \
  }
#define test_exitf(t,retval,func)  \
  {                                \
    if (!(t))                      \
    {                              \
      func;                        \
      verbose_return ();           \
      return (retval);             \
    }                              \
  }
#define test_call(t,typeretval)  \
  {                              \
    typeretval retval;           \
    retval = (t);                \
    if (retval)                  \
    {                            \
      verbose_return ()          \
      return (retval);           \
    }                            \
  }
#define test_call_exitf(t,typeretval,func)  \
  {                                         \
    typeretval retval;                      \
    retval = (t);                           \
    if (retval)                             \
    {                                       \
      func;                                 \
      verbose_return ()                     \
      return (retval);                      \
    }                                       \
  }
#define report_bug(prog)                                                 \
  {                                                                      \
    fprintf (stderr, #prog": A bug have been found: %s:%d\n"#prog        \
             ":Please, download a most recent version and try again\n",  \
             __FILE__, __LINE__);                                        \
  }
#define assert_return(prog,t,retval)                                  \
  {                                                                   \
    if (!(t))                                                         \
    {                                                                 \
      fprintf (stderr, #prog": Condition "#t" is not valid: %s:%d\n", \
                       __FILE__, __LINE__);                           \
      report_bug (prog);                                              \
      return (retval);                                                \
    }                                                                 \
  }


#ifdef COLE_VERBOSE

#define verbose_d(n) { printf ("%d\n", n); }
#define verbose(s) { printf (s); printf ("\n"); }
#define verbose_wonl(s) { printf (s); }
#define verboseU8(expr)  { printf (#expr " = 0x%02x\n", expr); }
#define verboseU16(expr) { printf (#expr " = 0x%04x\n", expr); }
#define verboseU32(expr) { printf (#expr " = 0x%08x\n", expr); }
#define verboseS(expr) { printf (#expr " = %s\n", expr); }
#define verboseS_wonl(expr) { printf (#expr " = %s", expr); }
#define warning(t) { if (!(t)) printf ("warning: %s is false\n", #t); }

#else /* ifndef COLE_VERBOSE */

#define verbose_d(n) ;
#define verbose(s) ;
#define verbose_wonl(s) ;
#define verboseU8(expr) ;
#define verboseU16(expr) ;
#define verboseU32(expr) ;
#define verboseS(expr)
#define verboseS_wonl(expr)
#define warning(t)

#endif /* ifdef COLE_VERBOSE */


#ifdef COLE_VERBOSE
#define verboseU32Array(array,len)                              \
  {                                                             \
    U32 temp;                                                   \
    for (temp = 0; temp < len; temp++)                          \
      printf (#array "[%u] = 0x%08x\n", temp, array [temp]);  \
  }
#else
#define verboseU32Array(array,len)
#endif


#define verboseU8Array_force(rec,len,reclen)			\
	__cole_dump ((rec), (rec), ((len)*(reclen)), "");


#ifdef COLE_VERBOSE
#define verboseU8Array(rec,len,reclen) verboseU8Array_force(rec,len,reclen)
#else
#define verboseU8Array(rec,len,reclen)
#endif

#endif
