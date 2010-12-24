/* This header file contains code to convert graphic file formats found in RTF to EPS.
   At present, only jpeg files are converted into EPS. I will add other file formats 
   as soon as I figure out how. The jpeg->eps conversion routine was adapted from the
   jpeg2ps code by Thomas Merz with his permission. jpeg2ps can be found at
   http://www.ifconnection.de/~tm
*/


/* data output mode: binary, ascii85, hex-ascii */
typedef enum { BINARY, ASCII85, ASCIIHEX } DATAMODE;

#ifdef min
#undef min
#endif
#define min(a, b) ((a) < (b) ? (a) : (b))

int Margin     = 20;    /* safety margin */
int quiet      = 0;     /* suppress informational messages */
int autorotate = 0;     /* disable automatic rotation */

/* Array of known page sizes including name, width, and height */

typedef struct { const char *name; int width; int height; } PageSize_s;

PageSize_s PageSizes[] = {
    {"a0",  2380, 3368},
    {"a1",  1684, 2380},
    {"a2",  1190, 1684},
    {"a3",  842, 1190},
    {"a4",  595, 842},
    {"a5",  421, 595},
    {"a6",  297, 421},
    {"b5",  501, 709},
    {"letter",  612, 792},
    {"legal",   612, 1008},
    {"ledger",  1224, 792},
    {"p11x17",  792, 1224}
};

#define PAGESIZELIST    (sizeof(PageSizes)/sizeof(PageSizes[0]))

#ifdef A4
int PageWidth  = 595;           /* page width A4  */
int PageHeight = 842;           /* page height A4 */
#else
int PageWidth  = 612;           /* page width letter  */
int PageHeight = 792;           /* page height letter */
#endif

#define VERSION     "V1.8"
static char *ColorSpaceNames[] = {"", "Gray", "", "RGB", "CMYK" };

typedef struct {
  FILE     *fp;                /* file pointer for jpeg file          */
  char     *filename;          /* name of image file                  */
  int      width;              /* pixels per line                     */
  int      height;             /* rows                                */
  int      components;         /* number of color components          */
  int      bits_per_component; /* bits per color component            */
  float    dpi;                /* image resolution in dots per inch   */
  DATAMODE mode;               /* output mode: 8bit, ascii, ascii85   */
  long     startpos;           /* offset to jpeg data                 */
  int      landscape;          /* rotate image to landscape mode?     */
  int      adobe;              /* image includes Adobe comment marker */
} imagedata;

#define DPI_IGNORE      -1.0f      /* dummy value for imagedata.dpi       */
#define DPI_USE_FILE     0.0f      /* dummy value for imagedata.dpi       */

typedef enum {      /* JPEG marker codes            */
  M_SOF0  = 0xc0,   /* baseline DCT             */
  M_SOF1  = 0xc1,   /* extended sequential DCT      */
  M_SOF2  = 0xc2,   /* progressive DCT          */
  M_SOF3  = 0xc3,   /* lossless (sequential)        */
  
  M_SOF5  = 0xc5,   /* differential sequential DCT      */
  M_SOF6  = 0xc6,   /* differential progressive DCT     */
  M_SOF7  = 0xc7,   /* differential lossless        */
  
  M_JPG   = 0xc8,   /* JPEG extensions          */
  M_SOF9  = 0xc9,   /* extended sequential DCT      */
  M_SOF10 = 0xca,   /* progressive DCT          */
  M_SOF11 = 0xcb,   /* lossless (sequential)        */
  
  M_SOF13 = 0xcd,   /* differential sequential DCT      */
  M_SOF14 = 0xce,   /* differential progressive DCT     */
  M_SOF15 = 0xcf,   /* differential lossless        */
  
  M_DHT   = 0xc4,   /* define Huffman tables        */
  
  M_DAC   = 0xcc,   /* define arithmetic conditioning table */
  
  M_RST0  = 0xd0,   /* restart              */
  M_RST1  = 0xd1,   /* restart              */
  M_RST2  = 0xd2,   /* restart              */
  M_RST3  = 0xd3,   /* restart              */
  M_RST4  = 0xd4,   /* restart              */
  M_RST5  = 0xd5,   /* restart              */
  M_RST6  = 0xd6,   /* restart              */
  M_RST7  = 0xd7,   /* restart              */
  
  M_SOI   = 0xd8,   /* start of image           */
  M_EOI   = 0xd9,   /* end of image             */
  M_SOS   = 0xda,   /* start of scan            */
  M_DQT   = 0xdb,   /* define quantization tables       */
  M_DNL   = 0xdc,   /* define number of lines       */
  M_DRI   = 0xdd,   /* define restart interval      */
  M_DHP   = 0xde,   /* define hierarchical progression  */
  M_EXP   = 0xdf,   /* expand reference image(s)        */
  
  M_APP0  = 0xe0,   /* application marker, used for JFIF    */
  M_APP1  = 0xe1,   /* application marker           */
  M_APP2  = 0xe2,   /* application marker           */
  M_APP3  = 0xe3,   /* application marker           */
  M_APP4  = 0xe4,   /* application marker           */
  M_APP5  = 0xe5,   /* application marker           */
  M_APP6  = 0xe6,   /* application marker           */
  M_APP7  = 0xe7,   /* application marker           */
  M_APP8  = 0xe8,   /* application marker           */
  M_APP9  = 0xe9,   /* application marker           */
  M_APP10 = 0xea,   /* application marker           */
  M_APP11 = 0xeb,   /* application marker           */
  M_APP12 = 0xec,   /* application marker           */
  M_APP13 = 0xed,   /* application marker           */
  M_APP14 = 0xee,   /* application marker, used by Adobe    */
  M_APP15 = 0xef,   /* application marker           */
  
  M_JPG0  = 0xf0,   /* reserved for JPEG extensions     */
  M_JPG13 = 0xfd,   /* reserved for JPEG extensions     */
  M_COM   = 0xfe,   /* comment              */
  
  M_TEM   = 0x01,   /* temporary use            */

  M_ERROR = 0x100   /* dummy marker, internal use only  */
} JPEG_MARKER;


void JPEGtoEPS(pictureStruct *picturePtr);
void WriteEPSHeader (imagedata *JPEG, FILE *PSfile, int *llx1, int *lly1, int *urx1, int *ury1);
uint8_t ReadOneByteInHex (FILE *pictureFile);
uint16_t ReadTwoBytesInHex (FILE *pictureFile);
int ReadNextMarker (FILE *pictureFile);

