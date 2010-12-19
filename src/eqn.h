/* MathType Equation converter, see comments in C file */

//#include <stdio.h>

typedef struct {
    int mathattr;
    int do_lookup;
    int use_codepoint;
} CHARSETatts;

// object used to hold MT elements
typedef struct {
    struct MT_OBJLIST *next;
    int tag;
    void *obj_ptr;
} MT_OBJLIST;

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
#define xfAUTO      0x10
#define xfEMBELL    0x20
#define xfNULL      0x10
#define xfLSPACE    0x40
#define xfRULER     0x20


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
    long character;
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
    MT_OBJLIST *o_list;
    FILE *out_file;

    char indent[128];
    int log_level;
    int math_mode;

    int text_mode;
    CHARSETatts *atts_table;
    char **m_atts_table;
    char **m_char_table;

    int m_mtef_ver;
    int m_platform;
    int m_product;
    int m_version;
    int m_version_sub;
    int m_inline;
} MTEquation;


boolean Eqn_Create(MTEquation * eqn, unsigned char *eqn_stream,
		   int eqn_size);
void Eqn_Destroy(MTEquation * eqn);
void Eqn_TranslateObjectList(MTEquation * eqn, FILE * outfile,
			     int loglevel);
