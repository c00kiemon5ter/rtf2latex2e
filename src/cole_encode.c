/*
   OLEcode - Generate a Microsoft OLE 2 file from given streams.
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


#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "cole.h"
#include "cole_support.h"
#include "cole_internal.h"

/* warning: some lines are longer than 80 characters */

struct str_MY_FILE {
    enum {
        real, MY_FILE_list, block_list, root_list
    } type;
    size_t size;                   /* size of list _itself_ */
    U32 *blocks;                /* size in big blocks (0x0200) of the information
                                   that contains this file */
    union union_file {
        struct str_real {
            char *name;         /* file name */
            U32 ppsnumber;      /* ppsnumber of pps property in stream_list */
        } real;
        struct str_MY_FILE *MY_FILE_list;
        U32 *block_list;
        U8 *root_list;
    } file;
};
typedef struct str_MY_FILE MY_FILE;


static MY_FILE Input;
static MY_FILE *sbfile;
static MY_FILE *SDepot;
static MY_FILE *BDepot;
static MY_FILE *bbd_list;
static MY_FILE *Root;

/* starting and sizing blocks (calculated in calculate_blocks()) */
static U32 sbfile_start_block;  /* where sbfile starts */
static U32 BDepot_start_block;  /* where BDepot starts */

static FILE *output_file;
/* the output file OLE2 file */
static U8 output_block[0x0200];
/* used as buffer to write later to output_file */
static U16 pos_block;
/*
   position inside output_block from where the next write will happen,
   when it cames 0x0200 must write the block to output_file and
   make pos_block = 0x00
 */
static U32 next_block;
/* number of the next block to be written in output_file.
   the first block is -1, the second 0, the third 1 and so on */

/* process stage functions */
static U32 max_pps_referenced(pps_entry * pps_list, U32 node);
static U32 max3(U32 a, U32 b, U32 c, U32 d);
static int process_streams(pps_entry * pps_list, pps_entry * root);
static int add_stream_to_sbfile_and_SDepot(U32 size, char *name,
                                           U32 ppsnumber);
static int add_stream_to_Input_and_BDepot(U32 size, char *name,
                                          U32 ppsnumber);
static U32 add_MY_FILE_entry(MY_FILE * list, U32 size);
static void reset_links_in_Input(void);
static void reset_links_in_BDepot(void);
static void reset_links_in_SDepot(void);
/* generate starge functions */
static int generate_recursive(MY_FILE * list);
static int generate_real_file(MY_FILE * MY_FILE_file);
static int write_block_list(U32 start_count, MY_FILE * list,
                            int write_end_chain);

/* support functions for both stages */
static U32 sum_block_list(MY_FILE * list);
/* useless function by now, may be later */
/* static U32 sum_MY_FILE_list (MY_FILE * list); */
static U32 sum_blocks_MY_FILE_list(MY_FILE * list);

#define size2blocks(s,b) ((U32)(!(s) ? 1 : (1+((s)-1)/(b))))
#define size2blocks_preserve_zero(s,b) ((U32)(!(s) ? 0 : (1+((s)-1)/(b))))
#define clean_block(b,s) memset((b),0xff,(s))
#define init_MY_FILE(n, t, s, b, f) { \
  (n)->type = t; \
  (n)->size = (s); \
  (n)->blocks = (b); \
  (n)->file.t = (f); \
  }
#define init_MY_FILE_real(n, t, s, b, f, p) { \
  n->type = t; \
  n->size = s; \
  n->blocks = b; \
  n->file.real.name = f; \
  n->file.real.ppsnumber = p; \
  }

#define reset_links() { reset_links_in_Input(); reset_links_in_BDepot(); reset_links_in_SDepot(); }
/* if this block is full, write it to output_file and
   restart using this block */
#define check_output_block_boundary() { \
  if (pos_block == 0x0200) { \
    test_exitf (fwrite (output_block, 0x0200, 1, output_file) == 1, 1, dummy()); \
    next_block++; \
    pos_block = 0x00; \
    } \
  }
