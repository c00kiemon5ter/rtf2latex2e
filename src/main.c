/*
 * RTF-to-LaTeX2e translation driver code.
 * (c) 1999 Ujwal S. Sathyam
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include    <ctype.h>
#include    <stdarg.h>

#include     "rtf.h"
#include     "mygetopt.h"
#include     "rtf2latex2e.h"

#define      LIBDIR       "/opt/local"

extern long  groupLevel;
extern char  outputMapName[];
FILE         *OpenLibFile(char *name, char *mode);

FILE         *ifp, *ofp;
extern char  fileCreator[];
extern long  groupLevel;

#if RTF2LATEX2E_DOS
#include <Windows.h>
#endif

char         *g_library_path = NULL;
int          g_little_endian = 0;
int          g_debug_level   = 0;
int          g_include_both  = 0;

/* Figure out endianness of machine.  Needed for OLE & graphics support */
static void 
SetEndianness(void)
{
    unsigned int    endian_test = (unsigned int) 0xaabbccdd;
    unsigned char   endian_test_char = *(unsigned char *) &endian_test;
    if (endian_test_char == 0xdd)
        g_little_endian = 1;
}


/****************************************************************************
 * purpose:  append path to .cfg file name and open
             return NULL upon failure,
             return filepointer otherwise
 ****************************************************************************/
static FILE    *
try_path(char *path, char *file, char *mode)
{
    char           *both;
    FILE           *fp = NULL;
    size_t          lastchar;

    if (path == NULL || file == NULL)
        return NULL;

    /* fprintf(stderr, "trying path=<%s> file=<%s>\n", path, file); */

    lastchar = strlen(path);

    both = malloc(strlen(path) + strlen(file) + 2);
    if (both == NULL) {
        fprintf(stderr, "Could not allocate memory for both strings.");
        return NULL;
    }
    strcpy(both, path);

    /* fix path ending if needed */
    if (both[lastchar] != PATH_SEP) {
        both[lastchar] = PATH_SEP;
        both[lastchar + 1] = '\0';
    }
    strcat(both, file);
    /* fprintf(stderr, "trying filename=<%s>\n\n", both); */
    fp = fopen(both, mode);
    free(both);
    return fp;
}

/****************************************************************************
purpose: open library files by trying multiple paths
 ****************************************************************************/
FILE           *
OpenLibFile(char *name, char *mode)
{
    char           *env_path, *p, *p1;
    char           *lib_path;
    FILE           *fp;

    /* try path specified on the line */
    fp = try_path(g_library_path, name, mode);
    if (fp)
        return fp;

    /* try the environment variable RTFPATH */
    p = getenv("RTFPATH");
    if (p) {
        env_path = strdup(p);   /* create a copy to work with */
        p = env_path;
        while (p) {
            p1 = strchr(p, ENV_SEP);
            if (p1)
                *p1 = '\0';

            fp = try_path(p, name, mode);
            if (fp) {
                free(env_path);
                return fp;
            }
            p = (p1) ? p1 + 1 : NULL;
        }
        free(env_path);
    }
    /* last resort.  try LIBDIR from compile time */
    lib_path = strdup(LIBDIR);
    if (lib_path) {
        p = lib_path;
        while (p) {
            p1 = strchr(p, ENV_SEP);
            if (p1)
                *p1 = '\0';

            fp = try_path(p, name, mode);
            if (fp) {
                free(lib_path);
                return fp;
            }
            p = (p1) ? p1 + 1 : NULL;
        }
        free(lib_path);
    }
    /* failed ... give some feedback */
    {
        char           *s;
        fprintf(stderr, "Cannot open the rtf2latex library files\n");
        fprintf(stderr, "Locate the directory containing the rtf2latex binary, \n");
        fprintf(stderr, "the character set map files, and the output map TeX-map file\n\n");
        fprintf(stderr, "Then you can\n");
        fprintf(stderr, "   (1) define the environment variable RTFPATH, *or*\n");
        fprintf(stderr, "   (2) use command line path option \"-P /path/to/cfg/file\", *or*\n");
        fprintf(stderr, "   (3) recompile rtf2latex with LIBDIR defined properly\n");
        s = getenv("RTFPATH");
        fprintf(stderr, "Current RTFPATH: %s", (s) ? s : "not defined\n");
        s = LIBDIR;
        fprintf(stderr, "Compiledin support directory: %s", (s) ? s : "not defined\n\n");
        fprintf(stderr, " Depending on your shell, you can set the environment variable RTFPATH using\n");
        fprintf(stderr, "     export RTFPATH=directory (bash) or \n");
        fprintf(stderr, "     setenv RTFPATH directory (csh) or \n");
        fprintf(stderr, "     SET RTFPATH=directory (DOS) or \n");
        fprintf(stderr, "You should also add this directory to your search path.\n");
        fprintf(stderr, "You can set these variables in your .bash_profile, .login, or autoexec.bat file.\n\n");
        fprintf(stderr, "Giving up.  Please don't hate me.\n");
    }
    return NULL;
}

static void 
print_version(void)
{
    fprintf(stdout, "rtf2latex %s\n\n", rtf2latex2e_version);
    fprintf(stdout, "Copyright (C) 2010 Free Software Foundation, Inc.\n");
    fprintf(stdout, "This is free software; see the source for copying conditions.  There is NO\n");
    fprintf(stdout, "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n");
}

