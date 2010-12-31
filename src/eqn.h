#ifndef _EQN_H

#define _EQN_H

# define DISPLAY_EQUATION 0
# define INLINE_EQUATION 1

typedef struct {
    int mathattr;
    int do_lookup;
    int use_codepoint;
} MT_CHARSET_ATTS;

typedef struct {
    struct MT_OBJLIST *next;
    int tag;
    void *obj_ptr;
} MT_OBJLIST;

typedef struct {
    MT_OBJLIST *o_list;
    FILE *out_file;

    char indent[128];
    int log_level;
    
    MT_CHARSET_ATTS *atts_table;
    char **m_atts_table;
    char **m_char_table;

    int m_mtef_ver;
    int m_platform;
    int m_product;
    int m_version;
    int m_version_sub;
    int m_inline;
    int m_mode;
} MTEquation;

int Eqn_Create(MTEquation * eqn, unsigned char *eqn_stream, int eqn_size);
void Eqn_Destroy(MTEquation * eqn);
void Eqn_TranslateObjectList(MTEquation * eqn, FILE * outfile, int loglevel);

#endif