#define write_until_output_block_boundary(clean) { \
  if (pos_block != 0x00) { \
    if (clean && pos_block%0x0200) \
      clean_block (output_block + pos_block, (pos_block/0x0200 + 1)*0x0200 - pos_block); \
    test_exitf (fwrite (output_block, 1, 0x0200, output_file) == 0x0200, 1, dummy ()); \
    next_block++; \
    pos_block = 0x00; \
  } \
}
#define write_until_output_block_small_boundary(clean) { \
  if (pos_block % 0x40) { \
    if (clean) \
      clean_block (output_block + pos_block, (pos_block/0x40 + 1)*0x40 - pos_block); \
    assert (pos_block+(pos_block/0x40 + 1)*0x40 - pos_block == (pos_block/0x40 + 1)*0x40); \
    pos_block += (pos_block/0x40 + 1)*0x40 - pos_block; \
  } \
}

#define write_rest_of_output_block_with_null_pps() { \
  if (pos_block != 0x00) { \
    int zzzi; \
    U16 U16zero = 0x0000U; \
    clean_block (output_block + pos_block, 0x0200 - pos_block); \
     for (zzzi = 0; zzzi < 4; zzzi++) \
      if (zzzi*0x80 >= pos_block) \
        fil_swriteU16 (output_block + zzzi*0x80 + 0x40, &U16zero); \
    } \
  }
#define dummy()
/* 
may be should be (means no op, do nothing):
#define dummy() {1;}
or may be:
#define dummy() {;}
*/

#define MAX(a,b) ((a) > (b) ? (a) : (b))
U32 max3(U32 a, U32 b, U32 c, U32 d)
{
    U32 m = 0;

    /*verbose ("calling: max3 ()"); */

    m = MAX(m, a);
    m = MAX(m, b);
    m = MAX(m, c);
    m = MAX(m, d);
    return m;
}


U32 max_pps_referenced(pps_entry * pps_list, U32 node)
{
    U32 max_pps;

    /*verbose ("calling: max_pps_referenced ()"); */

    max_pps = max3(node,
                   pps_list[node].previous !=
                   0xffffffff ? pps_list[node].previous : 0,
                   pps_list[node].next !=
                   0xffffffff ? pps_list[node].next : 0,
                   pps_list[node].dir !=
                   0xffffffff ? pps_list[node].dir : 0);

    if (pps_list[node].previous != 0xffffffff)
        max_pps = MAX(max_pps,
                      max_pps_referenced(pps_list,
                                         pps_list[node].previous));
    if (pps_list[node].next != 0xffffffff)
        max_pps = MAX(max_pps,
                      max_pps_referenced(pps_list, pps_list[node].next));
    if (pps_list[node].dir != 0xffffffff)
        max_pps = MAX(max_pps,
                      max_pps_referenced(pps_list, pps_list[node].dir));

    return max_pps;
}


/* reviewed when coding ole2 file */
int process_streams(pps_entry * pps_list, pps_entry * node)
{
    U32 U32end_chain = 0xfffffffe;

    verbose("calling: process_streams ()");

    test_exitf(node->name[0], 12, dummy());
    switch (node->type) {
    case 1:                     /* dir */
        warning(node->size == 0);
        if (node->dir != 0xffffffff)
            test_call(process_streams(pps_list, &pps_list[node->dir]),
                      int);
        if (node->next != 0xffffffff)
            test_call(process_streams(pps_list, &pps_list[node->next]),
                      int);
        break;

    case 5:                     /* root dir */
        assert(*(Root->file.root_list + 0x42) == 5);
        /* write the default start block of SDepot: empty if there are no
           sbfile at all */
        fil_swriteU32(Root->file.root_list + 0x74, &U32end_chain);
        if (node->dir != 0xffffffff)
            test_call(process_streams(pps_list, &pps_list[node->dir]),
                      int);
        if (node->next != 0xffffffff)
            test_call(process_streams(pps_list, &pps_list[node->next]),
                      int);
        break;

    case 2:                     /* file */
        test_exitf(node->dir == 0xffffffff, 12, dummy());
        if (node->size < 0x1000)
            /* must be in sbfile, and its block list in SDepot */
            test_call(add_stream_to_sbfile_and_SDepot
                      (node->size, node->filename, node->ppsnumber), int)
                else            /* node->size >= 0x1000 */
            /* must be in Input, and its block list in BDepot */
             test_call(add_stream_to_Input_and_BDepot
                       (node->size, node->filename, node->ppsnumber), int);
        if (node->next != 0xffffffff)
            test_call(process_streams(pps_list, &pps_list[node->next]),
                      int);
        break;

    default:
        return 12;
    }

    return 0;
}

