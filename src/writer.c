/*
 * RTF-to-LaTeX2e translation writer code.
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

# include <stdint.h>
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <ctype.h>

# include "rtf.h"
# include "rtfprep/tokenscan.h"
# include "cole.h"
# include "rtf2latex2e.h"
# include "eqn.h"
void __cole_dump(void *_m, void *_start, uint32_t length, char *msg);

char outputMapName[255];
# define        prefFileName    "r2l-pref"
# define        MAX_BLANK_LINES       2
# define        MATH_NONE_MODE        0
# define        MATH_INLINE_MODE      1
# define        MATH_DISPLAY_MODE     2 

char texMapQualifier[rtfBufSiz];

void RTFSetOutputStream(FILE * stream);
static void ReadObject(void);

int groupLevel = 0;            /* a counter for keeping track of opening and closing braces */
extern FILE *ifp, *ofp;

# define EQUATION_OFFSET 35
# define MTEF_HEADER_SIZE 28
# define RESTORE_LEVELS 1
# define SAVE_LEVELS    0
# define        NumberOfPreferences 16

const char *preferenceList[] = { 
    "outputMapFile",
    "ignoreRulerSettings",
    "paperWidth",
    "leftMargin",
    "rightMargin",
    "ignoreColor",
    "ignoreParagraphAlignment",
    "ignoreTextStyle",
    "ignoreHypertext",
    "ignoreSpacing",
    "convertEquations",
    "pict2eps_translate",
    "pict2eps_fonts",
    "pict2eps_preview",
    "fileCreator",
    "swpMode"
};

const char *objectClassList[] = { 
    "Unknown",
    "Equation",
    "Word.Picture",
    "MSGraph.Chart",
    (char *) NULL
};

const char *justificationList[] = {
    "\\raggedright", 
    "\\centering", 
    "\\raggedleft" 
};

const char *environmentList[] = { 
    "flushleft", 
    "center", 
    "flushright" 
};

const char *fontSizeList[] = { 
    "\\tiny",
    "\\scriptsize",
    "\\footnotesize",
    "\\small",
    "\\normalsize",
    "\\large",
    "\\Large",
    "\\LARGE",
    "\\huge",
    "\\Huge"
};

const char *r2lList[] = { 
    "documentclass",
    "bold",
    "nobold",
    "italic",
    "noitalic",
    "underline",
    "smallcaps",
    "heading1",
    "heading2",
    "heading3",
    "table"
};

static struct pageStyleStruct {
    double width;
    double leftMargin;
    double rightMargin;
} page;

static struct {
    boolean newStyle;
    int alignment;
    boolean wroteAlignment;
    int oldSpacing;
    int lineSpacing;
    boolean wroteSpacing;
    int firstIndent;
    int leftIndent;
    int rightIndent;
    boolean parbox;
} paragraph;

static struct {
    boolean newStyle;
    int cols;
} section;

static struct {
    int class;
    char className[rtfBufSiz];
    int word97;
} object;

float preferenceValue[NumberOfPreferences];

/*
 * Flags global to LaTeX2e-writer.c
 */
static int wrapCount = 0;
static int charAttrCount = 0;
static int mathMode = 0;
static int word97ObjectType;
static boolean lastCharWasLineBreak = true;
static boolean seenLeftDoubleQuotes = false;
static boolean wroteBeginDocument = false;
static int spaceCount = 0;
static int blankLineCount = 0;
static boolean suppressLineBreak;
static boolean requireColorPackage;
static boolean requireSetspacePackage;
static boolean requireTablePackage;
static boolean requireMultirowPackage;
static boolean requireGraphicxPackage;
static boolean requireAmsSymbPackage;
static boolean requireMultiColPackage;
static boolean requireUlemPackage;
static boolean requireHyperrefPackage;
static boolean requireAmsMathPackage;
static boolean requireUnicodePackage;
static size_t packagePos;
static boolean writingHeading1, writingHeading2, writingHeading3;
static boolean insideFootnote, justWroteFootnote;
static boolean insideHyperlink;

/*
static int mathGroup = 0;
static int colorChange = 0;
static boolean requireFontEncPackage;
*/

struct EQN_OLE_FILE_HDR {
    uint16_t   cbHdr;     /* length of header, sizeof(EQNOLEFILEHDR) = 28 bytes */
    uint32_t   version;   /* hiword = 2, loword = 0 */
    uint16_t   format;
    uint32_t   size;
    uint32_t   reserved1; 
    uint32_t   reserved2; 
    uint32_t   reserved3; 
    uint32_t   reserved4; 
};

static int codePage;
char fileCreator[10];


static char *outMap[rtfSC_MaxChar];

#define NumberOfR2LMappings     11
static char *r2lMap[NumberOfR2LMappings];
static boolean r2lMapPresent = true;

static char *boldString;
static char *noBoldString;
static char *italicString;
static char *noItalicString;
static char *underlineString;
static char *smallcapsString;
static char *documentclassString;
static char *heading1String;
static char *heading2String;
static char *heading3String;
static char *tableString;
static short itemNumber;

static FILE *ostream;
static textStyleStruct textStyle;
static pictureStruct picture;
static equationStruct oleEquation;
static tableStruct table;

static boolean dblQuoteLeft = false;
static boolean wroteCellHeader = false;
static boolean startFootnoteText = false;
static boolean continueTextStyle = false;
static boolean lineIsBlank = true;

static short R2LItem(char *name)
{
    short i;

    for (i = 0; i < NumberOfR2LMappings; i++) {
        if (strcmp(name, r2lList[i]) == 0)
            return (i);
    }
    printf("R2LItem: invalid preference %s!\n", name);
    return (-1);
}

static short ReadR2LMap(void)
{
    FILE *f;
    char buf[rtfBufSiz];
    char *name, *seq;
    short stdCode;
    short i;
    TSScanner scanner;
    char *scanEscape;
    char *fn = "ReadR2LMap";

    /* clobber current mapping */
    for (i = 0; i < NumberOfR2LMappings; i++) {
        RTFFree(r2lMap[i]);
        r2lMap[i] = (char *) NULL;
    }


    if ((f = RTFOpenLibFile("r2l-map", "r")) == (FILE *) NULL)
        return (0);

    /*
     * Turn off scanner's backslash escape mechanism while reading
     * file.  Restore it later.
     */
    TSGetScanner(&scanner);
    scanEscape = scanner.scanEscape;
    scanner.scanEscape = "";
    TSSetScanner(&scanner);

    /* read file */

    while (fgets(buf, (int) sizeof(buf), f) != (char *) NULL) {
        if (buf[0] == '#')      /* skip comment lines */
            continue;

        TSScanInit(buf);

        if ((name = TSScan()) == (char *) NULL)
            continue;           /* skip blank lines */

        if ((stdCode = R2LItem(name)) < 0) {
            RTFMsg("%s: unknown preference: %s\n", fn, name);
            continue;
        }

        if ((seq = TSScan()) == (char *) NULL) {
            RTFMsg("%s: malformed output sequence line for preference %s\n", fn, name);
            continue;
        }

        if ((seq = RTFStrSave(seq)) == (char *) NULL)
            RTFPanic("%s: out of memory", fn);

        r2lMap[stdCode] = seq;
    }
    scanner.scanEscape = scanEscape;
    TSSetScanner(&scanner);
    fclose(f);
/*      
        for (i = 0; i < NumberOfR2LMappings; i++)
                printf ("%s\n", r2lMap[i]);
*/
    return (1);
}


/*
 * Initialize the writer. Basically reads the output map.
 */
void WriterInit(void)
{
    /* read character map files */
    if (RTFReadCharSetMap("cp1252.map", rtfCS1252) == 0)
        RTFPanic("Cannot read character set map file cp1252.map for code page 1252!\n");
        
    if (RTFReadCharSetMap("cp1250.map", rtfCS1250) == 0)
        RTFPanic("Cannot read character set map file cp1250.map for code page 1250!\n");
        
    if (RTFReadCharSetMap("cp1254.map", rtfCS1254) == 0)
        RTFPanic("Cannot read character set map file cp1254.map for code page 1254!\n");
        
    if (RTFReadCharSetMap("applemac.map", rtfCSMac) == 0)
        RTFPanic("Cannot read character set map file applemac.map!\n");
        
    if (RTFReadCharSetMap("cp437.map", rtfCS437) == 0)
        RTFPanic("Cannot read character set map file cp437.map for code page 437!\n");
        
    if (RTFReadCharSetMap("cp850.map", rtfCS850) == 0)
        RTFPanic("Cannot read character set map file cp850.map for code page 850!\n");

    /* read preferences */
    if (ReadPrefFile(prefFileName) == 0)
        RTFPanic("Cannot read preferences file %s", prefFileName);

    /* read output map file TeX-map */
    strcpy(texMapQualifier, "");

    /* if a TeX-map file was not specified, set it to the default */
    if (strcmp(outputMapName, "") == 0)
        strcpy(outputMapName, "TeX-map");

    if (RTFReadOutputMap(outputMapName, outMap, 1) == 0)
        RTFPanic("Cannot read output map %s", outputMapName);

    /* read r2l mapping file if present */
    if (ReadR2LMap() == 0)
        r2lMapPresent = false;
}

static boolean IsValidPref(char *name, char *pref)
{
    short i;

    if (!strcmp(name, "fileCreator") || !strcmp(name, "outputMapFile"))
        return true;

    if (strcmp(pref, "true") == 0 || strcmp(pref, "false") == 0)
        return true;

    for (i = 0; i < strlen(pref); i++)
        if (!(isdigit(pref[i]) || pref[i] == '.')) return false;

    return true;
}

static int GetPreferenceNum(char *name)
{
    short i;

    for (i = 0; i < NumberOfPreferences; i++)
        if (strcmp(name, preferenceList[i]) == 0) return (i);

    printf("GetPreferenceNum: invalid preference %s!\n", name);

    return (-1);
}

short ReadPrefFile(char *file)
{
    char *fn = "ReadPrefFile";
    FILE *f;
    char buf[rtfBufSiz];
    char *name, *seq;
    TSScanner scanner;
    char *scanEscape;
    int whichPref;
    short i;

    /* initialize preferences */
    for (i = 0; i < NumberOfPreferences; i++)
        preferenceValue[i] = 0;

    if ((f = RTFOpenLibFile(file, "r")) == (FILE *) NULL) {
        RTFMsg("can't open file %s\n", file);
        return (0);
    }

    /*
     * Turn off scanner's backslash escape mechanism while reading
     * file.  Restore it later.
     */
    TSGetScanner(&scanner);
    scanEscape = scanner.scanEscape;
    scanner.scanEscape = "";
    TSSetScanner(&scanner);

    /* read file */

    while (fgets(buf, (int) sizeof(buf), f) != (char *) NULL) {
        if (buf[0] == '#')      /* skip comment lines */
            continue;
            
        TSScanInit(buf);
        
        if ((name = TSScan()) == (char *) NULL)
            continue;           /* skip blank lines */
            
        if ((whichPref = GetPreferenceNum(name)) < 0) {
            RTFMsg("%s: unkown preference: %s\n", fn, name);
            continue;
        }
        
        if ((seq = TSScan()) == (char *) NULL || !IsValidPref(name, seq)) {
            RTFMsg("%s: malformed preference setting for %s\n", fn, name);
            continue;
        }

        if (strcmp(name, "fileCreator") == 0) {
            strcpy(fileCreator, seq);
            strcpy(seq, "true");
        }

        if (strcmp(name, "outputMapFile") == 0) {
            if (!strcmp(outputMapName, ""))
                strcpy(outputMapName, seq);
            strcpy(seq, "true");
        }

        if (strcmp(seq, "true") == 0)
            strcpy(seq, "1");
        else if (strcmp(seq, "false") == 0)
            strcpy(seq, "0");

        preferenceValue[whichPref] = (int) atof(seq);
    }
    scanner.scanEscape = scanEscape;
    TSSetScanner(&scanner);
    if (fclose(f) != 0)
        printf("¥ error closing pref file %s\n", file);

    /* set the preferences here to values in file or to default */
    if ((page.width = preferenceValue[GetPreferenceNum("paperWidth")]) == 0)
        page.width = 8.5;
        
    if ((page.leftMargin = preferenceValue[GetPreferenceNum("leftMargin")]) == 0)
        page.leftMargin = 1.0;
        
    if ((page.rightMargin = preferenceValue[GetPreferenceNum("rightMargin")]) == 0)
        page.rightMargin = 1.0;

    return (1);

}

static void PutLitChar(int c)
{
    fputc(c, ostream);
}

static void PutLitStr(char *s)
{
    fputs(s, ostream);
}

static void InsertNewLine(void)
{
    PutLitStr("\n");
    lineIsBlank = true;
}

static void EnsureInlineMathMode(void)
{
    switch (mathMode) {
    case MATH_INLINE_MODE:
        break;
        
    case MATH_DISPLAY_MODE:
        break;
   
    case MATH_NONE_MODE:
        PutLitStr("$ ");
        wrapCount+=2;
        mathMode = MATH_INLINE_MODE;
    }
}

static void EnsureNoMathMode(void)
{
    switch (mathMode) {
    case MATH_INLINE_MODE:
        PutLitStr(" $ ");
        wrapCount+=3;
        mathMode = MATH_NONE_MODE;
        break;
        
    case MATH_DISPLAY_MODE:
        PutLitStr("$$\n");
        wrapCount=0;
        mathMode = MATH_NONE_MODE;
        break;
   
    case MATH_NONE_MODE:
        break;
    }
}


/* 
 * This function reads colors from the color table and defines them in 
 * LaTeX format to be included in the 
 * LaTeX preamble. This is done after the color table has been read (see above).
*/
static void DefineColors(void)
{
    RTFColor *rtfColorPtr;
    int i = 1;
    float textRed, textBlue, textGreen;
    char buf[rtfBufSiz];

    while ((rtfColorPtr = RTFGetColor(i)) != (RTFColor *) NULL) {
        textRed = (float) ((rtfColorPtr->rtfCRed) / 255.0);
        textGreen = (float) ((rtfColorPtr->rtfCGreen) / 255.0);
        textBlue = (float) ((rtfColorPtr->rtfCBlue) / 255.0);

        snprintf(buf, rtfBufSiz, "\\definecolor{color%d}{rgb}{%1.3f,%1.3f,%1.3f}\n",
                i, textRed, textGreen, textBlue);
        PutLitStr(buf);
        wrapCount = 0;

        i++;
    }
}


/* 
 * a useful diagnostic function to examine the token just read.
 */
void ExamineToken(void)
{
    printf("* Token is %s\n", rtfTextBuf);
    printf("* Class is %d\n", rtfClass);
    printf("* Major is %d\n", (int) rtfMajor);
    printf("* Minor is %d\n", rtfMinor);
    printf("* Param is %d\n\n", rtfParam);
}


static uint32_t CountCharInString(char *theString, char theChar)
{
    uint32_t i, count, length;
    
    count = 0;
    length = (uint32_t) strlen(theString);

    for (i = 0; i < length; i++)
        if (theString[i] == theChar)
            count++;

    return (count);
}

/*
 * Eventually this should keep track of the destination of the
 * current state and only write text when in the initial state.
 *
 * If the output sequence is unspecified in the output map, write
 * the character's standard name instead.  This makes map deficiencies
 * obvious and provides incentive to fix it. :-)
 */

static void PutStdChar(stdCode)
int stdCode;
{
    char *oStr = (char *) NULL;
    char buf[rtfBufSiz];

    if (stdCode == rtfSC_nothing) {
        RTFMsg
            ("* Warning: I don't know this character %c (0x%x) in character set %d!\n",
             rtfTextBuf[0], rtfTextBuf[0], RTFGetCharSet());
        ExamineToken();
        PutLitStr("(unknown char)");
        return;
    }
    oStr = outMap[stdCode];
    if (oStr == (char *) NULL) {        /* no output sequence in map */
        snprintf(buf, rtfBufSiz, "(%s)", RTFStdCharName(stdCode));
        oStr = buf;
    }
    PutLitStr(oStr);
    wrapCount += (int) strlen(oStr);
}

/* unused */
/*
static void WriteColor (void)
{
char buf[rtfBufSiz];


        snprintf(buf, rtfBufSiz, "{\\color{color%ld} ", (int)rtfParam);
        PutLitStr (buf);        
        wrapCount += 17;

}
*/

/* 
 * make sure we write this all important stuff. This routine is called 
 * whenever something is written to the output file.
 */
static void CheckForBeginDocument(void)
{
    char buf[100];

    if (!wroteBeginDocument) {
        if (!preferenceValue[GetPreferenceNum("ignoreRulerSettings")]) {
            snprintf(buf, 100, "\\setlength{\\oddsidemargin}{%3.2fin}\n", 1 - page.leftMargin);
            PutLitStr(buf);
            snprintf(buf, 100, "\\setlength{\\evensidemargin}{%3.2fin}\n", 1 - page.rightMargin);
            PutLitStr(buf);
            snprintf(buf, 100, "\\setlength{\\textwidth}{%3.2fin}\n", page.width - page.leftMargin - page.rightMargin);
            PutLitStr(buf);
            blankLineCount++;
        }
        if (blankLineCount < MAX_BLANK_LINES) {
            InsertNewLine();
            blankLineCount++;
        }

        PutLitStr("\\begin{document}\n\n");
        blankLineCount++;
    }

    wroteBeginDocument = true;
}


