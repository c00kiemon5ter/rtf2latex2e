/* MathType Equation converter, see comments in C file */

#define embDOT        2
#define embDDOT       3
#define embTDOT       4
#define embPRIME      5
#define embDPRIME     6
#define embBPRIME     7
#define embTILDE      8
#define embHAT        9
#define embNOT        10
#define embRARROW     11
#define embLARROW     12
#define embBARROW     13
#define embR1ARROW    14
#define embL1ARROW    15
#define embMBAR       16
#define embOBAR       17
#define embTPRIME     18
#define embFROWN      19
#define embSMILE      20

// embellishment object
typedef struct {
    struct MT_EMBELL *next;
    int nudge_x;
    int nudge_y;
    int embell;
} MT_EMBELL;

#define MT_LEFT       0
#define MT_CENTER     1
#define MT_RIGHT      2
#define MT_OPERATOR   3
#define MT_DECIMAL    4

#define MT_PILE_LEFT       1
#define MT_PILE_CENTER     2
#define MT_PILE_RIGHT      3
#define MT_PILE_OPERATOR   4
#define MT_PILE_DECIMAL    5

#define TAB_LEFT      0
#define TAB_CENTER    1
#define TAB_RIGHT     2
#define TAB_EQUAL     3
#define TAB_DECIMAL   4

// tabstop object
typedef struct {
    struct MT_TABSTOP *next;
    int type;
    int offset;
} MT_TABSTOP;

#define END     0
#define LINE    1
#define CHAR    2
#define TMPL    3
#define PILE    4
#define MATRIX  5
#define EMBELL  6
#define RULER   7
#define FONT    8
#define SIZE    9
#define FULL    10
#define SUB     11
#define SUB2    12
#define SYM     13
#define SUBSYM  14
#define COLOR 	15

#define COLOR_DEF 	 16
#define FONT_DEF 	 17
#define EQN_PREFS 	 18
#define ENCODING_DEF 19

#define xfLMOVE     0x08

#define xfAUTO      0x01
#define xfEMBELL    0x02

#define xfNULL      0x01
#define xfRULER     0x02
#define xfLSPACE    0x04


// ruler object
typedef struct {
    int n_stops;
    MT_TABSTOP *tabstop_list;
} MT_RULER;


// line (of math) object
typedef struct {
    int nudge_x;
    int nudge_y;
    int line_spacing;
    MT_RULER *ruler;
    MT_OBJLIST *object_list;
} MT_LINE;


// character object
typedef struct {
    int nudge_x;
    int nudge_y;
    int atts;
    int typeface;
    uint32_t character;
    MT_EMBELL *embellishment_list;
} MT_CHAR;

// template object
typedef struct {
    int nudge_x;
    int nudge_y;
    int selector;
    int variation;
    int options;
    MT_OBJLIST *subobject_list;
} MT_TMPL;

#define PHA_LEFT      1
#define PHA_CENTER    2
#define PHA_RIGHT     3
#define PHA_RELOP     4
#define PHA_DECIMAL   5

#define PVA_TOP       0
#define PVA_CENTER    1
#define PVA_BOTTOM    2
#define PVA_CENTERING 3

// pile object
typedef struct {
    int nudge_x;
    int nudge_y;
    int halign;
    int valign;
    MT_RULER *ruler;
    MT_OBJLIST *line_list;
} MT_PILE;

#define MATR_MAX  16

// matrix object
typedef struct {
    int nudge_x;
    int nudge_y;
    int valign;
    int h_just;
    int v_just;
    int rows;
    int cols;
    unsigned char row_parts[MATR_MAX];
    unsigned char col_parts[MATR_MAX];
    MT_OBJLIST *element_list;
} MT_MATRIX;


// font object
typedef struct {
    int tface;
    int style;
    char *zname;
} MT_FONT;

#define szFULL          0
#define szSUB           1
#define szSUB2          2
#define szSYM           3
#define szSUBSYM        4
#define szUSER1         5
#define szUSER2         6

// size object
typedef struct {
    int type;
    int lsize;
    int dsize;
} MT_SIZE;


// our equation object
typedef struct {
    int log_level;
    int do_delete;
    int ilk;
    int is_line;
    char *data;
} EQ_STRREC;

static MT_OBJLIST *Eqn_GetObjectList(MTEquation * eqn,
                                     unsigned char *eqn_stream, int *index,
                                     int count);