/* reviewed when processing streams */
int add_stream_to_sbfile_and_SDepot(U32 size, char *name, U32 ppsnumber)
{
    U32 entry_number;
    U32 *new_entry_SDepot;
    MY_FILE *new_entry_sbfile;

    verbose("calling: add_stream_to_sbfile_and_SDepot ()");
    /* must be called only by process streams */

    /* 1. add entries in SDepot and sbfile */
    /*    add in SDepot */
    entry_number = add_MY_FILE_entry(SDepot, size);
    test_exitf(entry_number != 0xffffffff, 10, dummy());
    new_entry_SDepot = SDepot->file.block_list + entry_number;
    /*    add in sbfile */
    entry_number = add_MY_FILE_entry(sbfile, size);
    test_exitf(entry_number != 0xffffffff, 10, dummy());
    new_entry_sbfile = sbfile->file.MY_FILE_list + entry_number;

    /* 2. write info about the new stream in the new entry in SDepot */
    /*    write info in new entry (info related with parameter size) */
    /*          write blocks size of stream added */
    *new_entry_SDepot = size2blocks(size, 0x40);        /* 0x40 because is small stream */
    /* 3. update Input entry of SDepot */
    /*            update blocks size of SDepot (need info in new_entry_SDepot written) */
    *(SDepot->blocks) =
        size2blocks(sum_block_list(SDepot) * sizeof(U32), 0x0200);

    /* 4. write info about the new stream in the new entry in sbfile */
    /*    write info in new entry (info related with parameter size */
    /*                             and block size depot) */
    /*           write size of stream added */
    /*                 and write blocks size link of stream added */
    init_MY_FILE_real(new_entry_sbfile, real, size, new_entry_SDepot, name,
                      ppsnumber);
    /* 5. update Input entry of sbfile */
    /*            update blocks size of sbfile (need info in new_entry_sbfile->size */
    /*                   and new_entry_sbfile->blocks written) */
    *(sbfile->blocks) =
        size2blocks(sum_blocks_MY_FILE_list(sbfile) * 0x40, 0x0200);

    /* 6. update Input entry of BDepot */
    /*            update blocks size of BDepot (need blocks size of */
    /*                   SDepot and sbfile updated */
    *(BDepot->blocks) =
        size2blocks(sum_block_list(BDepot) * sizeof(U32), 0x0200);

    return 0;
}


/* reviewed when processing streams */
int add_stream_to_Input_and_BDepot(U32 size, char *name, U32 ppsnumber)
{
    U32 entry_number;
    MY_FILE *new_entry_Input;
    U32 *new_entry_BDepot;

    verbose("calling: add_stream_to_Input_and_BDepot ()");
    /* must be called only by process streams */

    /* 1. add entries in BDepot and Input */
    /*    add in BDepot */
    entry_number = add_MY_FILE_entry(BDepot, size);
    test_exitf(entry_number != 0xffffffff, 10, dummy());
    new_entry_BDepot = BDepot->file.block_list + entry_number;
    /*    add in Input */
    entry_number = add_MY_FILE_entry(&Input, size);
    test_exitf(entry_number != 0xffffffff, 10, dummy());
    new_entry_Input = Input.file.MY_FILE_list + entry_number;

    /* 2. write info about the new stream in the new entry in BDepot */
    /*    write info in new entry (info related with parameter size) */
    /*          write blocks size of stream added */
    *new_entry_BDepot = size2blocks(size, 0x0200);      /* 0x0200 because is big stream */
    /* 3. update Input entry of BDepot */
    /*            update blocks size of BDepot (need info in new_entry_BDepot written) */
    *(BDepot->blocks) =
        size2blocks(sum_block_list(BDepot) * sizeof(U32), 0x0200);

    /* 4. write info about the new stream in the new entry in Input */
    /*    write info in new entry (info related with parameter size */
    /*                             and block size depot) */
    /*           write size of stream added */
    /*                 and write blocks size link of stream added */
    init_MY_FILE_real(new_entry_Input, real, size, new_entry_BDepot, name,
                      ppsnumber);
    /* 5. ?? update Input entry of Input ?? */
    /*    this seems to be not aplicable here, not needed */

    /* 6. update Input entry of BDepot */
    /*            update blocks size of BDepot (need blocks size of */
    /*                   BbDepot and ?? Input ?? updated */
    *(BDepot->blocks) =
        size2blocks(sum_block_list(BDepot) * sizeof(U32), 0x0200);

    return 0;
}