static void InitializeGroupLevels(void)
{
    textStyle.boldGroupLevel = 0;
    textStyle.noBoldGroupLevel = 0;
    textStyle.italicGroupLevel = 0;
    textStyle.noItalicGroupLevel = 0;
    textStyle.underlinedGroupLevel = 0;
    textStyle.noUnderlinedGroupLevel = 0;
    textStyle.dbUnderlinedGroupLevel = 0;
    textStyle.noDbUnderlinedGroupLevel = 0;
    textStyle.foreColorGroupLevel = 0;
    textStyle.backColorGroupLevel = 0;
    textStyle.subScriptGroupLevel = 0;
    textStyle.noSubScriptGroupLevel = 0;
    textStyle.superScriptGroupLevel = 0;
    textStyle.noSuperScriptGroupLevel = 0;
    textStyle.fontSizeGroupLevel = 0;
    textStyle.allCapsGroupLevel = 0;
    textStyle.smallCapsGroupLevel = 0;
    textStyle.open = false;
}

static void SetGroupLevels(int flag)
{
    static textStyleStruct restoreStyle;

    if (flag == SAVE_LEVELS) {
        restoreStyle = textStyle;
        return;
    }

    textStyle.boldGroupLevel = restoreStyle.boldGroupLevel;
    textStyle.italicGroupLevel = restoreStyle.italicGroupLevel;
    textStyle.underlinedGroupLevel = restoreStyle.underlinedGroupLevel;
    textStyle.foreColorGroupLevel = restoreStyle.foreColorGroupLevel;
    textStyle.backColorGroupLevel = restoreStyle.backColorGroupLevel;
    textStyle.subScriptGroupLevel = restoreStyle.subScriptGroupLevel;
    textStyle.superScriptGroupLevel = restoreStyle.superScriptGroupLevel;
    textStyle.fontSizeGroupLevel = restoreStyle.fontSizeGroupLevel;
    textStyle.noBoldGroupLevel = restoreStyle.noBoldGroupLevel;
    textStyle.noItalicGroupLevel = restoreStyle.noItalicGroupLevel;
    textStyle.fontSize = restoreStyle.fontSize;
}

/*
 * This function initializes the text style. Pretty much self-explanatory.
 */
static void InitializeTextStyle(void)
{
    textStyle.foreColor = -1;
    textStyle.backColor = -1;
    textStyle.fontSize = normalSize;
    textStyle.newStyle = false;
    textStyle.wroteBold = false;
    textStyle.wroteNoBold = false;
    textStyle.wroteItalic = false;
    textStyle.wroteNoItalic = false;
    textStyle.wroteUnderlined = false;
    textStyle.wroteDbUnderlined = false;
    textStyle.wroteShadowed = false;
    textStyle.wroteAllcaps = false;
    textStyle.wroteSmallCaps = false;
    textStyle.wroteForeColor = false;
    textStyle.wroteFontSize = false;
    textStyle.wroteSubScript = false;
    textStyle.wroteNoSubScript = false;
    textStyle.wroteSuperScript = false;
    textStyle.wroteNoSuperScript = false;

    paragraph.firstIndent = 0;
    paragraph.leftIndent = 0;
    paragraph.rightIndent = 0;

    InitializeGroupLevels();
}

/*
 * This function makes sure any braces opened for text styles are closed;
 * rather messy, but it will do for now.
 */
void CheckForCharAttr(void)
{
    int italicGL, noItalicGL;
    int boldGL, noBoldGL;
    int underlinedGL;
/*  int noUnderlinedGL; */
    int dbUnderlinedGL;
/*  int noDbUnderlinedGL; */
    int foreColorGL, backColorGL;
    int subScriptGL, superScriptGL;
/*     int noSubScriptGL, noSuperScriptGL; */
    int fontSizeGL;
/*  int allCapsGL; */
    int smallCapsGL;
    int i;

    italicGL         = textStyle.italicGroupLevel;
    noItalicGL       = textStyle.noItalicGroupLevel;
    boldGL           = textStyle.boldGroupLevel;
    noBoldGL         = textStyle.noBoldGroupLevel;
    underlinedGL     = textStyle.underlinedGroupLevel;
/*     noUnderlinedGL   = textStyle.noUnderlinedGroupLevel; */
    dbUnderlinedGL   = textStyle.dbUnderlinedGroupLevel;
/*     noDbUnderlinedGL = textStyle.noDbUnderlinedGroupLevel; */
    foreColorGL      = textStyle.foreColorGroupLevel;
    backColorGL      = textStyle.backColorGroupLevel;
    subScriptGL      = textStyle.subScriptGroupLevel;
/*     noSubScriptGL    = textStyle.noSubScriptGroupLevel; */
    superScriptGL    = textStyle.superScriptGroupLevel;
/*     noSuperScriptGL  = textStyle.noSuperScriptGroupLevel; */
    fontSizeGL       = textStyle.fontSizeGroupLevel;
 /*    allCapsGL        = textStyle.allCapsGroupLevel; */
    smallCapsGL      = textStyle.smallCapsGroupLevel;

    if (smallCapsGL == groupLevel && smallCapsGL > 0) {
        if (charAttrCount > 0 && textStyle.wroteSmallCaps) {
            for (i = 0; i < CountCharInString(smallcapsString, '{'); i++)
                PutLitStr("}");
            charAttrCount -= CountCharInString(smallcapsString, '{');
        }
        textStyle.smallCapsGroupLevel = 0;
        textStyle.wroteSmallCaps = false;
    }
    if (underlinedGL == groupLevel && underlinedGL > 0) {
        if (charAttrCount > 0 && textStyle.wroteUnderlined) {
            for (i = 0; i < CountCharInString(underlineString, '{'); i++)
                PutLitStr("}");
            charAttrCount -= CountCharInString(underlineString, '{');
        }
        textStyle.underlinedGroupLevel = 0;
        textStyle.wroteUnderlined = false;
    }
    if (dbUnderlinedGL == groupLevel && dbUnderlinedGL > 0) {
        if (charAttrCount > 0 && textStyle.wroteDbUnderlined) {
            PutLitStr("}}");
            charAttrCount -= 2;
        }
        textStyle.dbUnderlinedGroupLevel = 0;
        textStyle.wroteDbUnderlined = false;
    }
    if (noItalicGL == groupLevel && noItalicGL > 0) {
        if (charAttrCount > 0 && textStyle.wroteNoItalic) {
            for (i = 0; i < CountCharInString(noItalicString, '{'); i++)
                PutLitStr("}");
            charAttrCount -= CountCharInString(noItalicString, '{');
        }
        textStyle.noItalicGroupLevel = 0;
        textStyle.wroteNoItalic = false;
    }
    if (italicGL == groupLevel && italicGL > 0) {
        if (charAttrCount > 0 && textStyle.wroteItalic) {
            for (i = 0; i < CountCharInString(italicString, '{'); i++)
                PutLitStr("}");
            charAttrCount -= CountCharInString(italicString, '{');
        }
        textStyle.italicGroupLevel = 0;
        textStyle.wroteItalic = false;
    }
    if (noBoldGL == groupLevel && noBoldGL > 0) {
        if (charAttrCount > 0 && textStyle.wroteNoBold) {
            for (i = 0; i < CountCharInString(noBoldString, '{'); i++)
                PutLitStr("}");
            charAttrCount -= CountCharInString(noBoldString, '{');
        }
        textStyle.noBoldGroupLevel = 0;
        textStyle.wroteNoBold = false;
    }
    if (boldGL == groupLevel && boldGL > 0) {
        if (charAttrCount > 0 && textStyle.wroteBold) {
            for (i = 0; i < CountCharInString(boldString, '{'); i++)
                PutLitStr("}");
            charAttrCount -= CountCharInString(boldString, '{');
        }
        textStyle.boldGroupLevel = 0;
        textStyle.wroteBold = false;
    }
    if (superScriptGL == groupLevel && superScriptGL > 0) {
        if (boldGL < superScriptGL && boldGL > 0 && textStyle.wroteBold) {
            PutLitStr("}");
            charAttrCount--;
            textStyle.newStyle = true;
            textStyle.wroteBold = false;
        }
        if (italicGL < superScriptGL && italicGL > 0
            && textStyle.wroteItalic) {
            PutLitStr("}");
            charAttrCount--;
            textStyle.newStyle = true;
            textStyle.wroteItalic = false;
        }
        if (charAttrCount > 0) {
            PutLitStr("}");
            charAttrCount -= 1;
            EnsureNoMathMode();
        }
        textStyle.superScriptGroupLevel = 0;
        textStyle.wroteSuperScript = false;
    }
    if (subScriptGL == groupLevel && subScriptGL > 0) {
        if (boldGL < superScriptGL && boldGL > 0 && textStyle.wroteBold) {
            PutLitStr("}");
            charAttrCount--;
            textStyle.newStyle = true;
            textStyle.wroteBold = false;
        }
        if (italicGL < superScriptGL && italicGL > 0
            && textStyle.wroteItalic) {
            PutLitStr("}");
            charAttrCount--;
            textStyle.newStyle = true;
            textStyle.wroteItalic = false;
        }
        if (charAttrCount > 0) {
            PutLitStr("}");
            charAttrCount -= 1;
            EnsureNoMathMode();
        }
        textStyle.subScriptGroupLevel = 0;
        textStyle.wroteSubScript = false;
    }
    if (foreColorGL == groupLevel && foreColorGL > 0) {
        if (charAttrCount > 0 && textStyle.wroteForeColor) {
            PutLitStr("}");
            charAttrCount--;
        }
        textStyle.foreColorGroupLevel = 0;
        textStyle.wroteForeColor = false;
    }
    if (backColorGL == groupLevel && backColorGL > 0) {
        if (charAttrCount > 0 && textStyle.wroteBackColor) {
            PutLitStr("}");
            charAttrCount--;
        }
        textStyle.backColorGroupLevel = 0;
        textStyle.wroteBackColor = false;
    }
    if (fontSizeGL == groupLevel && fontSizeGL > 0) {
        if (charAttrCount > 0 && textStyle.wroteFontSize) {
            PutLitStr("}");
            charAttrCount--;
        }
        textStyle.fontSizeGroupLevel = 0;
        textStyle.fontSize = normalSize;
        textStyle.wroteFontSize = false;
    }
    if (charAttrCount == 0)
        textStyle.open = false;

}

static void ForceCloseCharAttr(void)
{
    int i;
    if (charAttrCount > 0 && textStyle.wroteSmallCaps) {
        for (i = 0; i < CountCharInString(smallcapsString, '{'); i++)
            PutLitStr("}");
        charAttrCount -= CountCharInString(smallcapsString, '{');
    }
    textStyle.smallCapsGroupLevel = 0;
    textStyle.wroteSmallCaps = false;

    if (charAttrCount > 0 && textStyle.wroteUnderlined) {
        for (i = 0; i < CountCharInString(underlineString, '{'); i++)
            PutLitStr("}");
        charAttrCount -= CountCharInString(underlineString, '{');
    }
    textStyle.underlinedGroupLevel = 0;
    textStyle.wroteUnderlined = false;

    if (charAttrCount > 0 && textStyle.wroteNoItalic) {
        for (i = 0; i < CountCharInString(noItalicString, '{'); i++)
            PutLitStr("}");
        charAttrCount -= CountCharInString(noItalicString, '{');
    }
    textStyle.noItalicGroupLevel = 0;
    textStyle.wroteNoItalic = false;

    if (charAttrCount > 0 && textStyle.wroteItalic) {
        for (i = 0; i < CountCharInString(italicString, '{'); i++)
            PutLitStr("}");
        charAttrCount -= CountCharInString(italicString, '{');
    }
    textStyle.italicGroupLevel = 0;
    textStyle.wroteItalic = false;

    if (charAttrCount > 0 && textStyle.wroteBold) {
        for (i = 0; i < CountCharInString(boldString, '{'); i++)
            PutLitStr("}");
        charAttrCount -= CountCharInString(boldString, '{');
    }
    textStyle.boldGroupLevel = 0;
    textStyle.wroteBold = false;

    if (charAttrCount > 0 && textStyle.wroteSuperScript) {
        PutLitStr("}");
        charAttrCount -= 1;
        EnsureNoMathMode();
        textStyle.superScriptGroupLevel = 0;
        textStyle.wroteSuperScript = false;
    }

    if (charAttrCount > 0 && textStyle.wroteSubScript) {
        PutLitStr("}");
        charAttrCount -= 1;
        EnsureNoMathMode();
        textStyle.subScriptGroupLevel = 0;
        textStyle.wroteSubScript = false;
    }

    if (charAttrCount > 0 && textStyle.wroteForeColor) {
        PutLitStr("}");
        charAttrCount--;
    }
    textStyle.foreColorGroupLevel = 0;
    textStyle.wroteForeColor = false;

    if (charAttrCount > 0 && textStyle.wroteBackColor) {
        PutLitStr("}");
        charAttrCount--;
    }
    textStyle.backColorGroupLevel = 0;
    textStyle.wroteBackColor = false;

    if (charAttrCount > 0 && textStyle.wroteFontSize) {
        PutLitStr("}");
        charAttrCount--;
    }
    textStyle.fontSizeGroupLevel = 0;
    textStyle.fontSize = normalSize;
    textStyle.wroteFontSize = false;

    for (i = 0; i < charAttrCount; i++)
        PutLitStr("}");
    charAttrCount = 0;
}

/*
 * This function stores the text style.
 */
static void SetTextStyle(void)
{
    if (insideHyperlink)
        return;

    switch (rtfMinor) {
    case rtfSmallCaps:
        textStyle.smallCapsGroupLevel = groupLevel;
        break;
    case rtfAllCaps:
        textStyle.allCapsGroupLevel = groupLevel;
        break;
    case rtfItalic:
        if (rtfParam != 0) {
            textStyle.italicGroupLevel = groupLevel;
            if (textStyle.noItalicGroupLevel == groupLevel)
                textStyle.noItalicGroupLevel = 0;
        } else if (textStyle.wroteItalic) {
            textStyle.noItalicGroupLevel = groupLevel;
            if (textStyle.italicGroupLevel == groupLevel) {
                SetGroupLevels(SAVE_LEVELS);
                CheckForCharAttr();
                SetGroupLevels(RESTORE_LEVELS);
                textStyle.italicGroupLevel = 0;
            }
        }
        break;
    case rtfBold:
        if (rtfParam != 0) {
            textStyle.boldGroupLevel = groupLevel;
            if (textStyle.noBoldGroupLevel == groupLevel)
                textStyle.noBoldGroupLevel = 0;
        } else if (textStyle.wroteBold) {
            textStyle.noBoldGroupLevel = groupLevel;
            if (textStyle.boldGroupLevel == groupLevel) {
                SetGroupLevels(SAVE_LEVELS);
                CheckForCharAttr();
                SetGroupLevels(RESTORE_LEVELS);
                textStyle.boldGroupLevel = 0;
            }
        }
        break;
    case rtfUnderline:
        if (rtfParam != 0) {
            textStyle.underlinedGroupLevel = groupLevel;
            if (textStyle.noUnderlinedGroupLevel == groupLevel)
                textStyle.noUnderlinedGroupLevel = 0;
        } else if (textStyle.wroteUnderlined) {
            textStyle.noUnderlinedGroupLevel = groupLevel;
            if (textStyle.underlinedGroupLevel == groupLevel) {
                SetGroupLevels(SAVE_LEVELS);
                CheckForCharAttr();
                SetGroupLevels(RESTORE_LEVELS);
                textStyle.underlinedGroupLevel = 0;
            }
        }
        break;
    case rtfDbUnderline:
        if (rtfParam != 0) {
            textStyle.dbUnderlinedGroupLevel = groupLevel;
            if (textStyle.noDbUnderlinedGroupLevel == groupLevel)
                textStyle.noDbUnderlinedGroupLevel = 0;
        } else if (textStyle.wroteDbUnderlined) {
            textStyle.noDbUnderlinedGroupLevel = groupLevel;
            if (textStyle.dbUnderlinedGroupLevel == groupLevel) {
                SetGroupLevels(SAVE_LEVELS);
                CheckForCharAttr();
                SetGroupLevels(RESTORE_LEVELS);
                textStyle.dbUnderlinedGroupLevel = 0;
            }
        }
        break;
    case rtfForeColor:
        textStyle.foreColor = rtfParam;
        textStyle.foreColorGroupLevel = groupLevel;
        textStyle.foreColor = rtfParam;
        break;
    case rtfSubScrShrink:
    case rtfSubScript:
        if (rtfParam != 0) {
            textStyle.subScriptGroupLevel = groupLevel;
            if (textStyle.noSubScriptGroupLevel == groupLevel)
                textStyle.noSubScriptGroupLevel = 0;
        } else if (textStyle.wroteSubScript) {
            textStyle.noSubScriptGroupLevel = groupLevel;
            if (textStyle.subScriptGroupLevel == groupLevel) {
                SetGroupLevels(SAVE_LEVELS);
                CheckForCharAttr();
                SetGroupLevels(RESTORE_LEVELS);
                textStyle.subScriptGroupLevel = 0;
            }
        }
/*                       textStyle.subScriptGroupLevel = groupLevel; */
        break;
    case rtfSuperScript:
        if (rtfParam != 0) {
            textStyle.superScriptGroupLevel = groupLevel;
            if (textStyle.noSuperScriptGroupLevel == groupLevel)
                textStyle.noSuperScriptGroupLevel = 0;
        } else if (textStyle.wroteSuperScript) {
            textStyle.noSuperScriptGroupLevel = groupLevel;
            if (textStyle.superScriptGroupLevel == groupLevel) {
                SetGroupLevels(SAVE_LEVELS);
                CheckForCharAttr();
                SetGroupLevels(RESTORE_LEVELS);
                textStyle.superScriptGroupLevel = 0;
            }
        }
        break;
    case rtfSuperScrShrink:
        RTFGetToken();
        if (strcmp(rtfTextBuf, "\\chftn") != 0
            && !RTFCheckCM(rtfGroup, rtfEndGroup))
            textStyle.superScriptGroupLevel = groupLevel;
        RTFUngetToken();
        break;
    case rtfFontSize:
        if (rtfParam <= 18) {
            textStyle.fontSize = smallSize;
            textStyle.fontSizeGroupLevel = groupLevel;
        }
        if (rtfParam <= 14) {
            textStyle.fontSize = footNoteSize;
            textStyle.fontSizeGroupLevel = groupLevel;
        }
        if (rtfParam <= 12) {
            textStyle.fontSize = scriptSize;
            textStyle.fontSizeGroupLevel = groupLevel;
        }
        if (rtfParam >= 28) {
            textStyle.fontSize = largeSize;
            textStyle.fontSizeGroupLevel = groupLevel;
        }
        if (rtfParam >= 32) {
            textStyle.fontSize = LargeSize;
            textStyle.fontSizeGroupLevel = groupLevel;
        }
        if (rtfParam >= 36) {
            textStyle.fontSize = LARGESize;
            textStyle.fontSizeGroupLevel = groupLevel;
        }
        if (rtfParam >= 48) {
            textStyle.fontSize = giganticSize;
            textStyle.fontSizeGroupLevel = groupLevel;
        }
        if (rtfParam >= 72) {
            textStyle.fontSize = GiganticSize;
            textStyle.fontSizeGroupLevel = groupLevel;
        }
        break;

    }
    textStyle.newStyle = true;
}

