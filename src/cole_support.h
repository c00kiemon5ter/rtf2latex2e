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

  Some code was from Caolan, but I have replaced all the code,
  now all code here is mine, so I changed copyright announce in cole-1.0.0.
     Arturo Tena
     
  This has been merged with the internal.h file
 */


#ifndef COLE_SUPPORT_H
#define COLE_SUPPORT_H

#include <ctype.h>
#include <stdio.h>
#include <stdint.h>

#define F32 float
#define F64 double

uint16_t fil_sreadU16 (uint8_t * in);
uint32_t fil_sreadU32 (uint8_t * in);
F64 fil_sreadF64 (uint8_t * in);

void fil_swriteU16 (uint8_t * dest, uint16_t * src);
void fil_swriteU32 (uint8_t * dest, uint32_t * src);

void __cole_dump (void *_m, void *_start, uint32_t length, char *msg);

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
    uint32_t temp;                                                   \
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

/* This structure describe one stream. */
struct pps_block
  {
    char name[0x20];
    char filename[L_tmpnam];	/* valid only if type == 2 */
    uint8_t type;			/* 5 == root, 1 == dir, 2 == file */
    uint32_t size;			/* the size of the file,
				           valid only if type == 2 */
    uint32_t next;			/* next entry in this level,
				           this directory */
    uint32_t dir;			/* valid only if type != 2 */
    uint16_t level;			/* level in the ole tree */
    uint32_t seconds1;
    uint32_t seconds2;
    uint32_t days1;
    uint32_t days2;
    uint32_t start;			/* start block */

    /* private fields, used only inside OLEdecode and OLEcode,
       don't modify them if you want to use OLEcode */
    uint32_t previous;		/* previous pps, valid before reordering */
    uint32_t ppsnumber;		/* pps number */
  };
typedef struct pps_block pps_entry;


/*
   Create a OLE stream tree from a file.
   Input: char *Olefilename        = File to be decoded (ie. .xsl, .doc, .ppt).
   .      pps_entry ** stream_list = The stream tree.
   .      uint32_t * root               = The number of root dir in stream_list.
   .      uint8_t **_BDepot, uint8_t **_SDepot, FILE **_sbfile, char **_sbfilename,
   .      FILE **_input,
   .                               = Exposes internals, read only.
   .      uint16_t max_level            = The maximum level on stream tree in which
   .                                 streams will be actually extracted
   .                                 to a file. 0 (zero) means extract all.
   Output: 0 = Sucess.
   .       4 = Couldn't open OLEfilename file (can use perror).
   .       8 = OLEfilename file seems to contain plain text, not OLE file.
   .       9 = OLEfilename is a binary file, but it have not OLEfile format.
   .       5 = Error reading from file, means OLEfilename file has a faulty
   .           OLE file format (UPDATE: not always).
   .       6 = Error removing temporal files.  <-- this is never returned now
   .       7 = Error creating temporal files, can use perror.
   .       10 = Error allocating memory, there's no more memory.
 */
int __OLEdecode (char *OLEfilename, pps_entry ** stream_list, size_t * root,
		 uint8_t **_BDepot, uint8_t **_SDepot, FILE **_sbfile, char **_sbfilename,
		 FILE **_input, uint16_t max_level);

struct _COLEFS {
	/* This structure is for internal use only, not for the public API */
	pps_entry *tree;
	size_t root;			/* entry root, root pps_entry */
	uint8_t *BDepot;
	uint8_t *SDepot;
	FILE *sbfile;
	char *sbfilename;
	FILE *file;			/* actual file (the filesystem) */
};
struct _COLEDIRENT {
	/* This structure is for internal use only, not for the public API */
	size_t entry;
	struct _COLEDIR *dir;		/* father */
};
struct _COLEDIR {
	/* This structure is for internal use only, not for the public API */
	size_t entry;
	struct _COLEDIRENT visited_entry;
	struct _COLEFS *fs;		/* father */
};
struct _COLEFILE {
	/* This structure is for internal use only, not for the public API */
	size_t entry;
	FILE *file;			/* actual extracted file */
	char *filename;			/* actual extracted file's name */
	uint32_t filesize;			/* actual extracted file size */
	struct _COLEFS *fs;		/* father */
	size_t pos;			/* file pointer position */
};
int __cole_extract_file (FILE **file, char **filename, uint32_t size,
			 uint32_t pps_start, uint8_t *BDepot, uint8_t *SDepot, FILE *sbfile,
			 FILE *inputfile);
#define _COLE_TYPE_DIR 1
#define _COLE_TYPE_FILE 2
#define _COLE_TYPE_ROOT 5

#endif