/* reviewed all conditions */
U32 add_MY_FILE_entry(MY_FILE * list, U32 size)
{
    verbose("calling: add_MY_FILE_entry ()");

#ifdef VERBOSE
    if (list == &Input) {
        verbose_wonl("add_MY_FILE_entry: list = &Input, ");
    } else if (list == sbfile) {
        verbose_wonl("add_MY_FILE_entry: list = sbfile, ");
    } else if (list == SDepot) {
        verbose_wonl("add_MY_FILE_entry: list = SDepot, ");
    } else if (list == BDepot) {
        verbose_wonl("add_MY_FILE_entry: list = BDepot, ");
    } else if (list == bbd_list) {
        verbose_wonl("add_MY_FILE_entry: list = bbd_list, ");
    } else if (list == Root) {
        verbose_wonl("add_MY_FILE_entry: list = Root, ");
    } else {
        verbose_wonl("add_MY_FILE_entry: list = UNKNOWN (ERROR!), ");
    }
    verboseU32(size);
#endif

#define verbose_structures() { \
  if (SDepot->file.block_list!=NULL) \
    verboseU32Array (SDepot->file.block_list, (U32)SDepot->size / sizeof(U32)); \
  verboseU32 ((U32) Root->size); \
  verboseU32Array (BDepot->file.block_list, (U32)BDepot->size / sizeof(U32)); \
  verboseU32 ((U32) *(bbd_list->file.block_list)); \
  }

    assert(list != NULL);

    switch (list->type) {

/* reviewed when adding to sbfile */
/* reviewed when adding to Input */
    case MY_FILE_list:{
            MY_FILE *new_MY_FILE_list;
            U32 new_entry;

            assert(list == sbfile || list == &Input);
            /* actually add */
            list->size = list->size + sizeof(MY_FILE);
            new_MY_FILE_list =
                realloc(list->file.MY_FILE_list, list->size);
            list->file.MY_FILE_list = new_MY_FILE_list;
            test_exitf(new_MY_FILE_list != NULL, 0xffffffff, dummy());
            new_entry = (U32) (list->size / sizeof(MY_FILE) - 1);
            reset_links();

            return new_entry;
        }

/* reviewed when adding to SDepot */
/* reviewed when adding to BDepot */
    case block_list:{
            U32 *new_block_list;
            U32 new_entry;

            assert(list == SDepot || list == BDepot);
            /* actually add */
            list->size = list->size + sizeof(U32);
            new_block_list = realloc(list->file.block_list, list->size);
            list->file.block_list = new_block_list;
            test_exitf(new_block_list != NULL, 0xffffffff, dummy());
            new_entry = (U32) (list->size / sizeof(U32) - 1);
            reset_links();

            return new_entry;
        }

/* reviewed when adding to Root */
    case root_list:{
            U32 new_entry;
            U8 *new_root_list;

            assert(list == Root);
            /* actually add */
            list->size = list->size + 0x80;
            new_root_list = realloc(list->file.root_list, list->size);
            list->file.root_list = new_root_list;
            test_exitf(new_root_list != NULL, 0xffffffff, dummy());
            new_entry = (U32) (list->size / 0x80 - 1);
            reset_links();

            return new_entry;
        }

    default:
        verbose_structures();
        assert("list->type UNKNOWN in add_MY_FILE_entry" == NULL);
        return 0;
    }
}


U32 sum_block_list(MY_FILE * list)
{
    U32 sum = 0;
    U32 *block;

    /*verbose ("calling: sum_block_list ()"); */

    assert(list != NULL);
    assert(list->type == block_list);
    for (block = list->file.block_list;
         ((U8 *) block - (U8 *) list->file.block_list) < list->size;
         block++)
        sum += *block;

    return sum;
}


/*
U32
sum_MY_FILE_list (MY_FILE * list)
{
  U32 sum = 0;
  MY_FILE *file;

  verbose ("calling: sum_MY_FILE_list ()");

  assert (list != NULL);
  assert (list->type == MY_FILE_list);
  for (file = list->file.MY_FILE_list;
       ((U8 *) file - (U8 *) list->file.MY_FILE_list) < list->size;
       file++)
    sum += file->size;

  return sum;
}
*/