/*
 * This function writes the text style.
 */
static void WriteTextStyle(void)
{
    char buf[rtfBufSiz];
    int italicGL, noItalicGL;
    int boldGL, noBoldGL;
    int underlinedGL;
/*  int noUnderlinedGL; */
    int dbUnderlinedGL;
/*  int noDbUnderlinedGL; */
    int foreColorGL;
/*  int backColorGL; */
    int subScriptGL, superScriptGL;
/*     int fontSizeGL; */
    int smallCapsGL;
/*  int allCapsGL; */

    if (writingHeading1 || writingHeading2 || writingHeading3
        || insideHyperlink)
        return;

    italicGL = textStyle.italicGroupLevel;
    noItalicGL = textStyle.noItalicGroupLevel;
    boldGL = textStyle.boldGroupLevel;
    noBoldGL = textStyle.noBoldGroupLevel;
    underlinedGL = textStyle.underlinedGroupLevel;
/*     noUnderlinedGL = textStyle.noUnderlinedGroupLevel; */
    dbUnderlinedGL = textStyle.dbUnderlinedGroupLevel;
/*     noDbUnderlinedGL = textStyle.noDbUnderlinedGroupLevel; */
    foreColorGL = textStyle.foreColorGroupLevel;
/*     backColorGL = textStyle.backColorGroupLevel; */
    subScriptGL = textStyle.subScriptGroupLevel;
    superScriptGL = textStyle.superScriptGroupLevel;
/*     fontSizeGL = textStyle.fontSizeGroupLevel; */
/*     allCapsGL = textStyle.allCapsGroupLevel; */
    smallCapsGL = textStyle.smallCapsGroupLevel;

    CheckForBeginDocument();

    if (rtfClass == 2 && rtfMajor == 32)
        PutLitChar(' ');

    if (foreColorGL <= groupLevel && foreColorGL > 0
        && (textStyle.foreColor) > 0 && !(textStyle.wroteForeColor)) {

        snprintf(buf, rtfBufSiz, "{\\color{color%d} ", (int) (textStyle.foreColor));
        PutLitStr(buf);
        wrapCount += 17;
        charAttrCount++;
        textStyle.wroteForeColor = true;

    }
    
    if (subScriptGL <= groupLevel && subScriptGL > 0 && !(textStyle.wroteSubScript)) {
        if (boldGL <= superScriptGL && boldGL > 0 && textStyle.wroteBold) {
            PutLitStr("}");
            charAttrCount--;
            textStyle.wroteBold = false;
        }
        if (italicGL <= superScriptGL && italicGL > 0
            && textStyle.wroteItalic) {
            PutLitStr("}");
            charAttrCount--;
            textStyle.wroteItalic = false;
        }
        EnsureInlineMathMode();
        PutLitStr("_{");
        wrapCount += 2;
        charAttrCount += 1;
        textStyle.wroteSubScript = true;
        textStyle.open = true;
    }
    
    if (superScriptGL <= groupLevel && superScriptGL > 0 &&
        !(textStyle.wroteSuperScript)) {
        if (boldGL <= superScriptGL && boldGL > 0 && textStyle.wroteBold) {
            PutLitStr("}");
            charAttrCount--;
            textStyle.wroteBold = false;
        }
        if (italicGL <= superScriptGL && italicGL > 0
            && textStyle.wroteItalic) {
            PutLitStr("}");
            charAttrCount--;
            textStyle.wroteItalic = false;
        }
        EnsureInlineMathMode();
        PutLitStr("^{");
        wrapCount += 2;
        charAttrCount += 1;
        textStyle.wroteSuperScript = true;
        textStyle.open = true;
    }
    
    if (boldGL <= groupLevel && boldGL > 0 && !(textStyle.wroteBold)) {
        if (mathMode == MATH_NONE_MODE) {
            PutLitStr(boldString);
            wrapCount += (int) strlen(boldString);
            charAttrCount += CountCharInString(boldString, '{');
        } else {
            PutLitStr("\\mathbf{");
            wrapCount += 8;
            charAttrCount++;
        }
        textStyle.wroteBold = true;
/*               textStyle.open = true; */
    }
    
    if (noBoldGL <= groupLevel && noBoldGL > 0 && !(textStyle.wroteNoBold)
        && textStyle.wroteBold && mathMode == MATH_NONE_MODE) {
        PutLitStr(noBoldString);
        wrapCount += (int) strlen(noBoldString);
        charAttrCount += CountCharInString(noBoldString, '{');
        textStyle.wroteNoBold = true;
    }
    
    if (italicGL <= groupLevel && italicGL > 0 && !(textStyle.wroteItalic)) {
        if (mathMode==MATH_NONE_MODE) {
            PutLitStr(italicString);
            wrapCount += (int) strlen(italicString);
            charAttrCount += CountCharInString(italicString, '{');
        } else {
            PutLitStr("\\mathit{");
            wrapCount += 8;
            charAttrCount++;
        }
        textStyle.wroteItalic = true;
/*               textStyle.open = true; */
    }
    if (noItalicGL <= groupLevel && noItalicGL > 0
        && !(textStyle.wroteNoItalic)
        && textStyle.wroteItalic && mathMode==MATH_NONE_MODE) {
        PutLitStr(noItalicString);
        wrapCount += (int) strlen(noItalicString);
        charAttrCount += CountCharInString(noItalicString, '{');
        textStyle.wroteNoItalic = true;
    }
    if (underlinedGL <= groupLevel && underlinedGL > 0
        && !(textStyle.wroteUnderlined) && mathMode==MATH_NONE_MODE) {
        PutLitStr(underlineString);
        wrapCount += (int) strlen(underlineString);
        charAttrCount += CountCharInString(underlineString, '{');
        textStyle.wroteUnderlined = true;
        textStyle.open = true;

    }
    if (dbUnderlinedGL <= groupLevel && dbUnderlinedGL > 0
        && !(textStyle.wroteDbUnderlined) && mathMode==MATH_NONE_MODE) {
        PutLitStr("{\\uuline {");
        wrapCount += 12;
        charAttrCount += 2;
        textStyle.wroteDbUnderlined = true;
        textStyle.open = true;
        requireUlemPackage = true;
    }
    if (smallCapsGL <= groupLevel && smallCapsGL > 0
        && !(textStyle.wroteSmallCaps) && mathMode==MATH_NONE_MODE) {
        PutLitStr(smallcapsString);
        wrapCount += (int) strlen(smallcapsString);
        charAttrCount += CountCharInString(smallcapsString, '{');
        textStyle.wroteSmallCaps = true;
    }
    if (textStyle.fontSize != normalSize && mathMode==MATH_NONE_MODE
        && !(textStyle.wroteFontSize)) {
        snprintf(buf, rtfBufSiz, "{%s ", fontSizeList[textStyle.fontSize]);
        PutLitStr(buf);
        charAttrCount++;
        wrapCount += 7;
        textStyle.wroteFontSize = true;
    }

}

/*
 * This function writes the LaTeX header and includes some basic packages.
 */
static void WriteLaTeXHeader(void)
{
    FILE *f;
    char buf[rtfBufSiz];
    char *item;
    TSScanner scanner;
    char *scanEscape;
    int i, j;
    boolean preambleFilePresent = true;

    if ((f = RTFOpenLibFile("r2l-head", "r")) == (FILE *) NULL)
        preambleFilePresent = false;

    PutLitStr("%&LaTeX\n");
    PutLitStr("\\documentclass");
    PutLitStr(documentclassString);
    InsertNewLine();

    if (preambleFilePresent) {
        /*
         * Turn off scanner's backslash escape mechanism while reading
         * file.  Restore it later.
         */
        TSGetScanner(&scanner);
        scanEscape = scanner.scanEscape;
        scanner.scanEscape = "";
        TSSetScanner(&scanner);

        while (fgets(buf, (int) sizeof(buf), f) != (char *) NULL) {
            if (buf[0] == '#')  /* skip comment lines */
                continue;
            TSScanInit(buf);
            if ((item = TSScan()) == (char *) NULL)
                continue;       /* skip blank lines */

            PutLitStr(item);
            PutLitChar('\n');
        }

        scanner.scanEscape = scanEscape;
        TSSetScanner(&scanner);

        if (fclose(f) != 0)
            printf("¥ error closing preamble file\n");
    }

    /* insert TeX-map qualifier */
    if (strcmp(texMapQualifier, "") != 0)
        PutLitStr(texMapQualifier);

    /* to come back and write necessary \usepackage{...}
     * commands if necessary */
    packagePos = ftell(ofp);

    for (j = 0; j < PACKAGES + 1; j++) {
        for (i = 0; i < 100; i++)
            PutLitChar(' ');
        PutLitChar('\n');
    }

    PutLitStr("\\newcommand{\\tab}{\\hspace{5mm}}\n\n");
    blankLineCount++;
}

static void WriteColors(void)
{
    ReadColorTbl();
    if (requireColorPackage)
        DefineColors();
}

static void DoParagraphCleanUp(void)
{
    char buf[rtfBufSiz];
    int i;

    CheckForBeginDocument();
/*       ExamineToken(); */

    if (charAttrCount > 0)
        CheckForCharAttr();
    for (i = 0; i < charAttrCount; i++)
        PutLitChar('}');
    charAttrCount = 0;

    if (writingHeading1) {
        for (i = 0; i < CountCharInString(heading1String, '{'); i++)
            PutLitStr("}");
        InsertNewLine();
        wrapCount = 0;
        writingHeading1 = false;
        suppressLineBreak = true;
    }
    if (writingHeading2) {
        for (i = 0; i < CountCharInString(heading2String, '{'); i++)
            PutLitStr("}");
        InsertNewLine();
        wrapCount = 0;
        writingHeading2 = false;
        suppressLineBreak = true;
    }
    if (writingHeading3) {
        for (i = 0; i < CountCharInString(heading3String, '{'); i++)
            PutLitStr("}");
        InsertNewLine();
        wrapCount = 0;
        writingHeading3 = false;
        suppressLineBreak = true;
    }

    if (insideFootnote) {
        PutLitStr("}");
        InsertNewLine();
        wrapCount = 0;
        insideFootnote = false;
        suppressLineBreak = true;
    }

    if (paragraph.lineSpacing != paragraph.oldSpacing && paragraph.wroteSpacing) {
        PutLitStr("\\end{spacing}");
        InsertNewLine();
       /* paragraph.lineSpacing = singleSpace;*/
        paragraph.wroteSpacing = false;
        suppressLineBreak = true;
    }

    /* close previous environment */
    if (paragraph.alignment != left && paragraph.wroteAlignment) {
        snprintf(buf, rtfBufSiz, "\n\\end{%s}", environmentList[paragraph.alignment]);
        PutLitStr(buf);
        InsertNewLine();
        paragraph.alignment = left;
        paragraph.wroteAlignment = false;
        suppressLineBreak = true;
    }

    if (paragraph.parbox) {
        PutLitStr("}");
        InsertNewLine();
        wrapCount = 0;
        paragraph.parbox = false;
    }


    paragraph.firstIndent = 0;
    paragraph.leftIndent = 0;
    paragraph.rightIndent = 0;


}

static void DoSectionCleanUp(void)
{
    if (section.cols > 1) {
        PutLitStr("\n\\end{multicols}");
        InsertNewLine();
        section.cols = 1;
    }

    wrapCount = 0;

}

static void WriteLaTeXFooter(void)
{

    DoParagraphCleanUp();
    DoSectionCleanUp();

    PutLitStr("\n\n\\end{document}\n");
    fseek(ofp, packagePos, 0);

    /* load required packages */
    if (requireSetspacePackage)
        PutLitStr("\\usepackage{setspace}\n");
    if (requireColorPackage)
        PutLitStr("\\usepackage{color}\n");
    if (requireGraphicxPackage)
        PutLitStr("\\usepackage{graphicx}\n");
    if (requireTablePackage)
        PutLitStr("\\usepackage{longtable}\n");
    if (requireMultirowPackage)
        PutLitStr("\\usepackage{multirow}\n");
    if (requireAmsSymbPackage)
        PutLitStr("\\usepackage{amssymb}\n");
    if (requireMultiColPackage)
        PutLitStr("\\usepackage{multicol}\n");
    if (requireUlemPackage)
        PutLitStr("\\usepackage{ulem}\n");
    if (requireAmsMathPackage)
        PutLitStr("\\usepackage{amsmath}\n");
    if (requireUnicodePackage) {
        PutLitStr("\\usepackage{textgreek}\n");
        PutLitStr("\\usepackage{ucs}\n");
    }
    if (requireHyperrefPackage) {
        PutLitStr("\\usepackage{hyperref}\n");
    }
    PutLitStr("\\def\\degree{\\ensuremath{^\\circ}}\n");
    fseek(ofp, 0L, 2);          /* go back to end of stream */
}

/* This function causes the present group to be skipped.  */
static void SkipGroup(void)
{
    RTFSkipGroup();
    RTFRouteToken();
}

static void IndentParagraph(void)
{
    char buf[100];

    if (paragraph.firstIndent == 0) {
        PutLitStr("\n\\noindent");
        InsertNewLine();
    } else {
        snprintf(buf, 100, "\n\\setlength{\\parindent}{%2.3fin}",
                (double) ((double) (paragraph.leftIndent) /
                          (double) rtfTpi));
        PutLitStr(buf);
        InsertNewLine();
    }
}

static void WriteParagraphStyle(void)
{
    char buf[rtfBufSiz];
    double spacing;
    int temp1, temp2, temp3;
    double textWidth = page.width - page.leftMargin - page.rightMargin;

    temp1 = paragraph.firstIndent;
    temp2 = paragraph.leftIndent;
    temp3 = paragraph.rightIndent;

    DoParagraphCleanUp();

    paragraph.firstIndent = temp1;
    paragraph.leftIndent = temp2;
    paragraph.rightIndent = temp3;

    if (paragraph.lineSpacing == singleSpace)
        spacing = 1.0;
    else if (paragraph.lineSpacing == oneAndAHalfSpace)
        spacing = 1.5;
    else if (paragraph.lineSpacing == doubleSpace)
        spacing = 2;


    if (!preferenceValue[GetPreferenceNum("ignoreRulerSettings")]) {

        if (paragraph.leftIndent != 0 || paragraph.rightIndent != 0) {
            if (paragraph.leftIndent > 0) {
                snprintf(buf, rtfBufSiz, "\n\\makebox[%2.3fin]{}",
                        ((double) (paragraph.leftIndent) /
                         (double) rtfTpi));
                PutLitStr(buf);
            }
            snprintf(buf, rtfBufSiz, "\n\\parbox{%2.3fin}\n{",
                    textWidth -
                    ((double) (paragraph.rightIndent) / (double) rtfTpi) -
                    ((double) (paragraph.leftIndent) / (double) rtfTpi));
            PutLitStr(buf);
            paragraph.parbox = true;
        }
    }

    if (paragraph.alignment != left &&
        !preferenceValue[GetPreferenceNum("ignoreParagraphAlignment")]) {
        snprintf(buf, rtfBufSiz, "\n\\begin{%s}",
                environmentList[paragraph.alignment]);
        PutLitStr(buf);
        InsertNewLine();
        paragraph.wroteAlignment = true;
    }

    if (paragraph.lineSpacing != paragraph.oldSpacing &&
        !preferenceValue[GetPreferenceNum("ignoreSpacing")]) {
        snprintf(buf, rtfBufSiz, "\\begin{spacing}{%1.1f}", spacing);
        PutLitStr(buf);
        InsertNewLine();
        paragraph.wroteSpacing = true;
        requireSetspacePackage = true;
    }

    if (!preferenceValue[GetPreferenceNum("ignoreRulerSettings")])
        IndentParagraph();

    paragraph.newStyle = false;
    wrapCount = 0;
    suppressLineBreak = true;


}


