/*
   cole - A free C OLE library.
   Copyright 1998, 1999  Roberto Arturo Tena Sanchez

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
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
   Arturo Tena <arturo@directmail.org>
 */

#include <stdio.h>
#include <stdlib.h>

#include "cole_internal.h"


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