U32 sum_blocks_MY_FILE_list(MY_FILE * list)
{
    U32 sum = 0;
    MY_FILE *file;

    /*verbose ("calling: sum_blocks_MY_FILE_list ()"); */

    assert(list != NULL);
    assert(list->type == MY_FILE_list);
    for (file = list->file.MY_FILE_list;
         ((U8 *) file - (U8 *) list->file.MY_FILE_list) < list->size;
         file++)
        if (file->blocks != NULL)
            sum += *(file->blocks);
    return sum;
}


void reset_links_in_Input(void)
{
    verbose("calling: reset_links_in_Input ()");

    sbfile = Input.file.MY_FILE_list + 4;
    SDepot = Input.file.MY_FILE_list + 3;
    BDepot = Input.file.MY_FILE_list + 1;
    bbd_list = Input.file.MY_FILE_list;
    Root = Input.file.MY_FILE_list + 2;
}


void reset_links_in_BDepot(void)
{
    U32 i;

    verbose("calling: reset_links_in_BDepot ()");

    sbfile->blocks = BDepot->file.block_list;
    SDepot->blocks = BDepot->file.block_list + 1;
    Root->blocks = BDepot->file.block_list + 2;

    /* relink big streams block sizes in Input with BDepot entries */
    for (i = 0; i < (Input.size / sizeof(MY_FILE)) - 5; i++)
        Input.file.MY_FILE_list[i + 5].blocks =
            BDepot->file.block_list + i + 3;
}


void reset_links_in_SDepot(void)
{
    U32 i;

    verbose("calling: reset_links_in_SDepot ()");

    /* relink small streams block sizes in sbfile with SDepot entries */
    for (i = 0; i < sbfile->size / sizeof(MY_FILE); i++)
        sbfile->file.MY_FILE_list[i].blocks = SDepot->file.block_list + i;
}


/* reviewed all cases */
int generate_recursive(MY_FILE * list)
{
    MY_FILE *p_MY_FILE_list;

    verbose("calling: generate_recursive ()");

    switch (list->type) {

        /* reviewed when generating Input and sbfile */
    case MY_FILE_list:
        for (p_MY_FILE_list = list->file.MY_FILE_list;
             ((U8 *) p_MY_FILE_list - (U8 *) list->file.root_list) <
             list->size; p_MY_FILE_list++)
            test_call(generate_recursive(p_MY_FILE_list), int);
        break;

        /* reviewed when generating bbd_list, BDepot and SDepot */
    case block_list:
        if (list == bbd_list) {
            test_call(write_block_list(BDepot_start_block, bbd_list, 0),
                      int);
            write_until_output_block_boundary(1);
            break;
        } else if (list == BDepot)
            /* we want to skip generate BDepot by now */
            break;
        else if (list == SDepot)
            /* we want to skip generate SDepot by now */
            break;
        else
            assert
                ("list->type==block_list but list UNKNOWN in generate_recursive"
                 == NULL);

        /* reviewed when generating Root */
    case root_list:
        /* we want to skip generate Root by now */
        assert(list == Root);
        break;

    case real:
        /* we are generating big and small streams here */
        test_call(generate_real_file(list), int);
        break;

    default:
        assert("list->type UNKNOWN in generate_recursive" == NULL);
    }

    return 0;
}