static void WriteSectionStyle(void)
{
    char buf[rtfBufSiz];
    int i;

    DoParagraphCleanUp();
    if (charAttrCount > 0) {
        for (i = 0; i < charAttrCount; i++)
            PutLitStr("}");
        charAttrCount = 0;
    }

    if (section.cols > 1) {
        snprintf(buf, rtfBufSiz, "\n\\begin{multicols}{%d}", section.cols);
        PutLitStr(buf);
        InsertNewLine();
        wrapCount = 0;
        requireMultiColPackage = true;
    }

    section.newStyle = false;
    suppressLineBreak = true;

}

/* This function make sure that the output TeX file does not 
 * contain one very, very long line of text.
 */
static void WrapText(void)
{
    if (wrapCount >= WRAP_LIMIT) {
        if (rtfMinor == rtfSC_space) {
            PutLitChar('\n');
            wrapCount = 0;
            return;
        }
    }
    wrapCount++;

}

/*
 * Write out a character.  rtfMajor contains the input character, rtfMinor
 * contains the corresponding standard character code.
 *
 * If the input character isn't in the charset map, try to print some
 * representation of it.
 */

static void TextClass(void)
{

    CheckForBeginDocument();

    if (section.newStyle && !table.inside)
        WriteSectionStyle();
    if (paragraph.newStyle && !table.inside)
        WriteParagraphStyle();
    if (textStyle.newStyle && rtfMinor != rtfSC_space) {
        WriteTextStyle();
        textStyle.newStyle = false;
    }

    if (insideFootnote && !startFootnoteText && rtfMinor == rtfSC_space)
        return;

    if (insideFootnote)
        startFootnoteText = true;
    else
        justWroteFootnote = false;

    lastCharWasLineBreak = false;


    if (rtfMinor == rtfSC_space) {
        spaceCount++;

        /* 
         * Here we check whether there is an end group '}' just after the space
         * If so, we process that first and then process the space. This is mainly
         * for aesthetic reasons to more produce "hello \textbf{and} hello" instead 
         * of "hello \textbf{and }hello". Just some of Scott's nitpicking...
         */
        while (RTFCheckCMM(rtfText, 32, rtfSC_space))
            RTFGetToken();
            
        if (RTFCheckCM(rtfGroup, rtfEndGroup)) {
            RTFRouteToken();
            PutStdChar(rtfSC_space);
            return;
        } else if (rtfClass == rtfText && rtfMinor == rtfSC_period) {
            RTFUngetToken();
            return;             /* sometimes RTF stupidly puts a space before a period */
        } else {
            RTFUngetToken();
            RTFSetToken(rtfText, 32, rtfSC_space, rtfNoParam, " ");
        }
    } else {
        spaceCount = 0;
        suppressLineBreak = false;
        lineIsBlank = false;
    }

    if (spaceCount < 5) {
        if (rtfMinor >= rtfSC_therefore && rtfMinor < rtfSC_currency)
            requireAmsSymbPackage = true;

        /* check for a few special characters if we are translating hyperlinks */
        if (insideHyperlink) {
            switch (rtfMinor) {
            case rtfSC_underscore:
                PutLitChar('H');
                return;
            case rtfSC_backslash:
                RTFGetToken();  /* ignore backslah */
                RTFGetToken();  /* and next character */
                return;
            }
        }

        PutStdChar(rtfMinor);
        WrapText();
        if (rtfMinor != rtfSC_space)
            blankLineCount = 0;
    }

}

/*
 * This function notices destinations that should be ignored
 * and skips to their ends.  This keeps, for instance, picture
 * data from being considered as plain text.
 */

static void Destination(void)
{
    switch (rtfMinor) {
    case rtfFNContSep:
    case rtfFNContNotice:
    case rtfInfo:
    case rtfIndexRange:
    case rtfITitle:
    case rtfISubject:
    case rtfIAuthor:
    case rtfIOperator:
    case rtfIKeywords:
    case rtfIComment:
    case rtfIVersion:
    case rtfIDoccomm:
    case rtfUserPropsGroup:
    case rtfWGRFmtFilter:
        RTFSkipGroup();
        break;
    }
}

/* reads footnote. Just puts a footnote wrapper around whatever is
 * inside the footnote. Table footnotes are skipped for now
 * until I figure out a way that TeX likes.
 */
static void ReadFootnote(void)
{
    int footnoteGL;

    suppressLineBreak = true;
    startFootnoteText = false;


    /* if we are in a table, skip the footnote. Later, I'll try to implement
     * it in a way LaTeX likes
     */
    /* 
       if (table.inside)
       {
       RTFMsg ("* Warning! Skipping footnote in table\n");
       SkipGroup ();
       return;
       }
     */
    SetGroupLevels(SAVE_LEVELS);

    ForceCloseCharAttr();

    footnoteGL = groupLevel;
    PutLitStr("\\footnote{");
    wrapCount += 10;
    insideFootnote = true;
    while (groupLevel >= footnoteGL) {
        RTFGetToken();
        RTFRouteToken();
    }
    if (insideFootnote)
        PutLitStr("}");
    insideFootnote = false;
    justWroteFootnote = true;
    wrapCount++;

    SetGroupLevels(RESTORE_LEVELS);
    textStyle.newStyle = true;
    suppressLineBreak = false;

}

/*
 * This function gets called when a \par token meaning is encountered.
 * We need to figure out if the \par is just an ordinary line break or the
 * end of a paragraph, since LaTeX treats paragraph endings differently from line 
 * breaks. We do this by scanning forward for a \par or \pard token.  If these
 * are found first, then the paragraph ends, otherwise it is just a line break.
 */
static void CheckForParagraph(void)
{
    int i;
    int storeGroupLevel = groupLevel;
    boolean newParagraph = false;

    if (suppressLineBreak == true)
        return;

    if (writingHeading1 || writingHeading2 || writingHeading3) {

        RTFGetToken();
        if (RTFCheckCM(rtfGroup, rtfEndGroup)) {
            RTFUngetToken();
            return;
        }
        RTFUngetToken();

        if (writingHeading1) {
            DoParagraphCleanUp();
            PutLitStr(heading1String);
            writingHeading1 = true;
            wrapCount = (int) strlen(heading1String);
            paragraph.newStyle = false;
        } else if (writingHeading2) {
            DoParagraphCleanUp();
            PutLitStr(heading2String);
            writingHeading2 = true;
            wrapCount = (int) strlen(heading2String);
            paragraph.newStyle = false;
        } else if (writingHeading3) {
            DoParagraphCleanUp();
            PutLitStr(heading3String);
            writingHeading3 = true;
            wrapCount = (int) strlen(heading3String);
            paragraph.newStyle = false;
        }
        return;
    }

    if (table.inside) {
        if (!suppressLineBreak) {
            PutLitStr(" \\linebreak");
            InsertNewLine();
            wrapCount = 0;
        }
        return;
    }


    if (lastCharWasLineBreak) {
        if (charAttrCount > 0) {
            SetGroupLevels(SAVE_LEVELS);
            continueTextStyle = true;
        }
        
        CheckForCharAttr();
        for (i = 0; i < charAttrCount; i++)
            PutLitChar('}');
            
        if (blankLineCount < MAX_BLANK_LINES) {
            CheckForCharAttr();
            InsertNewLine();
            InsertNewLine();
            blankLineCount += 2;
        }
        
        if (!preferenceValue[GetPreferenceNum("ignoreRulerSettings")])
            IndentParagraph();

        charAttrCount = 0;
        if (continueTextStyle) {
            SetGroupLevels(RESTORE_LEVELS);
            textStyle.newStyle = true;
            continueTextStyle = false;
        }
        return;
    }

    RTFGetToken();

    while (1) {
        /* stop if we see a line break */
        if (RTFCheckMM(rtfSpecialChar, rtfPar)) {
            newParagraph = true;
            break;
        }

        /* or a page break */
        if (RTFCheckMM(rtfSpecialChar, rtfPage) ) {
            newParagraph = true;
            break;
        }

        /* or a paragraph definition */
        if (RTFCheckMM(rtfParAttr, rtfParDef)) {
            newParagraph = true;
            break;
        }

        /* or a destination */
        if (RTFCheckCM(rtfControl, rtfDestination)) {
            newParagraph = false;
            break;
        }

        /* or something else */
        if ( (rtfClass == rtfText && rtfMinor != rtfSC_space)
           || rtfClass == rtfEOF 
           || rtfMajor == rtfSpecialChar
           || rtfMajor == rtfTblAttr 
           || rtfMajor == rtfFontAttr
           || rtfMajor == rtfSectAttr 
           || rtfMajor == rtfPictAttr
           || rtfMajor == rtfObjAttr) {
            newParagraph = false;
            break;
        }

        /* otherwise keep looking and route the tokens scanned */
        RTFRouteToken();
        RTFGetToken();
    }

    /* if we saw a line break or a paragraph definition, the paragraph has ended. */
    if (newParagraph) {

        if (charAttrCount > 0) {
            SetGroupLevels(SAVE_LEVELS);
            continueTextStyle = true;
        }
        
        CheckForCharAttr();
        for (i = 0; i < charAttrCount; i++)
            PutLitChar('}');
        charAttrCount = 0;
        
        if (groupLevel <= storeGroupLevel && blankLineCount < MAX_BLANK_LINES) {
            /* PutLitStr("[new par, but group decreased]"); */
            CheckForCharAttr();
            InsertNewLine();
            InsertNewLine();
            blankLineCount += 2;
        } else if (!suppressLineBreak && !(textStyle.open) && !lineIsBlank) {
            /* PutLitStr("[new par but same group depth]"); */
            PutLitStr("\\\\{}");  /* braces because next line may start with [ */
            InsertNewLine();
            InsertNewLine();
        }
        
        wrapCount = 0;
        lastCharWasLineBreak = true;
        RTFUngetToken();
        if (continueTextStyle) {
            SetGroupLevels(RESTORE_LEVELS);
            textStyle.newStyle = true;
        }
        return;
    }

    if (rtfClass != rtfEOF && !(textStyle.open) && !lineIsBlank) {
        /* PutLitStr("[line break]"); */
        PutLitStr("\\\\{}");
        InsertNewLine();
    }
    
    wrapCount = 0;
    RTFUngetToken();
    return;
}

/*
 * The reason these use the rtfSC_xxx thingies instead of just writing
 * out ' ', '-', '"', etc., is so that the mapping for these characters
 * can be controlled by the TeX-map file.
 */

static void SpecialChar(void)
{
    int i;

    switch (rtfMinor) {
    case rtfSect:
    case rtfLine:
    case rtfPar:
        if (!suppressLineBreak)
            CheckForParagraph();
        break;
/*      case rtfCurFNote:
                PutLitChar ('}');
                EnsureNoMathMode();
                charAttrCount--;
                textStyle.subScriptGroupLevel = 0;
                textStyle.superScriptGroupLevel = 0;
                break; 
*/
    case rtfNoBrkSpace:
        PutStdChar(rtfSC_nobrkspace);
        break;
    case rtfTab:
        PutLitStr("\\tab ");
        break;
    case rtfNoBrkHyphen:
        PutStdChar(rtfSC_nobrkhyphen);
        break;
    case rtfBullet:
        PutStdChar(rtfSC_bullet);
        suppressLineBreak = false;
        break;
    case rtfEmDash:
        PutStdChar(rtfSC_emdash);
        suppressLineBreak = false;
        break;
    case rtfEnDash:
        PutStdChar(rtfSC_endash);
        suppressLineBreak = false;
        break;
    case rtfLQuote:
        PutStdChar(rtfSC_quoteleft);
        suppressLineBreak = false;
        break;
    case rtfRQuote:
        PutStdChar(rtfSC_quoteright);
        suppressLineBreak = false;
        break;
    case rtfLDblQuote:
        PutStdChar(rtfSC_quotedblleft);
        suppressLineBreak = false;
        break;
    case rtfRDblQuote:
        PutStdChar(rtfSC_quotedblright);
        suppressLineBreak = false;
        break;
    case rtfPage:
        if (!(table.inside)) {
            if (writingHeading1) {
                for (i = 0; i < CountCharInString(heading1String, '{');
                     i++)
                    PutLitStr("}");
                InsertNewLine();
                wrapCount = 0;
                writingHeading1 = false;
                suppressLineBreak = true;
                blankLineCount++;
            } else if (writingHeading2) {
                for (i = 0; i < CountCharInString(heading2String, '{');
                     i++)
                    PutLitStr("}");
                InsertNewLine();
                wrapCount = 0;
                writingHeading2 = false;
                suppressLineBreak = true;
                blankLineCount++;
            } else if (writingHeading3) {
                for (i = 0; i < CountCharInString(heading3String, '{');
                     i++)
                    PutLitStr("}");
                InsertNewLine();
                wrapCount = 0;
                writingHeading3 = false;
                suppressLineBreak = true;
                blankLineCount++;
            }
            CheckForCharAttr();
            if (blankLineCount < MAX_BLANK_LINES) {
                PutLitStr("\n\n");
                blankLineCount++;
            }

            PutLitStr("\\newpage\n");
            if (!preferenceValue[GetPreferenceNum("ignoreRulerSettings")])
                IndentParagraph();
            wrapCount = 0;
            suppressLineBreak = true;
        }
        break;
    }
}


/* This function looks at a few character attributes that are relevant to LaTeX */
static void CharAttr(void)
{
    switch (rtfMinor) {

    case rtfItalic:
    case rtfBold:
    case rtfUnderline:
    case rtfFontSize:
    case rtfSmallCaps:
    case rtfAllCaps:
    case rtfDbUnderline:
        if (!(int) preferenceValue[GetPreferenceNum("ignoreTextStyle")])
            SetTextStyle();
        break;
    case rtfSubScript:
    case rtfSubScrShrink:
    case rtfSuperScript:
    case rtfSuperScrShrink:
        SetTextStyle();
        break;
    case rtfForeColor:
        if (requireColorPackage)
            SetTextStyle();
        break;
    case rtfPlain:
        CheckForCharAttr();     /* suggested by Jens Ricky */
        InitializeTextStyle();
        break;
    case rtfDeleted:
        RTFSkipGroup();
        break;
    }


}

/* This routine sets attributes for the detected cell and 
 * adds it to the table.cellInfo list. Memory for cells is 
 * allocated dynamically as each cell is encountered. 
 */
static void ReadCell(void)
{
    cell *cellPtr;
    char *fn = "ReadCell";

    cellPtr = New(cell);
    if (!cellPtr) {
        RTFPanic("%s: cannot allocate cell entry", fn);
        exit(1);
    }

    cellPtr->nextCell = table.cellInfo;
    cellPtr->x = table.rows;
    cellPtr->y = (table.rowInfo)[table.rows];
    if (table.cols == 0)
        cellPtr->left = table.leftEdge;
    else
        cellPtr->left = (table.cellInfo)->right;
    cellPtr->right = rtfParam;
    cellPtr->width =
        (double) ((double) (cellPtr->right) -
                  (double) (cellPtr->left)) / rtfTpi;
    cellPtr->index = table.cellCount;
    cellPtr->mergePar = table.cellMergePar;
    table.cellMergePar = none;  /* reset */
    table.cellInfo = cellPtr;

    if (cellPtr->right <= table.previousColumnValue)
        cellPtr->right = table.previousColumnValue + 100;

    table.previousColumnValue = cellPtr->right;

}

static void DoTableAttr(void)
{
    switch (rtfMinor) {
    case rtfRowLeftEdge:
        table.leftEdge = rtfParam;
        break;
    case rtfCellPos:
        ReadCell();
        table.cellCount++;
        ((table.rowInfo)[table.rows])++;
        (table.cols)++;
        break;
    case rtfVertMergeRngFirst:
        table.cellMergePar = first;
        break;
    case rtfVertMergeRngPrevious:
        table.cellMergePar = previous;
        break;

    }

}

/* This function searches the cell list by the cell index */
static cell *GetCellInfo(int cellNum)
{
    cell *cellPtr;
    char *fn = "GetCellInfo";

    if (cellNum == -1)
        return (table.cellInfo);
    for (cellPtr = (table.cellInfo); cellPtr != (cell *) NULL;
         cellPtr = cellPtr->nextCell) {
        if (cellPtr->index == cellNum)
            break;
    }
    if (cellPtr == (cell *) NULL)
        RTFPanic("%s: Attempting to access invalid cell at index %d\n", fn,
                 cellNum);
    return (cellPtr);           /* NULL if not found */
}

/* This function searches the cell list by the cell coordinates */
static cell *GetCellByPos(int x, int y)
{
    cell *cellPtr;
    char *fn = "GetCellByPos";

    for (cellPtr = (table.cellInfo); cellPtr != (cell *) NULL;
         cellPtr = cellPtr->nextCell) {
        if (cellPtr->x == x && cellPtr->y == y)
            break;
    }
    if (cellPtr == (cell *) NULL)
        RTFPanic("%s: Attempting to access invalid cell at %d, %d\n", fn,
                 x, y);
    return (cellPtr);           /* NULL if not found */
}



/* In RTF, each table row need not start with a table row definition.
 * The next row may decide to use the row definition of the previous
 * row. In that case, I need to call this InheritTableRowDef function
 */
