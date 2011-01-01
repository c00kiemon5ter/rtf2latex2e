/*
   Support - Provides some big and little endian abstraction functions,
             besides another things.
   Copyright (C) 1999  Roberto Arturo Tena Sanchez <arturo@directmail.org>

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
     
  Merged with internal.c 
 */

#include <stdio.h>
#include <stdlib.h>
#include "cole_support.h"

uint16_t fil_sreadU16(uint8_t * in)
{
#ifdef WORDS_BIGENDIAN
    uint16_t ret;
    *((uint8_t *) (&ret)) = *(in + 1);
    *(((uint8_t *) (&ret)) + 1) = *in;
    return ret;
#else                           /* !WORDS_BIGENDIAN */
    return *((uint16_t *) in);
#endif
}


uint32_t fil_sreadU32(uint8_t * in)
{
#ifdef WORDS_BIGENDIAN
    uint32_t ret;
    *(((uint8_t *) (&ret)) + 3) = *in;
    *(((uint8_t *) (&ret)) + 2) = *(in + 1);
    *(((uint8_t *) (&ret)) + 1) = *(in + 2);
    *((uint8_t *) (&ret)) = *(in + 3);
    return ret;
#else                           /* !WORDS_BIGENDIAN */
    return *((uint32_t *) in);
#endif
}


F64 fil_sreadF64(uint8_t * in)
{
#ifdef WORDS_BIGENDIAN
    F64 ret;
    *(((uint8_t *) (&ret)) + 7) = *in;
    *(((uint8_t *) (&ret)) + 6) = *(in + 1);
    *(((uint8_t *) (&ret)) + 5) = *(in + 2);
    *(((uint8_t *) (&ret)) + 4) = *(in + 3);
    *(((uint8_t *) (&ret)) + 3) = *(in + 4);
    *(((uint8_t *) (&ret)) + 2) = *(in + 5);
    *(((uint8_t *) (&ret)) + 1) = *(in + 6);
    *((uint8_t *) (&ret)) = *(in + 7);
    return ret;
#else                           /* !WORDS_BIGENDIAN */
    return *((F64 *) in);
#endif
}


void fil_swriteU16(uint8_t * dest, uint16_t * src)
{
#ifdef WORDS_BIGENDIAN
    *(dest) = *(((uint8_t *) src) + 1);
    *(dest + 1) = *((uint8_t *) src);
#else                           /* !WORDS_BIGENDIAN */
    *(dest) = *((uint8_t *) src);
    *(dest + 1) = *(((uint8_t *) src) + 1);
#endif
}


void fil_swriteU32(uint8_t * dest, uint32_t * src)
{
#ifdef WORDS_BIGENDIAN
    fil_swriteU16(dest, (uint16_t *) (((uint8_t *) src) + 2));
    fil_swriteU16(dest + 2, (uint16_t *) src);
#else                           /* !WORDS_BIGENDIAN */
    fil_swriteU16(dest, (uint16_t *) src);
    fil_swriteU16(dest + 2, (uint16_t *) (((uint8_t *) src) + 2));
#endif
}


/*-*
        @func (void) __cole_dump dump the content of memory
        @param (void *) m memory position which content will be dumped
        @param (void *) start memory position from where calculate
                                offset
        @param (uint32_t) length size in bytes of the dumped memory
        @param (char *) msg optional message, can be NULL
 *-*/
void __cole_dump(void *_m, void *_start, uint32_t length, char *msg)
{
    unsigned char *pm;
    char buff[18];
    long achar;
    unsigned char *m;
    unsigned char *start;

    if (_m == NULL) {
        printf("VERBOSE: can't dump because m is NULL\n");
        return;
    }
    if (_start == NULL) {
        printf("VERBOSE: can't dump because start is NULL\n");
        return;
    }

    m = (unsigned char *) _m;
    start = (unsigned char *) _start;
    buff[8] = ' ';
    buff[17] = 0;
    if (msg != NULL)
        printf("VERBOSE: %s (from 0x%p length 0x%08x (%d)):\n", msg, m, (unsigned int) length, (int) length);
    for (pm = m; pm - m < length; pm++) {
        achar = (pm - m) % 16;
        /* print offset */
        if (achar == 0)
            printf("%08lx  ", (unsigned long) ((pm - m) + (m - start)));
        /* write char in the right column buffer */
        buff[achar + (achar < 8 ? 0 : 1)] = (isprint(*pm) ? *pm : '.');
        /* print next char */
        if (!((pm - m + 1) % 16))
            /* print right column */
            printf("%02x  %s\n", *pm, buff);
        else if (!((pm - m + 1) % 8))
            printf("%02x  ", *pm);
        else
            printf("%02x ", *pm);
    }
    achar = (pm - m) % 16;
    if (achar) {
        int i;
        for (i = 0; i < (16 - achar) * 3 - 1; i++)
            printf(" ");
        if (achar != 8)
            buff[achar] = 0;
        printf("  %s\n", buff);
    }
}


#define MIN(a,b) ((a)<(b) ? (a) : (b))


int
__cole_extract_file(FILE ** file, char **filename, uint32_t size, uint32_t pps_start,
                    uint8_t * BDepot, uint8_t * SDepot, FILE * sbfile,
                    FILE * inputfile)
{
    /* FIXME rewrite this cleaner */

    FILE *ret;
    uint16_t BlockSize, Offset;
    uint8_t *Depot;
    FILE *infile;
    long FilePos;
    size_t bytes_to_copy;
    uint8_t Block[0x0200];

    *filename = malloc(L_tmpnam);       /* It must be L_tmpnam + 1? */
    if (*filename == NULL)
        return 1;

    if (tmpnam(*filename) == NULL) {
        free(*filename);
        return 2;
    }
	
    ret = fopen(*filename, "w+b");
    *file = ret;
    if (ret == NULL) {
        free(*filename);
        return 3;
    }
	
    if (size >= 0x1000) {
        /* read from big block depot */
        Offset = 1;
        BlockSize = 0x0200;
        infile = inputfile;
        Depot = BDepot;
    } else {
        /* read from small block file */
        Offset = 0;
        BlockSize = 0x40;
        infile = sbfile;
        Depot = SDepot;
    }
    while (pps_start < 0xfffffffd) {
        FilePos = (long) ((pps_start + Offset) * BlockSize);
        if (FilePos < 0) {
            fclose(ret);
            remove(*filename);
            free(*filename);
            return 4;
        }
        bytes_to_copy = MIN(BlockSize, size);
        if (fseek(infile, FilePos, SEEK_SET)) {
            fclose(ret);
            remove(*filename);
            free(*filename);
            return 4;
        }
        fread(Block, bytes_to_copy, 1, infile);
        if (ferror(infile)) {
            fclose(ret);
            remove(*filename);
            free(*filename);
            return 5;
        }
        fwrite(Block, bytes_to_copy, 1, ret);
        if (ferror(ret)) {
            fclose(ret);
            remove(*filename);
            free(*filename);
            return 6;
        }
        pps_start = fil_sreadU32(Depot + (pps_start * 4));
        size -= MIN(BlockSize, size);
        if (size == 0)
            break;
    }

    return 0;
}