int generate_real_file(MY_FILE * MY_FILE_file)
{
    FILE *file;
    uint32_t n_read;
    U8 *pps;
    U32 total_bytes;
    U32 sbfile_size;
    static U32 last_small_stream_next_block = 0;
    static int sbfile_start_block_set = 0;
    static int sbfile_may_need_write_until_boundary = 0;

    verbose("calling: generate_real_file ()");

/* FIXME MARK 3 */
    /* all seems to be working until here... I hope. Test is welcome! */

    /*verboseU16 (sbfile_start_block_set); */
    /*verboseU16 (sbfile_may_need_write_until_boundary); */
    /*verboseU16 (pos_block); */
    assert(pos_block <= 0x0200);
    assert(pos_block % 0x40 == 0);

    /* open real file */
    assert(MY_FILE_file->file.real.name[0]);
    file = fopen(MY_FILE_file->file.real.name, "rb");
    test_exitf(file != NULL, 11, dummy());

    /* write Root start_block's of this file */
    if (MY_FILE_file->size >= 0x1000) {
        /* generating big stream */

        /* first, end writting sbfile, if any */
        if (sbfile_may_need_write_until_boundary) {
            /* this happens after all small strams have been generated but
               before the first big stream is generated */
            write_until_output_block_boundary(1);
            sbfile_may_need_write_until_boundary = 0;
        }
        /* then, continue with this big stream */

        /* write start block of this stream in its Root pps */
        verboseU32(next_block);
        pps =
            Root->file.root_list +
            MY_FILE_file->file.real.ppsnumber * 0x80;
        fil_swriteU32(pps + 0x74, &next_block);
    } else {
        /* generating small stream */
        /* do nothing, start block of small streams was written
           in proces_streams */

        /* write start block of this stream in its Root pps */
        pps =
            Root->file.root_list +
            MY_FILE_file->file.real.ppsnumber * 0x80;
        fil_swriteU32(pps + 0x74, &last_small_stream_next_block);
        last_small_stream_next_block += *(MY_FILE_file->blocks);        /* small blocks */

        /* write sbfile start block in root directory pps in Root, if not done */
        if (!sbfile_start_block_set) {
            /* this happens before generate the first small stream that makes sbfile */
            verbose("generating sbfile");
            verbose("SUPPOSING: first entry in Root is root entry");
            /* write start block of sbfile */
            assert(sbfile_start_block == next_block);
            fil_swriteU32(Root->file.root_list + 0x74, &next_block);

            sbfile_size = sum_blocks_MY_FILE_list(sbfile) * 0x40;
            verboseU32(sbfile_size);
            /* compare calculated sbfile size with original */
            assert(sbfile_size ==
                   fil_sreadU32(Root->file.root_list + 0x78));

            sbfile_start_block_set = 1;
            sbfile_may_need_write_until_boundary = 1;
        }
    }


    verboseS(MY_FILE_file->file.real.name);
    total_bytes = 0;
    /* copy real file to output_file */
    while (!feof(file)) {
        n_read = (uint32_t) fread(output_block + pos_block, 1, 0x0200 - pos_block, file);
        test_exitf(!ferror(file), 11, dummy());
        if (n_read < 0x0200 - pos_block)
            /* if it was readed less than it could be possible */
            assert(feof(file));
        pos_block += n_read;
        total_bytes += n_read;
        check_output_block_boundary();
    }
    test_exitf(total_bytes == MY_FILE_file->size, 12, dummy());


    if (MY_FILE_file->size >= 0x1000)
        /* generating big stream */
        write_until_output_block_boundary(1)
            else
        /* generating small stream */
        write_until_output_block_small_boundary(1);

    /* close real file */
    fclose(file);

    return 0;
}


int write_block_list(U32 start_count, MY_FILE * list, int write_end_chain)
{
    U32 *p;
    U32 n;
    U32 end_chain = 0xfffffffe;
    U32 value_to_write;
    U32 delta;

    verbose("calling: write_block_list ()");

    assert(list->type = block_list);
    assert(pos_block <= 0x01fc);        /* 0x01fc = 0x0200 - sizeof(U32) */

    delta = start_count;
    if (list->size == 0)
        return 0;               /* we are done */
    for (p = list->file.block_list;
         ((U8 *) p - (U8 *) list->file.block_list) < list->size; p++) {
        for (n = 0; n < *p; n++) {
            /* this allow bbd_list runs over the block number 2 */
            check_output_block_boundary();

            /* if it's the last block in chain */
            if (write_end_chain && !(n + 1 < *p))
                value_to_write = end_chain;
            else
                value_to_write = n + delta;
            fil_swriteU32(output_block + pos_block, &value_to_write);
            pos_block += sizeof(U32);
            assert(pos_block <= 0x0200);
        }
        delta += n;
    }
    check_output_block_boundary();

    return 0;
}


/* You can use next lines to print some parts of Structure */
/*
verboseU32Array (SDepot->file.block_list, SDepot->size / sizeof(U32));
verboseU8Array (Root->file.root_list, Root->size / 0x80, 0x80);
verboseU32Array (BDepot->file.block_list, BDepot->size / sizeof (U32));
verboseU32 (*(bbd_list->file.block_list));
*/