static void InheritTableRowDef(void)
{
    int prevRow;
    int cellsInPrevRow;
    cell *cellPtr, *newCellPtr;
    int i;
    char *fn = "InheritTableRowDef";

    prevRow = table.rows;
    cellsInPrevRow = (table.rowInfo)[prevRow];

    (table.rowInfo)[prevRow + 1] = (table.rowInfo)[prevRow];

    for (i = 0; i < cellsInPrevRow; i++) {

        cellPtr = GetCellByPos(prevRow, i);

        newCellPtr = New(cell);
        if (!newCellPtr) {
            RTFPanic("%s: cannot allocate inheriting cell entry", fn);
            exit(1);
        }

        newCellPtr->nextCell = table.cellInfo;
        newCellPtr->x = prevRow + 1;
        newCellPtr->y = cellPtr->y;
        newCellPtr->left = cellPtr->left;
        newCellPtr->right = cellPtr->right;
        newCellPtr->width = cellPtr->width;
        newCellPtr->index = table.cellCount;
        newCellPtr->mergePar = cellPtr->mergePar;
        table.cellMergePar = none;      /* reset */
        table.cellInfo = newCellPtr;
        table.cellCount++;


    }


}

/*
 This function figures out how many columns the cell spans. 
 This is done by comparing the cell's left and right edges 
 to the sorted column border array. If the left and right 
 edges of the cell are not consecutive entries in the array, 
 the cell spans multiple columns.
 */
static int GetColumnSpan(cell * cellPtr)
{
    int i, j;

    /* if this is the last cell in the row, make its right edge
       flush with the table right edge */
    if (cellPtr->y == ((table.rowInfo)[cellPtr->x]) - 1)
        cellPtr->right = (table.columnBorders)[table.cols];

    for (i = 0; i < table.cols; i++)
        if ((table.columnBorders)[i] == cellPtr->left)
            break;
    for (j = i; j < table.cols + 1; j++)
        if ((table.columnBorders)[j] == cellPtr->right)
            break;


    return (j - i);
}


/* 
 This routine prescans the table. It figures out how many rows there are in the table
 and the number of cells in each row. It also calculates the cell widths. Finally, it
 buils an array of column borders that is useful in figuring out whether a cell spans
 multiple columns. It has lots of diagnostic printf statements. Shows how much I
 struggled, and how much I trust it.
*/
static void PrescanTable(void)
{
    size_t tableStart;
    boolean foundRow = true;
    boolean foundColumn = true;
    int i, j;
    cell *cellPtr, *cellPtr1;
    char *fn = "PrescanTable";
    short prevChar;
    int maxCols = 0;
    int tableLeft, tableRight, tableWidth;

    RTFStoreStack();
    prevChar = RTFPushedChar();

    /* mark the current cursor position */
    tableStart = ftell(ifp);

    RTFGetToken();

    table.rows = 0;
    table.cols = 0;
    table.inside = true;
    table.cellInfo = (cell *) NULL;

    /* Prescan each row until end of the table. */
    while (foundRow) {
        table.cols = 0;
        (table.rowInfo)[table.rows] = 0;

        while (foundColumn) {
            RTFGetToken();
            if (RTFCheckMM(rtfSpecialChar, rtfOptDest) != 0)
                RTFSkipGroup();

            else if (RTFCheckCM(rtfControl, rtfTblAttr) != 0)
                RTFRouteToken();

            else if (RTFCheckMM(rtfSpecialChar, rtfRow) != 0
                     || rtfClass == rtfEOF)
                foundColumn = false;

        }

/*              RTFMsg ("* reached end of row %d\n", table.rows); */

        table.inside = false;
        table.newRowDef = false;

        while (1) {

            RTFGetToken();

            if (RTFCheckMM(rtfSpecialChar, rtfOptDest) != 0) {
                RTFSkipGroup();
            }

            else if (RTFCheckMM(rtfTblAttr, rtfRowDef) != 0) {
                table.inside = true;
                table.newRowDef = true;
                break;
            }

            else if (rtfClass == rtfText || RTFCheckCM(rtfControl, rtfSpecialChar) != 0) {
                break;
            }

            else if (RTFCheckMM(rtfParAttr, rtfInTable) != 0) {
                table.inside = true;
                break;
            }

            else if (rtfClass == rtfEOF) {
/*                              printf ("* end of file!\n"); */
                table.inside = false;
                break;
            }
        }


        if (!(table.inside))
            foundRow = false;
        else
            foundRow = true;


        if ((table.rowInfo)[table.rows] == 0)
            (table.rowInfo)[table.rows] = (table.rowInfo)[table.rows - 1];

        if (table.cols > maxCols) {
            maxCols = table.cols;
        }

/*              RTFMsg ("* read row %d with %d cells\n", table.rows, (table.rowInfo)[table.rows]); */
        (table.rows)++;
        table.previousColumnValue = -1000000;

/*              RTFMsg ("* inside table is %d, new row def is %d\n", table.inside, table.newRowDef); */

        if (table.inside && !(table.newRowDef)) {
            (table.rows)--;
            InheritTableRowDef();
/*                      RTFMsg ("* Inherited: read row %d with %d cells\n", table.rows, (table.rowInfo)[table.rows]); */
            (table.rows)++;
        }


        foundColumn = true;

    }


    table.cols = maxCols;

    {
        int *rightBorders;
        boolean enteredValue;

        rightBorders = (int *) RTFAlloc((table.cellCount) * sizeof(int));
        if (!rightBorders) {
            RTFPanic("%s: cannot allocate array for cell borders\n", fn);
            exit(1);
        }

        table.cols = 0;
        for (cellPtr = table.cellInfo; cellPtr != NULL; cellPtr = cellPtr->nextCell) {
            enteredValue = false;
            for (j = 0; j < table.cols; j++)
                if (rightBorders[j] == cellPtr->right)
                    enteredValue = true;
            if (!enteredValue)
                rightBorders[(table.cols)++] = cellPtr->right;
            if (cellPtr->y == 0)
                cellPtr->left = table.leftEdge;
        }

        /* allocate array for coulumn border entries. */
        table.columnBorders = (int *) RTFAlloc(((table.cols) + 1) * sizeof(int));
        
        if (!table.columnBorders) {
            RTFPanic("%s: cannot allocate array for cell borders\n", fn);
            exit(1);
        }

        for (i = 0; i < table.cols; i++)
            (table.columnBorders)[i + 1] = rightBorders[i];

        RTFFree(rightBorders);
    }

/*      
 *      Table parsing can be messy, and it is still buggy. 
 *  Here are a few diagnostic messages
 *      that I print out when I am having trouble. 
 *  Comment out for normal operation.
*/


/*      RTFMsg ("* table has %d rows and %d cols \n", table.rows, table.cols); */

    (table.columnBorders)[0] = table.leftEdge;


    /*
       sort the column border array in ascending order. 
       This is important for figuring out 
       whether a cell spans multiple columns. 
       This is calculated in the function GetColumnSpan.
     */
/*      RTFMsg ("* sorting borders...\n"); */
    for (i = 0; i < (table.cols); i++)
        for (j = i + 1; j < (table.cols + 1); j++)
            if ((table.columnBorders)[i] > (table.columnBorders)[j])
                Swap((table.columnBorders)[i], (table.columnBorders)[j]);

/*      RTFMsg ("* table left border is at %ld\n", (table.columnBorders)[0]); */
    for (i = 1; i < (table.cols + 1); i++) {
        /* Sometimes RTF does something stupid and assigns two adjoining cell
           the same right border value. Dunno why. If this happens, I move the
           second cell's right border by 10 twips. Microsoft really has some 
           morons!
         */
        if (table.columnBorders[i] == table.columnBorders[i - 1])
            table.columnBorders[i] += 10;
/*
                RTFMsg ("* cell right border is at %ld\n", table.columnBorders[i]); 
*/
    }


    /* fill in column spans for each cell */
    for (i = 0; i < table.cellCount; i++) {
        cellPtr = GetCellInfo(i);
        if (!cellPtr){
            RTFPanic("%s: Attempting to access invalid cell at index %d\n", fn, i);
            exit(1);
        }
        
        cellPtr->columnSpan = GetColumnSpan(cellPtr);
        if (cellPtr->columnSpan > 1)
            table.multiCol = true;
/*              RTFMsg ("* cell %d spans %d columns\n", cellPtr->index, cellPtr->columnSpan); */

        /* correct the vertical cell position for any multicolumn cells */
        if ((cellPtr->y) != 0) {
            cellPtr1 = GetCellInfo(i - 1);
            if (cellPtr == (cell *) NULL)
                RTFPanic
                    ("%s: Attempting to access invalid cell at index %d\n",
                     fn, i);
            cellPtr->y = cellPtr1->y + cellPtr1->columnSpan;

        }

        tableLeft = (table.columnBorders)[0];
        tableRight = (table.columnBorders)[table.cols];
        tableWidth = tableRight - tableLeft;

        cellPtr->width =
            (double) ((double) (cellPtr->right) -
                      (double) (cellPtr->left)) / rtfTpi;
        if (tableWidth > 4.5 * rtfTpi)
            cellPtr->width *=
                (double) (4.5 * (double) rtfTpi / (double) tableWidth);

    }

/*
        for (cellPtr = table.cellInfo; cellPtr != (cell *)NULL; cellPtr = cellPtr->nextCell)
                RTFMsg ("* now cell %d (%d, %d) has left and right borders at %ld, %ld and spans %d columns\n", 
                cellPtr->index, cellPtr->x, cellPtr->y, cellPtr->left, cellPtr->right, cellPtr->columnSpan);
*/
    /* go back to beginning of the table */
    fseek(ifp, tableStart, 0);
    RTFSimpleInit();
    RTFSetPushedChar(prevChar);
    RTFRestoreStack();

}

/*
 This routine figures out how many cells are merged vertically and writes the
 corresponding \multirow statement.
 */
static void DoMergedCells(cell * cellPtr)
{
    int i;
    int x, y;
    cell *localCellPtr;
    char buf[rtfBufSiz];

    x = cellPtr->x;
    y = cellPtr->y;

    i = 1;
    localCellPtr = GetCellByPos(x + i, y);
    for (i = 1; localCellPtr->mergePar == previous; i++)
        if (x + i > table.rows - 1)
            break;
        else
            localCellPtr = GetCellByPos(x + i, y);

    snprintf(buf, rtfBufSiz, "\\multirow{%d}{%1.3fin}{%s ",
            i - 1, cellPtr->width, justificationList[paragraph.alignment]);
    PutLitStr(buf);

    table.multiRow = true;
    requireMultirowPackage = true;

}


/* writes cell information for each cell.
   Each cell is defined in a multicolumn
   environment for maximum flexibility.
   Useful when we have merged rows and
   columns.
 */
static void WriteCellHeader(int cellNum)
{
    char buf[rtfBufSiz];
    cell *cellPtr;
    char *fn = "WriteCellHeader";

    if (wroteCellHeader)
        return;

    cellPtr = GetCellInfo(cellNum);
    if (!cellPtr) {
        RTFPanic("%s: Attempting to access invalid cell at index %d\n", fn, cellNum);
        exit(1);
    }

    if (table.multiCol) {
        snprintf(buf, rtfBufSiz, "\\multicolumn{%d}{", cellPtr->columnSpan);
        PutLitStr(buf);
        if (cellPtr->columnSpan < 1)
            RTFMsg("* Warning: nonsensical table encountered...cell %d spans %d columns.\n \
                                        Proceed with caution!\n",
                   cellPtr->index, cellPtr->columnSpan);
        /* this check is to draw the left vertical boundary of the table */
        if (cellPtr->y == 0)
            PutLitChar('|');
        if (cellPtr->mergePar == first) {
            snprintf(buf, rtfBufSiz, "p{%1.3fin}|}\n{", cellPtr->width);
            PutLitStr(buf);
        } else {
            snprintf(buf, rtfBufSiz, "p{%1.3fin}|}\n{%s", cellPtr->width,
                    justificationList[paragraph.alignment]);
            PutLitStr(buf);
            InsertNewLine();
        }
    } else if (cellPtr->mergePar != first) {
        snprintf(buf,rtfBufSiz, "{%s ", justificationList[paragraph.alignment]);
        PutLitStr(buf);
    } else {
        PutLitStr("{");
    }

    if (rtfClass == rtfText)
        WriteTextStyle();

    if (cellPtr->mergePar == first)
        DoMergedCells(cellPtr);

    suppressLineBreak = true;
    wroteCellHeader = true;

}

/* This is where we actually process each table row. */
static void ProcessTableRow(int rowNum)
{
    boolean endOfRow = false;
    boolean startCellText = false;
    cell *cellPtr;
    int cellsInThisRow = 0;
    int i;

    wroteCellHeader = false;


    while (!endOfRow) {
        RTFGetToken();
        if (RTFCheckCM(rtfControl, rtfCharAttr) != 0) {
            switch (rtfMinor) {
            case rtfFontNum:
                RTFRouteToken();
                break;
            case rtfPlain:
                CheckForCharAttr();     /* suggested by Jens Ricky */
                InitializeTextStyle();
                break;
            case rtfBold:
            case rtfItalic:
            case rtfUnderline:
            case rtfAllCaps:
            case rtfSubScrShrink:
            case rtfSuperScrShrink:
            case rtfForeColor:
            case rtfBackColor:
            case rtfFontSize:
                CharAttr();
                if (startCellText
                    && (cellsInThisRow < (table.rowInfo)[rowNum]))
                    RTFRouteToken();
                break;
            }
        }

        else if (RTFCheckCM(rtfControl, rtfDestination)) {
            WriteCellHeader(table.cellCount);
            cellPtr = GetCellInfo(table.cellCount);
            wrapCount = 1;
            startCellText = true;
            RTFRouteToken();
        }

        /* ignore unknown destination groups */
        else if (strcmp(rtfTextBuf, "\\*") == 0) {
            RTFSkipGroup();
            continue;
        }

        else if (RTFCheckCM(rtfControl, rtfSpecialChar) != 0) {
            switch (rtfMinor) {
            case rtfRow:
                return;

            case rtfCell:
                (table.cellCount)++;
                cellsInThisRow++;
                if (!startCellText) {   /* this is in case the cell was blank. 
                                         * We still have to tell latex about it. */
                    cellPtr = GetCellInfo(table.cellCount - 1);
                    WriteCellHeader(table.cellCount - 1);
                }
                SetGroupLevels(SAVE_LEVELS);
                CheckForCharAttr();
                for (i = 0; i < charAttrCount; i++)
                    PutLitChar('}');
                charAttrCount = 0;
                SetGroupLevels(RESTORE_LEVELS);
                if (cellPtr->mergePar == first)
                    PutLitChar('}');
                PutLitStr("} & ");
                InsertNewLine();
                suppressLineBreak = true;
                startCellText = false;  /* we are not in the text 
                                         * field of a cell any more */
                paragraph.alignment = left;
                wroteCellHeader = false;
                break;

            default:
                if (!startCellText) {   /* this is in case the cell was blank. 
                                         * We still have to tell latex about it. */
                    cellPtr = GetCellInfo(table.cellCount);
                    WriteCellHeader(table.cellCount);
                    startCellText = true;
                }
                RTFRouteToken();
                break;
            }
        }

        else if (RTFCheckCM(rtfControl, rtfTblAttr) == 0) {
            /* if we see text, we are already in the text field of a cell */
            if (rtfClass == rtfText && !startCellText) {
                WriteCellHeader(table.cellCount);
                cellPtr = GetCellInfo(table.cellCount);
                wrapCount = 1;
                startCellText = true;
                textStyle.newStyle = false;
            }
            RTFRouteToken();
        }

        else if (RTFCheckCM(rtfControl, rtfGroup) != 0)
            RTFRouteToken();

    }

}

/*
 This function draws horizontal lines within a table. It looks 
 for vertically merged rows that  do not have any bottom border
 */
static void DrawTableRowLine(int rowNum)
{
    int i, cellPosition;
    cell *cellInfo1, *cellInfo2;
    char buf[rtfBufSiz];

    /* if we are at the last row of the table, just draw a straight \hline. */
    if (rowNum == (table.rows) - 1 || !table.multiRow) {
        PutLitStr("\\hline");
        InsertNewLine();
        return;
    }

    /* otherwise use \cline for every cell */
    /* this is to count cell positions as if the table is a matrix. */
    cellPosition = 0;
    for (i = 0; i < (table.rowInfo)[rowNum]; i++) {

        cellInfo1 = GetCellByPos(rowNum, cellPosition);
        cellPosition += cellInfo1->columnSpan;

        if (cellInfo1->mergePar == none) {
            snprintf(buf, rtfBufSiz, "\\cline{%d-%d}", cellInfo1->y + 1,
                    cellInfo1->y + cellInfo1->columnSpan);
            PutLitStr(buf);
        }

        else if (cellInfo1->mergePar == previous) {
            cellInfo2 = GetCellByPos(rowNum + 1, i);
            if (cellInfo2->mergePar != previous) {
                snprintf(buf, rtfBufSiz, "\\cline{%d-%d}", cellInfo1->y + 1,
                        cellInfo1->y + cellInfo1->columnSpan);
                PutLitStr(buf);
            }
        }

    }

    InsertNewLine();
}



/* All right, the big monster. When we reach a table, 
 * we don't know anything about it, ie. number of rows
 * and columns, whether any rows or columns are merged,
 * etc. We have to prescan the table first, where we
 * get vital table parameters, and then come back to
 * read the table in actual.
 */