static void 
print_usage(void)
{
    char           *s;

    fprintf(stdout, "`rtf2latex' converts text files in RTF format to the LaTeX text format.\n\n");
    fprintf(stdout, "Usage:  rtf2latex [-t <TeX-map>] <rtf file>\n\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -h               display help\n");
    fprintf(stdout, "  -b               include latex & picts for equations\n");
    fprintf(stdout, "  -o outputfile    file for RTF output\n");
    fprintf(stdout, "  -P path          paths to *.cfg & latex2png\n");
    fprintf(stdout, "  -t /path/to/tmp  temporary directory\n");
    fprintf(stdout, "  -v               version information\n");
    fprintf(stdout, "  -V               version information\n");
    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "  rtf2latex foo                     convert foo.rtf to foo.tex\n");
    fprintf(stdout, "  rtf2latex <foo >foo.TEX             convert foo to foo.TEX\n");
    fprintf(stdout, "  rtf2latex -P ./cfg/:./scripts/ foo  look for library files in ./cfg\n\n");
    fprintf(stdout, "Report bugs to <rtf2latex2e-developers@lists.sourceforge.net>\n\n");
    fprintf(stdout, "$RTFPATH designates the directory for the library files \n");
    s = getenv("RTFPATH");
    fprintf(stdout, "$RTFPATH = '%s'\n\n", (s) ? s : "not defined");
    s = LIBDIR;
    fprintf(stdout, "LIBDIR compiled-in directory for configuration files (*.cfg)\n");
    fprintf(stdout, "LIBDIR  = '%s'\n\n", (s) ? s : "not defined");
    fprintf(stdout, "rtf2latex %s\n", rtf2latex2e_version);
    exit(1);
}

int 
main(int argc, char **argv)
{
    char            c, buf[rtfBufSiz], *buf1, buf2[rtfBufSiz];
    int             i, fileCounter;
    long            cursorPos, bufLength;
    extern char    *optarg;
    extern int      optind;


    while ((c = my_getopt(argc, argv, "bhvVt:P:")) != EOF) {
        switch (c) {

        case 'b':
            g_include_both = 1;
            break;

        case 'v':
        case 'V':
            print_version();
            return (0);

        case 't':
            strcpy(outputMapName, optarg);
            break;

        case 'P':   /* -P path/to/cfg:path/to/script or -P
                 * path/to/cfg or -P :path/to/script */
            g_library_path = strdup(optarg);
            break;

        case 'h':
        case '?':
        default:
            print_usage();
        }
    }

    argc -= optind;
    argv += optind;

    /* Initialize stuff */
    if (argc > 0) {
        SetEndianness();/* needed for cole routines */
        strcpy(outputMapName, "");  /* assume that special
                         * TeX-Map is not used */
        RTFSetOpenLibFileProc(OpenLibFile); /* install new routine
                             * for opening library
                             * files */
        WriterInit();   /* one time writer initialization */
    } else {
        print_usage();
    }

    for (fileCounter = 0; fileCounter < argc; fileCounter++) {
        fprintf(stderr, "Processing %s\n", argv[fileCounter]);

        RTFInit();

        /*
         * open first file, set it as the input file, and enable
         * global access to input file name
         */
        /* strip .rtf from file name if found */
        if ((ifp = fopen(argv[fileCounter], "r")) == NULL) {
            RTFPanic("* Cannot open input file %s\n", argv[fileCounter]);
            exit(1);
        }
        RTFSetStream(ifp);

        /* look at second token to check if input file is of type rtf */
        cursorPos = ftell(ifp);
        RTFGetToken();
        RTFGetToken();
        if (rtfMajor != rtfVersion) {
            RTFMsg("* Oops! %s is not an rtf file!\n", argv[fileCounter]);
            if (fclose(ifp) != 0)
                printf("* error closing input file %s\n",
                       argv[fileCounter]);
            continue;
        }
        fseek(ifp, cursorPos, 0);
        RTFInit();

        strcpy(buf2, argv[fileCounter]);
        buf1 = buf2;
        bufLength = strlen(buf1);
        if (bufLength > 3) {
            buf1 += (bufLength - 4);
            /* strip .rtf by terminating string */
            if (strcmp(buf1, ".rtf") == 0 || strcmp(buf1, ".RTF") == 0)
                buf2[bufLength - 4] = '\0';

        }
        /* replace any spaces in input file name by a dash - */
        buf1 = strrchr(buf2, PATH_SEP); /* strip away path info */
        if (buf1 == (char *) NULL)
            buf1 = buf2;
        bufLength = strlen(buf1);
        for (i = 0; i < bufLength; i++) {
            if (buf1[i] == ' ' || buf1[i] == '_' || buf1[i] == '.')
                buf1[i] = '-';
        }

        /* set input name to modified name of input file */
        RTFSetInputName(buf2);

        /*
         * open output file, set it as the output file, and enable
         * global access to output file name
         */
        strcpy(buf, buf2);
        strcat(buf, ".tex");

        if ((ofp = fopen(buf, "w+")) == NULL)
            RTFPanic("Cannot open output file %s\n", buf);

        RTFSetOutputStream(ofp);
        RTFSetOutputName(buf);

        if (BeginLaTeXFile()) {
            RTFRead();
            EndLaTeXFile();
        }
        
        if (fclose(ifp) != 0)
            printf("* error closing input file %s\n", argv[fileCounter]);
        if (fclose(ofp) != 0)
            printf("* error closing output file %s\n", buf);
    }

    /* printf ("* groupLevel is %d\n", groupLevel); */

    return (0);
}