static MT_LINE *Eqn_inputLINE(MTEquation * eqn, unsigned char *src,
                              int *delta);
static MT_CHAR *Eqn_inputCHAR(MTEquation * eqn, unsigned char *src,
                              int *delta);
static MT_TMPL *Eqn_inputTMPL(MTEquation * eqn, unsigned char *src,
                              int *delta);
static MT_PILE *Eqn_inputPILE(MTEquation * eqn, unsigned char *src,
                              int *delta);
static MT_MATRIX *Eqn_inputMATRIX(MTEquation * eqn, unsigned char *src,
                                  int *delta);
static MT_EMBELL *Eqn_inputEMBELL(MTEquation * eqn, unsigned char *src,
                                  int *delta);
static MT_RULER *Eqn_inputRULER(MTEquation * eqn, unsigned char *src,
                                int *delta);
static MT_FONT *Eqn_inputFONT(MTEquation * eqn, unsigned char *src,
                              int *delta);
static MT_SIZE *Eqn_inputSIZE(MTEquation * eqn, unsigned char *src,
                              int *delta);

static int GetNudge(unsigned char *src, int *x, int *y);

static void DeleteObjectList(MT_OBJLIST * the_list);
static void DeleteTabstops(MT_TABSTOP * the_list);
static void DeleteEmbells(MT_EMBELL * the_list);

static char *Eqn_TranslateObjects(MTEquation * eqn, MT_OBJLIST * the_list);
static char *Eqn_TranslateLINE(MTEquation * eqn, MT_LINE * line);
static char *Eqn_TranslateFUNCTION(MTEquation * eqn,
                                   MT_OBJLIST * curr_node, int *advance);
static char *Eqn_TranslateTEXTRUN(MTEquation * eqn, MT_OBJLIST * curr_node,
                                  int *advance);
static char *Eqn_TranslateCHAR(MTEquation * eqn, MT_CHAR * thechar);
static char *Eqn_TranslateTMPL(MTEquation * eqn, MT_TMPL * tmpl);
static char *Eqn_TranslateLINE(MTEquation * eqn, MT_LINE * line);
static char *Eqn_TranslatePILE(MTEquation * eqn, MT_PILE * pile);
static char *Eqn_TranslateMATRIX(MTEquation * eqn, MT_MATRIX * matrix);
static char *Eqn_TranslateFONT(MTEquation * eqn, MT_FONT * font);
static char *Eqn_TranslateRULER(MTEquation * eqn, MT_RULER * ruler);
static char *Eqn_TranslateSIZE(MTEquation * eqn, MT_SIZE * size);
static char *Eqn_TranslateEQNARRAY(MTEquation * eqn, MT_PILE * pile);

static int Eqn_GetTmplStr(MTEquation * eqn, int selector, int variation,
                          EQ_STRREC * strs);
static int Eqn_GetTexChar(MTEquation * eqn, EQ_STRREC * strs,
                          MT_CHAR * thechar, int *math_attr);
static void Eqn_LoadCharSetAtts(MTEquation * eqn, char **table);
static void GetPileType(char *the_template, int arg_num, char *targ_nom);

static int GetProfileStr(char **section, char *key, char *data,
                         int datalen);

static void BreakTeX(char *ztex, FILE * outfile);
static char *ToBuffer(char *src, char *buffer, int *off, int *lim);
static void SetComment(EQ_STRREC * strs, int lev, char *src);
static void SetDollar(EQ_STRREC * strs, int turn_on);
static char *Eqn_JoinStrings(MTEquation * eqn, EQ_STRREC * strs, int num_strs);


#define NUM_TYPEFACE_SLOTS    32

#define Z_TEX         1
#define Z_COMMENT     2
#define Z_TMPL        3

#define MA_NONE         0
#define MA_FORCE_MATH   1
#define MA_FORCE_TEXT   2

#define CHAR_EMBELL         0x01
#define CHAR_FUNC_START     0x02
#define CHAR_ENC_CHAR_8     0x04
#define CHAR_NUDGE          0x08
#define CHAR_ENC_CHAR_16    0x10
#define CHAR_ENC_NO_MTCODE  0x20

static unsigned char HiNibble(unsigned char x);
static unsigned char LoNibble(unsigned char x);
static void PrintNibble(unsigned char n);
static void PrintNibbleDimension(unsigned char n);
static int SkipNibbles(unsigned char *p, int num);