static void DoTable(void)
{
    int i, rowNum;
    cell *cellPtr;
    char buf[100];

    requireTablePackage = true;
    table.previousColumnValue = -1000000;

    /* throw away old cell information lists */
    while (table.cellInfo != (cell *) NULL) {
        cellPtr = (table.cellInfo)->nextCell;
        RTFFree((char *) table.cellInfo);
        table.cellInfo = cellPtr;
    }

    /* Prescan table */
    table.cellCount = 0;
    PrescanTable();
/*      printf ("* done prescanning table...\n"); */
    table.cellCount = 0;

    table.inside = true;

    DoParagraphCleanUp();

    if (blankLineCount < MAX_BLANK_LINES) {
        InsertNewLine();
        InsertNewLine();
        blankLineCount++;
    }

    PutLitStr("\\begin{");
    PutLitStr(tableString);
    PutLitStr("}{");
    if (table.multiCol) {
        for (i = 0; i < table.cols; i++)
            PutLitChar('l');
        PutLitStr("}");
    } else {
        PutLitStr("|");
        for (i = 0; i < table.cols; i++) {
            cellPtr = GetCellInfo(i);
            if (cellPtr->x > 0)
                break;
            snprintf(buf, 100, "p{%1.3fin}|", cellPtr->width);
            PutLitStr(buf);
        }
        PutLitStr("}");
    }
    PutLitStr("\n\\hline");
    InsertNewLine();

    wrapCount = 0;

    paragraph.alignment = left; /* default justification */

    rowNum = 0;

/*      printf ("* processing table rows...\n");        */
    for (i = 0; i < table.rows; i++) {
        snprintf(buf, 100, "%% ROW %d", i + 1);
        PutLitStr(buf);
        InsertNewLine();

        ProcessTableRow(rowNum);

        fseek(ofp, -4, 2);      /* erase the "& \n" at the end of the last cell of the row */
        PutLitStr("\\\\");    
        InsertNewLine();
        if (i < (table.rows - 1))
            DrawTableRowLine(rowNum);
        wrapCount = 0;
        paragraph.alignment = left;
        rowNum++;

        InitializeTextStyle();

    }

    PutLitStr("\\hline\n");
    snprintf(buf, 100, "\\end{%s}\n", tableString);
    PutLitStr(buf);
    InsertNewLine();
    lastCharWasLineBreak = true;
    suppressLineBreak = true;

    wrapCount = 0;


    RTFFree((char *) table.columnBorders);
    table.inside = false;       /* end of table */
    table.previousColumnValue = -1000000;
    table.multiCol = false;
    table.multiRow = false;

}

/* set paragraph attributes that might be useful */
static void ParAttr(void)
{
    int i;
    RTFStyle *stylePtr = NULL;

    if (insideFootnote || insideHyperlink)
        return;

    if (!(table.inside)) {
        CheckForBeginDocument();

        switch (rtfMinor) {
        case rtfSpaceBetween:
            paragraph.oldSpacing = paragraph.lineSpacing;
            if (rtfParam == 240)
                paragraph.lineSpacing = singleSpace;
            else if (rtfParam == 360)
                paragraph.lineSpacing = oneAndAHalfSpace;
            else if (rtfParam == 480)
                paragraph.lineSpacing = doubleSpace;
            break;
        case rtfQuadCenter:
            paragraph.alignment = center;
            break;
        case rtfQuadJust:
        case rtfQuadLeft:
            InsertNewLine();
            wrapCount = 0;
            paragraph.alignment = left;
            break;
        case rtfQuadRight:
            paragraph.alignment = right;
            break;
        case rtfParDef:
            DoParagraphCleanUp();
            InitializeTextStyle();
            for (i = 0; i < charAttrCount; i++)
                PutLitChar('}');
            charAttrCount = 0;
            paragraph.alignment = left;
            paragraph.newStyle = true;
            break;
        case rtfStyleNum:
            if ((stylePtr = RTFGetStyle(rtfParam)) == (RTFStyle *) NULL)
                break;
            if (strcmp(stylePtr->rtfSName, "heading 1") == 0) {
                InsertNewLine();
                InsertNewLine();
                PutLitStr(heading1String);
                writingHeading1 = true;
                wrapCount = (int)strlen(heading1String);
                paragraph.newStyle = false;
            } else if (strcmp(stylePtr->rtfSName, "heading 2") == 0) {
                InsertNewLine();
                InsertNewLine();
                PutLitStr(heading2String);
                writingHeading2 = true;
                wrapCount = (int)strlen(heading2String);
                paragraph.newStyle = false;
            } else if (strcmp(stylePtr->rtfSName, "heading 3") == 0) {
                InsertNewLine();
                InsertNewLine();
                PutLitStr(heading3String);
                writingHeading3 = true;
                wrapCount = (int)strlen(heading3String);
                paragraph.newStyle = false;
            }
            break;
        case rtfFirstIndent:
            paragraph.firstIndent = rtfParam;
            break;
        case rtfLeftIndent:
            paragraph.leftIndent = rtfParam;
            break;
        case rtfRightIndent:
            paragraph.rightIndent = rtfParam;
            break;
        }
    } else {
        switch (rtfMinor) {
        case rtfQuadCenter:
            paragraph.alignment = center;
            break;
        case rtfQuadRight:
            paragraph.alignment = right;
            break;
        }
    }

}



static void SectAttr(void)
{
    switch (rtfMinor) {
    case rtfColumns:
        section.cols = rtfParam;
        section.newStyle = true;
        break;
    case rtfSectDef:
        DoParagraphCleanUp();
        DoSectionCleanUp();
        section.cols = 1;
/*                       section.newStyle = true; */
        break;
    }

}


/* decides what to do when a control word is encountered */
static void ControlClass(void)
{

    switch (rtfMajor) {
    case rtfDefFont:
        RTFSetDefaultFont(rtfParam);
    case rtfFontAttr:
        switch (rtfMinor) {
        case rtfAnsiCodePage:
        case rtfFontCodePage:
            codePage = rtfParam;
            break;
        }
        break;
    case rtfDestination:
        Destination();
        break;
    case rtfSpecialChar:
        SpecialChar();
        break;
    case rtfCharAttr:
        CharAttr();
        break;
    case rtfListAttr:
        RTFSkipGroup();
        RTFRouteToken();
        break;
    case rtfTblAttr:            /* trigger for reading table */
        if (rtfMinor == rtfRowDef && !(table.inside)) {
            DoTable();          /* if we are not already inside a table, get into it */
        } else
            DoTableAttr();      /* if we are already inside 
                                 * a table, set table attributes */
        break;
    case rtfParAttr:
        ParAttr();
        break;
    case rtfSectAttr:
        SectAttr();
        break;
    case rtfWord97ObjAttr:
        if (rtfMinor == rtfShapeName || rtfMinor == rtfShapeValue)
            SkipGroup();
        break;
    }
}

/* called when we are done reading the RTF file. */
void EndLaTeXFile(void)
{
    WriteLaTeXFooter();
}

/* sets the output stream */
void RTFSetOutputStream(stream)
FILE *stream;
{
    ostream = stream;
}

/* This function looks for the beginning of the hex data in a \pict structure */
static int HexData(void)
{
    /* get the next token */
    RTFGetToken();

    /* if we fall into a group, skip the whole she-bang */
    if (RTFCheckCM(rtfGroup, rtfBeginGroup) != 0) {
        RTFPeekToken();
        if ((int) (rtfTextBuf[0]) == 10) {
            RTFGetToken();      /* skip any carriage returns */
            RTFPeekToken();     /* look at the next token to 
                                 * check if there is another row */
        }

        /* 
         * there are some groups within the header that contain text data that should not
         * be confused with hex data
         */
        if (RTFCheckMM(rtfDestination, rtfSp) != 0 || strcmp(rtfTextBuf, "\\*") == 0) 
        {
            RTFSkipGroup();
            return (0);
        }
    }

    /* paydirt, hex data starts */
    if (rtfClass == rtfText)
        return (1);

    /* no such luck, but set picture attributes when encountered */
    if (RTFCheckCM(rtfControl, rtfPictAttr) != 0) {
        switch (rtfMinor) {
        case rtfMacQD:
            picture.type = pict;
            break;
        case rtfWinMetafile:
            picture.type = wmf;
            break;
        case rtfPng:
            picture.type = png;
            break;
        case rtfJpeg:
            picture.type = jpeg;
            break;
        case rtfPicGoalWid:
            picture.goalWidth = rtfParam;
            break;
        case rtfPicGoalHt:
            picture.goalHeight = rtfParam;
            break;
        case rtfPicScaleX:
            picture.scaleX = rtfParam;
            break;
        case rtfPicWid:
            picture.width = rtfParam;
            break;
        case rtfPicHt:
            picture.height = rtfParam;
            break;
        case rtfPicScaleY:
            picture.scaleY = rtfParam;
            break;
        }
    }
    return (0);

}

/* some picture formats like PICT and WMF require headers that
 * the RTF file does not include.
 */
static void WritePictureHeader(FILE * pictureFile)
{
    unsigned char wmfhead[22] = {
        /* key      = */ 0xd7, 0xcd, 0xc6, 0x9a,
        /* hmf      = */ 0x00, 0x00,
        /* bbox     = */ 0xfc, 0xff, 0xfc, 0xff, 
        /* width    = */ 0x00, 0x00, 
        /* height   = */ 0x00, 0x00,
        /* inch     = */ 0x60, 0x00,
        /* reserved = */ 0x00, 0x00, 0x00, 0x00,
        /* checksum = */ 0x00, 0x00
    };
    int i;
    int height, width;

    if (picture.goalHeight == 0) {
        height = ((float) picture.height * picture.scaleY * 96) / ((float) rtfTpi * 100);
        width = (int)((float) picture.width * picture.scaleX * 96) / (rtfTpi * 100);
    } else {
        height = (int)((float) picture.goalHeight * picture.scaleY * 96) / (rtfTpi * 100);
        width = (int)((float) picture.goalWidth * picture.scaleX * 96) / (rtfTpi * 100);
    }
    wmfhead[10] = (width) % 256;
    wmfhead[11] = (width) / 256;
    wmfhead[12] = (height) % 256;
    wmfhead[13] = (height) / 256;

    /* compute Checksum */
    wmfhead[20] = 0;
    wmfhead[21] = 0;
    for (i = 0; i < 20; i += 2) {
        wmfhead[20] ^= wmfhead[i];
        wmfhead[21] ^= wmfhead[i + 1];
    }


    switch (picture.type) {
    case pict:                  /* write 512 byte empty header */
        for (i = 0; i < 512; i++)
            fputc(' ', pictureFile);
        break;
    case wmf:
        fwrite(wmfhead, 22, 1, pictureFile);
        break;
    case png:
        break;

    }

}

/* start reading hex encoded picture */
static void ConvertHexPicture(char *pictureType)
{
    FILE *pictureFile;
    char dummyBuf[rtfBufSiz];
    char pictByte;
    int groupEnd = false;
    short hexNumber;
    short hexEvenOdd = 0;       /* check if we read in even number of hex characters */

    strcpy(picture.name, "");

    /* increment the picture counter and clear dummy buffer */
    (picture.count)++;
    strcpy(dummyBuf, "");

    /* get input file name and create corresponding picture file name */
    if (pictureType == (char *) NULL)
        strcpy(pictureType, "unknown");

    strcpy(picture.name, RTFGetInputName());
    snprintf(dummyBuf, rtfBufSiz, "Fig%03d.%s", picture.count, pictureType);
    strcat(picture.name, dummyBuf);

    /* open picture file */
    if ((pictureFile = fopen(picture.name, "wb")) == NULL)
        RTFPanic("Cannot open input file %s\n", picture.name);

    /* write appropriate header */
    WritePictureHeader(pictureFile);

    /* now we have to read the hex code in pairs of two 
     * (1 byte total) such as ff, a1, 4c, etc...*/
    while (!groupEnd) {
        RTFGetToken();

        /* CR or LF in the hex stream should be skipped */
        if ((int) (rtfTextBuf[0]) == 10 || (int) (rtfTextBuf[0]) == 13)
            RTFGetToken();

        if (rtfClass == rtfGroup)
            break;
        
        if (!groupEnd) {
            hexNumber = 16 * RTFCharToHex(rtfTextBuf[0]);
            hexEvenOdd++;
        }

        RTFGetToken();
        if ((int) (rtfTextBuf[0]) == 10 || (int) (rtfTextBuf[0]) == 13)
            RTFGetToken();
            
        if (rtfClass == rtfGroup)
            break;
        
        if (!groupEnd) {
            hexNumber += RTFCharToHex(rtfTextBuf[0]);   /* this is the the number */
            hexEvenOdd--;
            /* shove that number into a character of 1 byte */
            pictByte = hexNumber;
            fputc(pictByte, pictureFile);
        }
    }

    if (fclose(pictureFile) != 0)
        printf("* error closing picture file %s\n", picture.name);
    if (hexEvenOdd)
        printf("* Warning! Odd number of hex characters read for picture %s\n",
             picture.name);
}


/* This function writes the appropriate LaTeX2e commands to include the picture 
   into the LaTeX file */
static void IncludeGraphics(char *pictureType)
{
    char *figPtr, *suffix;
    char dummyBuf[rtfBufSiz], specialBuf[rtfBufSiz];
    double scaleX, scaleY, scale;
    double width, height;
    int llx, lly, urx, ury;


    suffix = strrchr(picture.name, '.');
    if (suffix != (char *) NULL && strcmp(pictureType, "eps") == 0)
        strcpy(suffix, ".eps");
    else if (strcmp(pictureType, "pict") == 0)
        strcpy(suffix, ".pdf");

    if (picture.scaleX == 0)
        scaleX = 1;
    else
        scaleX = (double) (picture.scaleX) / 100;
    if (picture.scaleY == 0)
        scaleY = 1;
    else
        scaleY = (double) (picture.scaleY) / 100;

    if (picture.goalHeight == 0) {
        width = (double) ((double) picture.width * scaleX);
        height = (double) ((double) picture.height * scaleY);
    } else {
        width = (double) ((double) picture.goalWidth * scaleX / 20);
        height = (double) ((double) picture.goalHeight * scaleY / 20);
    }

    DoParagraphCleanUp();

    figPtr = strrchr(picture.name, PATH_SEP);
    if (!figPtr)
        figPtr = picture.name;
    else
        figPtr++;

    if (!(int) preferenceValue[GetPreferenceNum("swpMode")]) {
        if (strcmp(pictureType, "eps") == 0) {

            llx = picture.llx;
            lly = picture.lly;
            urx = picture.urx;
            ury = picture.ury;
            scale = (double) ((double) (height) / ((double) (ury - lly)));
            snprintf(dummyBuf, rtfBufSiz, "\\includegraphics[bb = %d %d %d %d, scale=%2.2f]{%s}",
                    llx, lly, urx, ury, scale, figPtr);

        } else
            snprintf(dummyBuf, rtfBufSiz, "\\includegraphics[width=%2.3fin, height=%2.3fin]{%s}",
                    width / 72, height / 72, figPtr);

        if (!(table.inside) && !insideFootnote) {
            if (height > 50) {
                PutLitStr("\\begin{figure}[htbp]");
                wrapCount = 0;
            }
            if (height > 20) {
                PutLitStr("\n\\begin{center}");
                InsertNewLine();
                wrapCount = 0;
            }
            PutLitStr(dummyBuf);
            wrapCount += (int) strlen(dummyBuf);
            if (height > 50) {
                snprintf(dummyBuf, rtfBufSiz, "\n\\caption{%s about here.}", figPtr);
                PutLitStr(dummyBuf);
            }
            if (height > 20) {
                PutLitStr("\n\\end{center}");
                InsertNewLine();
                if (height <= 50) {
                    InsertNewLine();
                    InsertNewLine();
                }
                blankLineCount++;
            }
            if (height > 50) {
                PutLitStr("\\end{figure}");
                InsertNewLine();
                InsertNewLine();
                wrapCount = 0;
            }
            suppressLineBreak = true;
        }
    } else {                    /* this is for compatibility with Scientific Word */

        snprintf(dummyBuf, rtfBufSiz,
                "\\FRAME{ftbpxFU}{%2.3fpt}{%2.3fpt}{0pt}{}{}{Figure %s}",
                width, height, figPtr);
        PutLitStr(dummyBuf);
        PutLitStr("{");
        InsertNewLine();
        snprintf(specialBuf,rtfBufSiz,
                "\\special{language \"Scientific Word\";type \"GRAPHIC\";maintain-aspect-ratio TRUE; display \"USEDEF\";valid_file \"T\";");
        PutLitStr(specialBuf);
        InsertNewLine();
        snprintf(specialBuf, rtfBufSiz, "height %2.3fpt;width %2.3fpt;depth 0pt;",
                width, height);
        PutLitStr(specialBuf);
        InsertNewLine();
        snprintf(specialBuf,rtfBufSiz,
                "cropleft \"0\";croptop \"1\";cropright \"1\";cropbottom \"0\";");
        PutLitStr(specialBuf);
        InsertNewLine();
        snprintf(specialBuf,rtfBufSiz,
                "tempfilename '%s';tempfile-properties \"XPNEU\";",
                figPtr);
        PutLitStr(specialBuf);
        PutLitStr("}}");
        InsertNewLine();
        suppressLineBreak = true;
    }
}

/* This function reads in a picture */
static void ReadPicture(void)
{
/*     char *fn = "ReadPicture"; */
    requireGraphicxPackage = true;
    picture.type = unknownPict;
    picture.width = 0;
    picture.height = 0;
    picture.goalWidth = 0;
    picture.goalHeight = 0;
    picture.scaleX = 100;
    picture.scaleY = 100;

/*     RTFMsg("%s: Starting ...\n",fn); */

    /* skip everything until we reach hex data */
    while (!HexData());

    /* Put back the first hex character into the stream (removed by HexData) */
    RTFUngetToken();

    /* Process picture */
    switch (picture.type) {
    case pict:
/*         RTFMsg("* Warning: PICT format image encountered.\n"); */
        ConvertHexPicture("pict");
        IncludeGraphics("pict");
        suppressLineBreak = true;
        break;
    case wmf:
/*         RTFMsg("* Warning: WMF format image encountered.\n"); */
        ConvertHexPicture("wmf");
        IncludeGraphics("wmf");
        suppressLineBreak = true;
        break;
    case png:
/*         RTFMsg("* Warning: PNG format image encountered.\n"); */
        ConvertHexPicture("png");
        IncludeGraphics("png");
        suppressLineBreak = true;
        break;
    case jpeg:
/*         RTFMsg("* Warning: JPEG format image encountered.\n"); */
        ConvertHexPicture("jpg");
        IncludeGraphics("jpg");
        suppressLineBreak = true;
        break;
    default:
        ConvertHexPicture("unknown");
        printf("* Warning: unknown picture type encountered.\n");
        IncludeGraphics("unknown");
        break;
    }

    /* feed "}" back to router */
    RTFRouteToken();

    /* reset picture type */
    picture.type = unknownPict;
    picture.width = 0;
    picture.height = 0;
    picture.goalWidth = 0;
    picture.goalHeight = 0;
    picture.scaleX = 100;
    picture.scaleY = 100;
    strcpy(picture.name, "");
}

/*
 * slow simplistic reimplementation of strcasestr for systems that
 * don't include it in their library
 *
 * based on a GPL implementation in OpenTTD found under GPL v2
 */

char *my_strcasestr(const char *haystack, const char *needle)
{
    size_t hay_len = strlen(haystack);
    size_t needle_len = strlen(needle);
    while (hay_len >= needle_len) {
        if (strncasecmp(haystack, needle, needle_len) == 0) 
            return (char *) haystack;

        haystack++;
        hay_len--;
    }

    return NULL;
}

static void ReadObjWidth(void)
{
    g_object_width = rtfParam;
}


/* 
* parses \objectclass and adds the class type to the global variable 'object'
*/
static void GetObjectClass(int *groupCounter)
{
    int reachedObjectClass = 0;
    int reachedEndGroup = 0;
    int i;

/* keep scanning until \objectclass is found */
    while (!reachedObjectClass) {
        RTFGetToken();
        if (RTFCheckMM(rtfObjAttr, rtfObjWid)!=0)
            ReadObjWidth();
        if (RTFCheckCM(rtfGroup, rtfBeginGroup) != 0)
            (*groupCounter)++;
        else if (RTFCheckCM(rtfGroup, rtfEndGroup) != 0)
            (*groupCounter)--;
        if (RTFCheckMM(rtfDestination, rtfObjClass) != 0)
            reachedObjectClass = 1;
        if (*groupCounter == 0) {
            object.class = unknownObjClass;
            return;
        }
    }

/* read the object class */
    strcpy(object.className, "");
    while (!reachedEndGroup) {
        RTFGetToken();
        if (RTFCheckCM(rtfGroup, rtfBeginGroup) != 0)
            RTFSkipGroup();
        if (RTFCheckCM(rtfGroup, rtfEndGroup) == 0)
            strcat(object.className, rtfTextBuf);
        else {
            reachedEndGroup = 1;
            (*groupCounter)--;
        }
    }

/* do we recognize this object class? */
    for (i = 0; objectClassList[i] != (char *) NULL; i++) {
        if (my_strcasestr(object.className, objectClassList[i]) != (char *) NULL) {
            object.class = i;
            break;
        }
        object.class = 0;
    }
}


/* 
 * The result section of an \object usually contains a picture of the object
 */
static int ReachedResult(int *groupCount)
{
    RTFGetToken();

    if (RTFCheckCM(rtfGroup, rtfBeginGroup) != 0) {
        (*groupCount)++;
        return (0);
    }

    if (RTFCheckCM(rtfGroup, rtfEndGroup) != 0) {
        (*groupCount)--;
        return (0);
    }

    if (RTFCheckMM(rtfDestination, rtfObjResult) != 0 ||
        RTFCheckMM(rtfWord97ObjAttr, rtfWord97ObjResult) != 0 ||
        RTFCheckMM(rtfDestination, rtfPict) != 0 ||
        RTFCheckMM(rtfWord97ObjAttr, rtfWord97ObjText) != 0) {
        if (RTFCheckMM(rtfDestination, rtfPict) != 0)
            word97ObjectType = word97Picture;
        else if (RTFCheckMM(rtfDestination, rtfObjResult) != 0)
            word97ObjectType = standardObject;
        else if (RTFCheckMM(rtfWord97ObjAttr, rtfWord97ObjResult) != 0)
            word97ObjectType = word97Object;
        else if (RTFCheckMM(rtfWord97ObjAttr, rtfWord97ObjText) != 0)
            word97ObjectType = word97ObjText;
        (*groupCount)--;        /* account for opening brace just 
                                 * before result control word */
        return (1);
    }

    if (rtfClass == rtfEOF) {
        RTFPanic("* EOF reached!\n");
        exit(1);
    }

    return (0);
}

/* 
 * Decodes the OLE and extract the specified stream type into a buffer.
 * This function uses the cole library 
 */
static int
DecodeOLE(char *objectFileName, char *streamType,
          unsigned char **nativeStream, uint32_t * size)
{
    COLEFS *cfs;
    COLERRNO colerrno;
    COLEFILE *coleFile;

    cfs = cole_mount(objectFileName, &colerrno);
    if (cfs == NULL) {
        cole_perror("DecodeOLE cole_mount", colerrno, objectFileName);
        return (1);
    }

#ifdef COLE_VERBOSE
    cole_print_tree (cfs, &colerrno); 
#endif

    if ((coleFile = cole_fopen(cfs, streamType, &colerrno)) == NULL) {
        cole_perror("DecodeOLE cole_fopen", colerrno, objectFileName);
        cole_umount(cfs, NULL);
        return 1;
    }

    *size = (uint32_t) cole_fsize(coleFile);

    *nativeStream = (unsigned char *) malloc(*size);

    if (*nativeStream == (unsigned char *) NULL) {
        RTFMsg("* DecodeOLE: memory allocation failed for native stream!\n");
        cole_fclose(coleFile, &colerrno);
        cole_umount(cfs, NULL);
        return 1;
    }

    if (cole_fread(coleFile, *nativeStream, *size, &colerrno) == 0) {
        cole_perror("DecodeOLE cole_fread", colerrno, objectFileName);
        cole_fclose(coleFile, &colerrno);
        cole_umount(cfs, NULL);
        free(nativeStream);
        return 1;
    }

    if (cole_fclose(coleFile, &colerrno) != 0) {
        cole_perror("DecodeOLE cole_fclose", colerrno, objectFileName);
        cole_umount(cfs, NULL);
        free(nativeStream);
        return 1;
    }

    if (cole_umount(cfs, &colerrno)) {
        cole_perror("DecodeOLE cole_umount", colerrno, objectFileName);
        free(nativeStream);
        return (1);
    }
    return 0;
}


/* 
 * Save the hex-encoded object data and as binary bytes in objectFileName 
 */
static void ReadObjectData(char *objectFileName, int type, int offset)
{
    char dummyBuf[20];
    char *OLE_MARK = "d0cf11e0";
    FILE *objFile;
    int i;
    uint8_t hexNumber;
    uint8_t hexEvenOdd = 0;       /* should be even at the end */
/*     char *fn = "ReadObjectData"; */
/*     RTFMsg("%s: * starting ...\n", fn); */

    if (type == EquationClass) {
        (oleEquation.count)++;
        snprintf(dummyBuf, 20, "Eq%03d.eqn", oleEquation.count);
    } else
        snprintf(dummyBuf, 20, ".obj");

    /* construct full path of file name */
    strcpy(objectFileName, RTFGetInputName());
    strcat(objectFileName, dummyBuf);

    /* open object file */
    objFile = fopen(objectFileName, "wb");
    if (!objFile)
        RTFPanic("Cannot open input file %s\n", objectFileName);

/* skip offset header (2 hex characters for each byte) 
   for (i = 0; i < offset * 2; i++)
       RTFGetToken();
*/

/* 
 * The offset to the data should be a constant, but it seems to
 * vary from one RTF file to the next.  Perhaps there is some 
 * Microsoft documentation that explains how many bytes exactly
 * there is before we get to the chewy nuguat.  After adding
 * several hacks to shift and squirm, the simplest thing is just
 * to skip to a sequence of bytes that should start each OLE
 * object.  This is what is done now.
 */
       
    i = 0;
    while (i < 8) {
        RTFGetToken();
        if (rtfTextBuf[0] == OLE_MARK[i])
            i++;
        else if (rtfTextBuf[0] == OLE_MARK[0])
            i = 1;
        else
            i = 0;
    }
    fputc(0xd0, objFile);
    fputc(0xcf, objFile);
    fputc(0x11, objFile);
    fputc(0xe0, objFile);
        

    /* each byte is encoded as two hex chars ... ff, a1, 4c, ...*/
    while (1) {
        RTFGetToken();

        /* CR or LF in the hex stream should be skipped */
        if (rtfTextBuf[0] == 0x0a || rtfTextBuf[0] == 0x0d)
            RTFGetToken();

        if (rtfClass == rtfGroup)
            break;

        hexNumber = 16 * RTFCharToHex(rtfTextBuf[0]);
        hexEvenOdd++;

        RTFGetToken();

        if (rtfTextBuf[0] == 0x0a || rtfTextBuf[0] == 0x0d)
            RTFGetToken();  /* should not happen */

        if (rtfClass == rtfGroup)
            break;

        hexNumber += RTFCharToHex(rtfTextBuf[0]);   /* this is the the number */
        hexEvenOdd--;
        fputc(hexNumber, objFile);
    }

    if (fclose(objFile) != 0)
        printf("* error closing object file %s\n", objectFileName);

    if (hexEvenOdd)
        printf ("* Warning! Odd number of hex characters read for object!\n");
}

/* 
 * Convert OLE file containing equation 
 */
 
boolean ConvertEquationFile(char *objectFileName)
{
    unsigned char *nativeStream;
    MTEquation *theEquation;
    uint32_t equationSize;   

    nativeStream = NULL;
    theEquation = NULL;
        
    /* Decode the OLE and extract the equation stream into buffer nativeStream */
    if (DecodeOLE(objectFileName, "/Equation Native", &nativeStream, &equationSize)) {
        RTFMsg("* error decoding OLE equation object!\n");
        return (false);
    }    

    theEquation = (MTEquation *) malloc(sizeof(MTEquation));
    if (theEquation == NULL) {
        RTFMsg("* error allocating memory for equation!\n");
        free(nativeStream);
        return (false);
    }

   /* __cole_dump(nativeStream+slop, nativeStream+slop, 64, NULL); */
    if (*(nativeStream) == 0x1c && *(nativeStream+1) == 0x00) {
        equationSize -= MTEF_HEADER_SIZE;

        if (!Eqn_Create(theEquation, nativeStream+MTEF_HEADER_SIZE, equationSize)) {
            RTFMsg("* could not create equation structure!\n");
            free(nativeStream);
            free(theEquation);
            return (false);
        }

        if (lastCharWasLineBreak) 
            theEquation->m_inline = 0;
        else
            theEquation->m_inline = 1;
            
        DoParagraphCleanUp();
        DoSectionCleanUp();
        
        if (g_insert_eqn_name) {
            PutLitStr("\\url{file://");
            PutLitStr(objectFileName);
            PutLitStr("}");
            wrapCount += strlen(objectFileName) + strlen("\\url{file://}");
            requireHyperrefPackage = true;
        }

        Eqn_TranslateObjectList(theEquation, ostream, 0);
        Eqn_Destroy(theEquation);
    }

    if (theEquation != NULL)
        free(theEquation);

    if (nativeStream != NULL)
        free(nativeStream);

    requireAmsSymbPackage = true;
    requireAmsMathPackage = true;
    return true;
}

/* 
 * Translate an object containing a MathType equation
 */
static boolean ReadEquation(int *groupCount)
{
    FILE *in, *out;
    boolean result;
    char objectFileName[rtfBufSiz];
    char objectFileNameX[rtfBufSiz];

    /* look for start of \objdata  group */
    while (!RTFCheckMM(rtfDestination, rtfObjData)) {

        RTFGetToken();

        if (RTFCheckCM(rtfGroup, rtfBeginGroup) != 0)
            (*groupCount)++;

        else if (RTFCheckCM(rtfGroup, rtfEndGroup) != 0) {
            (*groupCount)--;
            if (*groupCount == 0) {
                RTFMsg("* ReadEquation: objdata group not found!\n");
                return (false);
            }

        } else if (rtfClass == rtfEOF) {
            RTFPanic("* ReadEquation: EOF reached!\n");
            exit(1);
        }
    }

    /* save hex-encoded object data a binary objectFileName */
    ReadObjectData(objectFileName, EquationClass, EQUATION_OFFSET);
    (*groupCount)--;

    /* hack because sometimes EQUATION_OFFSET needs to be 4 bytes longer */
    in = fopen(objectFileName, "r");
    if (fgetc(in) == 0xd0) 
        fclose(in);
    else {
        strcpy(objectFileNameX,objectFileName);
        objectFileNameX[strlen(objectFileName)-1] = 'x';
        fgetc(in); 
        fgetc(in); 
        fgetc(in);
        out = fopen(objectFileNameX, "wb");

        while (!feof(in))
            fputc(fgetc(in), out);
        fclose(out);
        fclose(in);
        rename(objectFileNameX,objectFileName);
    } 

    result = ConvertEquationFile(objectFileName);
    
    if (g_delete_eqn_file)
        remove(objectFileName);
        
    return result;
}


/* 
 * Read and process \object token 
 */
static void ReadObject(void)
{
    int i;
    int groupCounter = 1;       /* one opening brace has been counted */
    int temp;
    boolean res;
    /* char *fn = "ReadObject"; */
    /*  RTFMsg("%s: * starting ...\n", fn); */

    GetObjectClass(&groupCounter);

    switch (object.class) {
    case unknownObjClass:
    default:
        /* RTFMsg("%s: * unsupported object '%s', skipping...\n", fn, object.className); */
        RTFSkipGroup();
        break;

    case EquationClass:
        /* RTFMsg("%s: * equation object '%s', processing...\n", fn, object.className); */

        if ((int) preferenceValue[GetPreferenceNum("convertEquations")])
            res = ReadEquation(&groupCounter);
        else
            res = false;

        /* if unsuccessful, include the equation as a picture */
        if (!res || g_include_both) {
            temp = groupCounter;

            while (!RTFCheckMM(rtfDestination, rtfPict)) {
                RTFGetToken();
                if (RTFCheckCM(rtfGroup, rtfBeginGroup))
                    groupCounter++;
                if (RTFCheckCM(rtfGroup, rtfEndGroup))
                    groupCounter--;
                if (groupCounter < temp)
                    break;
            }

            if (groupCounter > temp) {
                ReadPicture();
                if (groupCounter - 1 - temp > 0) {
                    for (i = 0; i < groupCounter - 1 - temp; i++)
                        RTFSkipGroup();
                }
            }
            groupCounter = temp;
        }
        break;

    case WordPictureClass:
    case MSGraphChartClass:
        while (!ReachedResult(&groupCounter));
        ReadPicture();
        break;
    }

    object.class = 0;
    strcpy(object.className, "");

    /* if there are open groups left, close them */
    if (groupCounter != 0) {
        for (i = 0; i < groupCounter; i++)
            RTFSkipGroup();
    }

    /* send the last closing brace back into the router */
    RTFRouteToken();
}


/* This is the result field of the Word97 object */
static void ReadWord97Result(void)
{
    int i;
    int groupCount = 1;         /* one opening brace has been counted */
/*     char *fn = "ReadWord97Result"; */
/*     RTFMsg("%s: starting ...\n",fn); */

    /* scan until object or picture is reached */
    while (groupCount != 0) {
        RTFGetToken();
        if (RTFCheckMM(rtfDestination, rtfObject) != 0) {
            ReadObject();
            groupCount--;
            break;
        } else if (RTFCheckMM(rtfDestination, rtfPict) != 0) {
            ReadPicture();
            groupCount--;
            break;
        } else if (RTFCheckCM(rtfGroup, rtfBeginGroup) != 0)
            groupCount++;
        else if (RTFCheckCM(rtfGroup, rtfEndGroup) != 0)
            groupCount--;
    }

    if (groupCount == 0)
        printf
            ("* Warning: no supported structure in Word97 object found.\n");


    /* if there are open groups left, skip to the end */
    if (groupCount > 0)
        for (i = 0; i < groupCount; i++)
            RTFSkipGroup();

    RTFRouteToken();

}


/* Of course, Word97 has to do everything differently. */
static void ReadWord97Object(void)
{
    int i;
    long objectStart;
    int groupCount = 1;         /* one opening brace has been counted */
    int word97ObjTextGL = 1;
    short prevChar;
/*     char *fn = "ReadWord97Object"; */
/*     RTFMsg("%s: starting ...\n",fn); */

    word97ObjectType = unknownWord97Object;

    /* look for a standard embedded object first: may have an equation */
    /* mark the current cursor position */
    prevChar = RTFPushedChar();
    RTFStoreStack();
    objectStart = ftell(ifp);

    while (!RTFCheckMM(rtfDestination, rtfObject)) {
        RTFGetToken();

        if (RTFCheckCM(rtfGroup, rtfBeginGroup) != 0)
            groupCount++;

        else if (RTFCheckCM(rtfGroup, rtfEndGroup) != 0) {
            groupCount--;
            /* did not find a standard object */
            if (groupCount == 0) {
                fseek(ifp, objectStart, 0);
                RTFSimpleInit();
                RTFSetPushedChar(prevChar);
                RTFRestoreStack();
                break;
            }
        }

    }

    /* if we found a standard object, read it and get out */
    if (RTFCheckMM(rtfDestination, rtfObject)) {
        ReadObject();
        groupCount--;
        /* if there are open groups left, close them */
        if (groupCount != 0)
            for (i = 0; i < groupCount; i++)
                RTFSkipGroup();
        /* send the last closing brace back into the router */
        RTFRouteToken();
        return;
    }

    groupCount = 1;

    while (!ReachedResult(&groupCount)) {
        if (groupCount == 0) {
            RTFMsg("* unknown Word97 object...\n");
/*             PutLitStr(" [ missing object here ] "); */
/*             wrapCount += 25; */
            RTFRouteToken();
            return;
        }
    }

    switch (word97ObjectType) {
    case word97Picture:
        ReadPicture();
        break;
    case standardObject:
    case word97Object:
        ReadWord97Result();
        break;
    case word97ObjText:
        if (!table.inside) {
            SetGroupLevels(SAVE_LEVELS);
            for (i = 0; i < charAttrCount; i++)
                PutLitStr("}");
            charAttrCount = 0;
            if (blankLineCount < MAX_BLANK_LINES) {
                InsertNewLine();
                InsertNewLine();
                blankLineCount++;
            }
            wrapCount = 0;
            word97ObjTextGL = groupLevel;
            while (groupLevel >= word97ObjTextGL) {
                suppressLineBreak = true;
                RTFGetToken();
                RTFRouteToken();
            }
            SetGroupLevels(RESTORE_LEVELS);
            if (blankLineCount < MAX_BLANK_LINES) {
                InsertNewLine();
                InsertNewLine();
                blankLineCount++;
            }
            wrapCount = 0;
        }
        break;
    }

    if (groupCount == 0)
        return;

    /* if there are open groups left, close them */
    if (groupCount != 0)
        for (i = 0; i < groupCount; i++)
            RTFSkipGroup();

    object.word97 = 0;

    /* send the last closing brace back into the router */
    RTFRouteToken();
}


/* the following streams should just emit ... HToc268803753
 *    PAGEREF _Toc268803753 \\h 
 *    HYPERLINK \\l "_Toc268803753"
 *     _Toc268803753
 */
static void emitBookmark(void)
{
    int started = 0;

    RTFGetToken();
    while (rtfClass == rtfText) {
        switch (rtfTextBuf[0]) {
        case ' ':
            if (started) {     /* assume that bookmarks optionally start and end with spaces */
                RTFSkipGroup();
                return;
            }
            break;
        case '"':
            break;
        case '\\':
            RTFGetToken(); /* drop backslash and the next letter */
            break;
        case '_':
            started = 1;
            PutLitStr("H");
            break;
        default:
            started = 1;
            PutLitStr(rtfTextBuf);
            break;
        }
        RTFGetToken();
    }
}

static void ReadUnicode(void)
{
    char unitext[20];

    if (rtfParam == 8220) {
        PutLitStr("``");
        wrapCount+=2;
        RTFGetToken();
        return;
    }

    if (rtfParam == 8221) {
        PutLitStr("''");
        wrapCount+=2;
        RTFGetToken();
        return;
    }

    if (rtfParam == 8230) {
        PutLitStr("...");
        wrapCount+=3;
        RTFGetToken();
        return;
    }

    if (rtfParam<0) 
        rtfParam += 65536;

    snprintf(unitext,20,"\\unichar{%d}",rtfParam);
    if (0) fprintf(stderr,"unicode --- %s!\n",unitext);
    PutLitStr(unitext);
    wrapCount += (uint32_t) strlen(unitext);
    requireUnicodePackage = true;
    RTFGetToken();
}

static void ReadHyperlink(void)
{
    int localGL;

    PutLitStr("\\href{");
    wrapCount += 5;

    localGL = groupLevel;

    insideHyperlink = true;

    while (groupLevel >= localGL) {
        RTFGetToken();
        if (rtfClass == rtfText) {
            if (rtfTextBuf[0] != '"'
                && !RTFCheckMM(rtfSpecialChar, rtfLDblQuote)
                && !RTFCheckMM(rtfSpecialChar, rtfRDblQuote))
                    RTFRouteToken();
         } else 
             RTFRouteToken();
    }

    PutLitStr("}{");
    wrapCount += 2;

    /* skip over to the result group */
    while (!RTFCheckCMM(rtfControl, rtfDestination, rtfFieldResult))
        RTFGetToken();

    localGL = groupLevel;
    /* switch off hyperlink flag */
    insideHyperlink = false;


    while (groupLevel >= localGL) {
        RTFGetToken();
        if (RTFCheckCMM(rtfControl, rtfSpecialChar, rtfOptDest))
            RTFSkipGroup();
        textStyle.newStyle = false;
        /*if (rtfClass == rtfText)*/
            RTFRouteToken();
    }

    PutLitStr("}");
    wrapCount++;
    requireHyperrefPackage = true;
}

static void ReadSymbolField(void)
{
    char buf[100];
    short major, minor;
    short currentCharSet = RTFGetCharSet();

    RTFSetCharSet(rtfCSSymbol);

    /* go to the start of the symbol representation */
    strcpy(buf, "");
    if (RTFGetToken() != rtfText) {
        if (RTFCheckCM(rtfGroup, rtfBeginGroup) != 0)
            RTFSkipGroup();
        RTFSkipGroup();
        RTFRouteToken();
        return;
    }

    /* read in the symbol token */
    strcat(buf, rtfTextBuf);
    while (strcmp(rtfTextBuf, " ") != 0) {
        RTFGetToken();
        if (strcmp(rtfTextBuf, " ") != 0)
            strcat(buf, rtfTextBuf);
    }

    /* convert the text symbol token to an int */
    major = atoi(buf);
    /* do the mapping */
    minor = RTFMapChar(major);

    /* set the rtf token to the new value */
    RTFSetToken(rtfText, major, minor, rtfNoParam, buf);

    if (minor >= rtfSC_therefore && minor < rtfSC_currency)
        requireAmsSymbPackage = true;

    /* call the handler for text */
    TextClass();

    /* reset our character set */
    RTFSetCharSet(currentCharSet);
    RTFSkipGroup();
    RTFRouteToken();
}

/*
 *  Just emit \pageref{HToc268612944} for {\*\fldinst {...  PAGEREF _Toc268612944 \\h } ..}
*/
static void ReadPageRefField(void)
{
/*    char *fn = "ReadPageRefField";
    RTFMsg("%s: starting ...\n",fn); */

    PutLitStr("\\pageref{");
    wrapCount += 8;
    emitBookmark();
    PutLitStr("}");
    wrapCount += 1;
    RTFRouteToken();

    /* skip over to the result group */
    while (!RTFCheckCMM(rtfControl, rtfDestination, rtfFieldResult))
        RTFGetToken();

    RTFSkipGroup();
    RTFRouteToken();
}

/*
 *  Three possible types of fields
 *     (1) supported ... translate and ignore FieldResult
 *     (2) tolerated ... ignore FieldInst and translate FieldResult
 *     (3) unknown   ... ignore both FieldInst and FieldResult
 */
static void ReadFieldInst(void)
{
    char buf[100];
    int i;
    int groupCount = 1;
/*    char *fn = "ReadFieldInst";
    RTFMsg("%s: starting ... \n",fn);*/

    strcpy(buf, "");

    /* skip to text identifying the type of FIELD  */
    while (rtfClass != rtfText || rtfMinor == rtfSC_space) {
        RTFGetToken();
        if (RTFCheckCM(rtfGroup, rtfBeginGroup))
            groupCount++;
        if (RTFCheckCM(rtfGroup, rtfEndGroup))
            groupCount--;
        if (groupCount == 0) {
            RTFRouteToken();
            return;
        }
    }

    /* extract text identifying the FIELD into buf */
    strcat(buf, rtfTextBuf);
    while (strcmp(rtfTextBuf, " ") != 0 && rtfClass != rtfGroup) {
        RTFGetToken();
        if (strcmp(rtfTextBuf, " ") != 0 && rtfClass != rtfGroup)
            strcat(buf, rtfTextBuf);
    }
/*    RTFMsg("%s: FIELD type is %s\n",fn,buf);*/

    if (0 && strcmp(buf, "HYPERLINK") == 0 )
        if (!(int) preferenceValue[GetPreferenceNum("ignoreHypertext")]) {
            ReadHyperlink();
            return;
        }

    if (strcmp(buf, "SYMBOL") == 0) {
        ReadSymbolField();
        return;
    }

    if (strcmp(buf, "PAGEREF") == 0) {
        ReadPageRefField();
        return;
    }

    /* Unsupported FIELD type ... the best we can do is bail from rtfFieldInst
       and hope rtfFieldResult can be processed  */
    for (i = 0; i < groupCount; i++)
        RTFSkipGroup();
}

/*
 *  Just emit \label{HToc268612944} for {\*\bkmkstart _Toc268612944}
 */
static void ReadBookmarkStart(void)
{
    PutLitStr("\\label{");
    wrapCount += 7;
    emitBookmark();
    PutLitStr("}");
    wrapCount += 1;
    RTFRouteToken();
}


/*
 * Prepares output TeX file for each input RTF file. 
 * Sets globals and installs callbacks.
 */
int BeginLaTeXFile(void)
{
    /* set some globals */

    RTFSetDefaultFont(-1);
    codePage = 0;
    wrapCount = 0;
    charAttrCount = 0;
    groupLevel = 0;
    paragraph.alignment = left;
    textStyle.newStyle = 0;
    lastCharWasLineBreak = true;
    seenLeftDoubleQuotes = false;
    wroteBeginDocument = false;
    spaceCount = 0;
    blankLineCount = 0;
    suppressLineBreak = true;
    continueTextStyle = false;
    writingHeading1 = false;
    writingHeading2 = false;
    writingHeading3 = false;
    insideFootnote = false;
    justWroteFootnote = false;
    insideHyperlink = false;
    paragraph.lineSpacing = singleSpace;
    paragraph.newStyle = false;
    paragraph.parbox = false;
    paragraph.wroteAlignment = false;
    paragraph.wroteSpacing = false;
    section.newStyle = false;
    section.cols = 1;
    dblQuoteLeft = false;
    lineIsBlank = true;

    if (preferenceValue[GetPreferenceNum("ignoreColor")])
        requireColorPackage = false;
    else
        requireColorPackage = true;
    requireSetspacePackage = false;
    requireTablePackage = false;
    requireGraphicxPackage = false;
    requireAmsSymbPackage = false;
    requireMultiColPackage = false;
    requireUlemPackage = false;
    requireHyperrefPackage = false;
    requireMultirowPackage = false;
    requireAmsMathPackage = false;
    requireUnicodePackage = false;


    picture.count = 0;
    picture.type = unknownPict;
    oleEquation.count = 0;
    object.class = unknownObjClass;
    object.word97 = 0;
    table.inside = false;
    table.cellCount = 0;
    table.cellInfo = (cell *) NULL;
    table.cellMergePar = none;
    table.multiCol = false;
    table.multiRow = false;
    InitializeTextStyle();


    /* install class callbacks */
    RTFSetClassCallback(rtfText, TextClass);
    RTFSetClassCallback(rtfControl, ControlClass);

    /* install destination callbacks */
    RTFSetDestinationCallback(rtfUnicode, ReadUnicode);
    RTFSetDestinationCallback(rtfObjWid, ReadObjWidth);
    RTFSetDestinationCallback(rtfColorTbl, WriteColors);
    RTFSetDestinationCallback(rtfParNumTextAfter, SkipGroup);
    RTFSetDestinationCallback(rtfParNumTextBefore, SkipGroup);
    RTFSetDestinationCallback(rtfFieldInst, ReadFieldInst);
    RTFSetDestinationCallback(rtfObject, ReadObject);
    RTFSetDestinationCallback(rtfWord97Object, ReadWord97Object);
    RTFSetDestinationCallback(rtfWord97NoPicture, SkipGroup);
    RTFSetDestinationCallback(rtfRevisionTbl, SkipGroup);
    RTFSetDestinationCallback(rtfPict, ReadPicture);
    RTFSetDestinationCallback(rtfFootnote, ReadFootnote);
    RTFSetDestinationCallback(rtfBookmarkStart, ReadBookmarkStart);
    RTFSetDestinationCallback(rtfBookmarkEnd, SkipGroup);
    RTFSetDestinationCallback(rtfDataField, SkipGroup);
    RTFSetDestinationCallback(rtfTemplate, SkipGroup);
    RTFSetDestinationCallback(rtfDocvar, SkipGroup);
    RTFSetDestinationCallback(rtfFchars, SkipGroup);
    RTFSetDestinationCallback(rtfLchars, SkipGroup);
    RTFSetDestinationCallback(rtfPgdsctbl, SkipGroup);


    /* use r2l-map if present */
    /* defaults */
    documentclassString = "{article}";
    boldString = "\\textbf{";
    noBoldString = "\\textmd{";
    italicString = "\\textit{";
    noItalicString = "\\textup{";
    underlineString = "{\\underline {";
    smallcapsString = "\\textsc{";
    heading1String = "\\section*{";
    heading2String = "\\subsection*{";
    heading3String = "\\subsubsection*{";
    tableString = "longtable";

    if (r2lMapPresent) {
        itemNumber = R2LItem("documentclass");
        if (r2lMap[itemNumber] != (char *) NULL)
            documentclassString = r2lMap[itemNumber];
        itemNumber = R2LItem("bold");
        if (r2lMap[itemNumber] != (char *) NULL)
            boldString = r2lMap[itemNumber];
        itemNumber = R2LItem("nobold");
        if (r2lMap[itemNumber] != (char *) NULL)
            noBoldString = r2lMap[itemNumber];
        itemNumber = R2LItem("italic");
        if (r2lMap[itemNumber] != (char *) NULL)
            italicString = r2lMap[itemNumber];
        itemNumber = R2LItem("noitalic");
        if (r2lMap[itemNumber] != (char *) NULL)
            noItalicString = r2lMap[itemNumber];
        itemNumber = R2LItem("underline");
        if (r2lMap[itemNumber] != (char *) NULL)
            underlineString = r2lMap[itemNumber];
        itemNumber = R2LItem("smallcaps");
        if (r2lMap[itemNumber] != (char *) NULL)
            smallcapsString = r2lMap[itemNumber];
        itemNumber = R2LItem("heading1");
        if (r2lMap[itemNumber] != (char *) NULL)
            heading1String = r2lMap[itemNumber];
        itemNumber = R2LItem("heading2");
        if (r2lMap[itemNumber] != (char *) NULL)
            heading2String = r2lMap[itemNumber];
        itemNumber = R2LItem("heading3");
        if (r2lMap[itemNumber] != (char *) NULL)
            heading3String = r2lMap[itemNumber];
        itemNumber = R2LItem("table");
        if (r2lMap[itemNumber] != (char *) NULL)
            tableString = r2lMap[itemNumber];
    }

    /* write LaTeX header */
    WriteLaTeXHeader();

    return (1);
}

/* this is a diagnostic function that I call when I get into trouble 
 * with text styles.  Unused
 */
 /*
    static void ExamineTextStyle (void)
    {
    RTFMsg ("* GL is %d\n", groupLevel);
    RTFMsg ("* charAttrCount is %d\n", charAttrCount);
    RTFMsg ("* Bgl is %d\n", textStyle.boldGroupLevel);
    RTFMsg ("* TBgl is %d\n", textStyle.noBoldGroupLevel);
    RTFMsg ("* Igl is %d\n", textStyle.italicGroupLevel);
    RTFMsg ("* TIgl is %d\n", textStyle.noItalicGroupLevel);
    RTFMsg ("* Ugl is %d\n", textStyle.underlinedGroupLevel);
    RTFMsg ("* TUgl is %d\n", textStyle.noUnderlinedGroupLevel);
    RTFMsg ("* FCgl is %d\n", textStyle.foreColorGroupLevel);
    RTFMsg ("* BCgl is %d\n", textStyle.backColorGroupLevel);
    RTFMsg ("* Sbgl is %d\n", textStyle.subScriptGroupLevel);
    RTFMsg ("* TSbgl is %d\n", textStyle.noSubScriptGroupLevel);
    RTFMsg ("* Spgl is %d\n", textStyle.superScriptGroupLevel);
    RTFMsg ("* TSpgl is %d\n", textStyle.noSuperScriptGroupLevel);
    RTFMsg ("* Fsgl is %d\n\n", textStyle.fontSizeGroupLevel);
    }
  */

/* This function scans the bounding box information for an EPS file 
   Currently unused
   */
/*
static int ScanBoundingBox (char *epsFile)
{
FILE *fp;
char    buf[50];


        if ((fp = fopen (epsFile, "r")) == (FILE *) NULL)
                        {
                                RTFMsg ("* can't open eps file %s\n", epsFile);
                                return (1);
                        }

        while (fscanf (fp, "%s", buf) != EOF)
        {
                if (strcmp (buf, "%%BoundingBox:") == 0)
                {
                        fscanf (fp, "%d", &(picture.llx));
                        fscanf (fp, "%d", &(picture.lly));
                        fscanf (fp, "%d", &(picture.urx));
                        fscanf (fp, "%d", &(picture.ury));
                        if (fclose(fp) != 0) 
            {
                printf("* error closing eps file %s\n", epsFile);
                            return (1);
            }
            return (0);
                }
        }

        if (fclose(fp) != 0) 
    {
        printf("* error closing eps file %s\n", epsFile);
        return (1);
    }
        return (0);
}
*/
