/* This source file contains code to convert graphic file formats found in RTF to EPS.
   At present, only jpeg files are converted into EPS. I will add other file formats 
   as soon as I figure out how. The jpeg->eps conversion routine was adapted from the
   jpeg2ps code by Thomas Merz with his permission. jpeg2ps can be found at
   http://www.ifconnection.de/~tm
*/


# include	<stdio.h>
# include	<string.h>
# include	<stdlib.h>
# include	<time.h>
# include	"rtf.h"
# include	"tokenscan.h"
# include	"rtf2LaTeX2e.h"
# include	"jpeg2eps.h"

extern FILE *ifp, *ofp;
static long hexCount;

/* analyze JPEG marker */
void JPEGtoEPS(pictureStruct *picturePtr) 
{
imagedata image;
long startHeXJpeg;
char dummyBuf[100];
FILE *pictureFile;
int b, c, unit, j;
unsigned long i, length;
boolean SOF_done = false;
#define APP_MAX 255
char appstring[APP_MAX];
int llx, lly, urx, ury;

    startHeXJpeg = ftell(ifp);

	strcpy (dummyBuf, "");

	/* get input file name and create corresponding picture file name */
	strcpy (picturePtr->name, RTFGetInputName ());
	sprintf (dummyBuf, "Fig%d.eps", ++(picturePtr->count));
	strcat (picturePtr->name, dummyBuf);
		
	if ((pictureFile = fopen(picturePtr->name, "wb")) == NULL) 
		RTFPanic("Cannot open input file %s\n", picturePtr->name);

	image.filename = NULL;
	image.mode     = ASCIIHEX;
	image.startpos = 0L;
	image.landscape= false;
	image.dpi      = DPI_IGNORE;
	image.adobe    = false;
	image.filename = picturePtr->name;

	for (i = 0; i < 6; i++)
	{
		for (j = 0; j < 200; j++)	
			fprintf (pictureFile, " ");
		fprintf (pictureFile, "\n");
	}
			

	hexCount = 0;
	do {
		do {
			c = ReadOneByteInHex (pictureFile);
		} while (RTFCheckCM (rtfGroup, rtfEndGroup) == 0 && c != 0xFF);
	
		do {
			c = ReadOneByteInHex (pictureFile);
		} while (c == 0xFF);
		
		if (c == M_SOI) 
			break;
	} while (RTFCheckCM (rtfGroup, rtfEndGroup) == 0);

	if (RTFCheckCM (rtfGroup, rtfEndGroup) != 0) {
		RTFMsg("Error: SOI marker not found!\n");
		return;
	}

  /* process JPEG markers */
	while (!SOF_done && (c = ReadNextMarker (pictureFile)) != M_EOI) {
		switch (c) {
			case M_ERROR:
			fprintf(stderr, "Error: unexpected end of JPEG file!\n");
			return;

      		/* The following are not officially supported in PostScript level 2 */
      		case M_SOF2:
      		case M_SOF3:
      		case M_SOF5:
      		case M_SOF6:
      		case M_SOF7:
      		case M_SOF9:
      		case M_SOF10:
      		case M_SOF11:
      		case M_SOF13:
      		case M_SOF14:
      		case M_SOF15:
				fprintf(stderr, 
         			"Warning: JPEG file uses compression method %X - proceeding anyway.\n",
		  				c);
        		fprintf(stderr, 
					"PostScript output does not work on all PS interpreters!\n");
			/* FALLTHROUGH */

      		case M_SOF0:
      		case M_SOF1:
	length = ReadTwoBytesInHex (pictureFile);    /* read segment length  */

	image.bits_per_component = ReadOneByteInHex (pictureFile);
	image.height             = ReadTwoBytesInHex (pictureFile);
	image.width              = ReadTwoBytesInHex (pictureFile);
	image.components         = ReadOneByteInHex (pictureFile);

	SOF_done = true;
	break;

      case M_APP0:		/* check for JFIF marker with resolution */
		length = ReadTwoBytesInHex (pictureFile);

	for (i = 0; i < length-2; i++) {	/* get contents of marker */
	  b = ReadOneByteInHex (pictureFile);
	  if (i < APP_MAX)			/* store marker in appstring */
	    appstring[i] = b;
	}

	/* Check for JFIF application marker and read density values
	 * per JFIF spec version 1.02.
	 * We only check X resolution, assuming X and Y resolution are equal.
	 * Use values only if resolution not preset by user or to be ignored.
	 */

#define ASPECT_RATIO	0	/* JFIF unit byte: aspect ratio only */
#define DOTS_PER_INCH	1	/* JFIF unit byte: dots per inch     */
#define DOTS_PER_CM	2	/* JFIF unit byte: dots per cm       */

	if (image.dpi == DPI_USE_FILE && length >= 14 &&
	    !strncmp("JFIF", appstring, 4)) {
	  unit = appstring[7];		        /* resolution unit */
	  					/* resolution value */
	  image.dpi = (float) ((appstring[8]<<8) + appstring[9]);

	  if (image.dpi == 0.0) {
	    image.dpi = DPI_USE_FILE;
	    break;
	  }

	  switch (unit) {
	    /* tell the caller we didn't find a resolution value */
	    case ASPECT_RATIO:
	      image.dpi = DPI_USE_FILE;
	      break;

	    case DOTS_PER_INCH:
	      break;

	    case DOTS_PER_CM:
	      image.dpi *= (float) 2.54;
	      break;

	    default:				/* unknown ==> ignore */
	      fprintf(stderr, 
		"Warning: JPEG file contains unknown JFIF resolution unit - ignored!\n");
	      image.dpi = DPI_IGNORE;
	      break;
	  }
	}
        break;

      case M_APP14:				/* check for Adobe marker */
	length = ReadTwoBytesInHex (pictureFile);

	for (i = 0; i < length-2; i++) {	/* get contents of marker */
	  b = ReadOneByteInHex (pictureFile);
	  if (i < APP_MAX)			/* store marker in appstring */
	    appstring[i] = b;
	}

	/* Check for Adobe application marker. It is known (per Adobe's TN5116)
	 * to contain the string "Adobe" at the start of the APP14 marker.
	 */
	if (length >= 12 && !strncmp("Adobe", appstring, 5))
	  image.adobe = true;			/* set Adobe flag */

	break;

      case M_SOI:		/* ignore markers without parameters */
      case M_EOI:
      case M_TEM:
      case M_RST0:
      case M_RST1:
      case M_RST2:
      case M_RST3:
      case M_RST4:
      case M_RST5:
      case M_RST6:
      case M_RST7:
	break;

      default:			/* skip variable length markers */
	length = ReadTwoBytesInHex (pictureFile);
	for (length -= 2; length > 0; length--)
	  (void) ReadOneByteInHex (pictureFile);
	break;
    }
  }

	/* do some sanity checks with the parameters */
	if (image.height <= 0 || image.width <= 0 || image.components <= 0) {
	fprintf(stderr, "Error: DNL marker not supported in PostScript Level 2!\n");
	}

	/* some broken JPEG files have this but they print anyway... */
	if (length != (unsigned int) (image.components * 3 + 8))
	fprintf(stderr, "Warning: SOF marker has incorrect length - ignored!\n");

	if (image.bits_per_component != 8) {
		fprintf(stderr, "Error: %d bits per color component ",
		image.bits_per_component);
		fprintf(stderr, "not supported in PostScript level 2!\n");
	}

	if (image.components!=1 && image.components!=3 && image.components!=4) {
		fprintf(stderr, "Error: unknown color space (%d components)!\n",
		image.components);
	}
  
  
  		RTFGetToken ();
		while (RTFCheckCM (rtfGroup, rtfEndGroup) == 0) {
  			fprintf (pictureFile, "%c", rtfTextBuf[0]);
  			hexCount++;
  			if (hexCount > 60)
  			{
  				fprintf (pictureFile, "\n");
  				hexCount = 0;
  			}
			RTFGetToken ();
  		}
	
	fprintf (pictureFile, ">\n\%\%EOF");
	
	fseek (pictureFile, 0, 0);
	WriteEPSHeader (&image, pictureFile, &llx, &lly, &urx, &ury);
	fprintf (pictureFile, "\n");

	if (fclose(pictureFile) != 0) 
		printf("* error closing picture file %s\n", picturePtr->name);
		
 
 	picturePtr->llx = llx;
 	picturePtr->lly = lly;
 	picturePtr->urx = urx;
 	picturePtr->ury = ury;
 	
	RTFRouteToken ();
}

void
WriteEPSHeader (imagedata *JPEG, FILE *PSfile, int *llx1, int *lly1, int *urx1, int *ury1)
{
int llx, lly, urx, ury;        /* Bounding box coordinates */
float scale, sx, sy;           /* scale factors            */
time_t t;
int i;

	if (JPEG->width > JPEG->height && autorotate) {	/* switch to landscape if needed */
		JPEG->landscape = 1; /*true*/
    }
    if (!JPEG->landscape) {       /* calculate scaling factors */
		sx = (float) (PageWidth - 2*Margin) / JPEG->width;
		sy = (float) (PageHeight - 2*Margin) / JPEG->height;
    }
    else {
		sx = (float) (PageHeight - 2*Margin) / JPEG->width;
		sy = (float) (PageWidth - 2*Margin) / JPEG->height;
    }
    scale = min(sx, sy);	/* We use at least one edge of the page */

	if (JPEG->landscape) {
		/* landscape: move to (urx, lly) */
		urx = PageWidth - Margin;
		lly = Margin;
		ury = (int) (Margin + scale*JPEG->width + 0.9);    /* ceiling */
		llx = (int) (urx - scale * JPEG->height);          /* floor  */
	}
	else {
		/* portrait: move to (llx, lly) */
		llx = lly = Margin;
		urx = (int) (llx + scale * JPEG->width + 0.9);     /* ceiling */
		ury = (int) (lly + scale * JPEG->height + 0.9);    /* ceiling */
  	}


  /* produce EPS header comments */
  fprintf(PSfile, "%%!PS-Adobe-3.0 EPSF-3.0\n");
  fprintf(PSfile, "%%%%Creator: jpeg2ps %s by Thomas Merz\n", VERSION);
  fprintf(PSfile, "%%%%Title: %s\n", JPEG->filename);
  fprintf(PSfile, "%%%%CreationDate: %s", ctime(&t));
  fprintf(PSfile, "%%%%BoundingBox: %d %d %d %d\n", 
                   llx, lly, urx, ury);
  fprintf(PSfile, "%%%%DocumentData: %s\n", 
                  JPEG->mode == BINARY ? "Binary" : "Clean7Bit");
  fprintf(PSfile, "%%%%LanguageLevel: 2\n");
  fprintf(PSfile, "%%%%EndComments\n");
  fprintf(PSfile, "%%%%BeginProlog\n");
  fprintf(PSfile, "%%%%EndProlog\n");
  fprintf(PSfile, "%%%%Page: 1 1\n");

  fprintf(PSfile, "/languagelevel where {pop languagelevel 2 lt}");
  fprintf(PSfile, "{true} ifelse {\n");
  fprintf(PSfile, "  (JPEG file '%s' needs PostScript Level 2!",
                  JPEG->filename);
  fprintf(PSfile, "\\n) dup print flush\n");
  fprintf(PSfile, "  /Helvetica findfont 20 scalefont setfont ");
  fprintf(PSfile, "100 100 moveto show showpage stop\n");
  fprintf(PSfile, "} if\n");

  fprintf(PSfile, "save\n");
  fprintf(PSfile, "/RawData currentfile ");

  if (JPEG->mode == ASCIIHEX)            /* hex representation... */
    fprintf(PSfile, "/ASCIIHexDecode filter ");
  else if (JPEG->mode == ASCII85)        /* ...or ASCII85         */
    fprintf(PSfile, "/ASCII85Decode filter ");
  /* else binary mode: don't use any additional filter! */

  fprintf(PSfile, "def\n");

  fprintf(PSfile, "/Data RawData << ");
  fprintf(PSfile, ">> /DCTDecode filter def\n");

  /* translate to lower left corner of image */
  fprintf(PSfile, "%d %d translate\n", (JPEG->landscape ? 
                   PageWidth - Margin : Margin), Margin);

  if (JPEG->landscape)                 /* rotation for landscape */
    fprintf(PSfile, "90 rotate\n");
      
  fprintf(PSfile, "%.2f %.2f scale\n", /* scaling */
                   JPEG->width * scale, JPEG->height * scale);
  fprintf(PSfile, "/Device%s setcolorspace\n", 
                  ColorSpaceNames[JPEG->components]);
  fprintf(PSfile, "{ << /ImageType 1\n");
  fprintf(PSfile, "     /Width %d\n", JPEG->width);
  fprintf(PSfile, "     /Height %d\n", JPEG->height);
  fprintf(PSfile, "     /ImageMatrix [ %d 0 0 %d 0 %d ]\n",
                  JPEG->width, -JPEG->height, JPEG->height);
  fprintf(PSfile, "     /DataSource Data\n");
  fprintf(PSfile, "     /BitsPerComponent %d\n", 
                  JPEG->bits_per_component);

  /* workaround for color-inverted CMYK files produced by Adobe Photoshop:
   * compensate for the color inversion in the PostScript code
   */
  if (JPEG->adobe && JPEG->components == 4) {
    if (!quiet)
	fprintf(stderr, "Note: Adobe-conforming CMYK file - applying workaround for color inversion.\n");
    fprintf(PSfile, "     /Decode [1 0 1 0 1 0 1 0]\n");
  }else {
    fprintf(PSfile, "     /Decode [0 1");
    for (i = 1; i < JPEG->components; i++) 
      fprintf(PSfile," 0 1");
    fprintf(PSfile, "]\n");
  }

  fprintf(PSfile, "  >> image\n");
  fprintf(PSfile, "  Data closefile\n");
  fprintf(PSfile, "  RawData flushfile\n");
  fprintf(PSfile, "  showpage\n");
  fprintf(PSfile, "  restore\n");
  fprintf(PSfile, "} exec");
  
  /* send back bounding box information */
  *llx1 = llx;
  *lly1 = lly;
  *urx1 = urx;
  *ury1 = ury;

}

int
ReadNextMarker (FILE *pictureFile)
{
int c, nbytes = 0;

	if (RTFCheckCM (rtfGroup, rtfEndGroup) != 0)
		return M_ERROR;

	do {
		do {                            /* skip to FF 		  */
			nbytes++;
			c = ReadOneByteInHex (pictureFile);
		} while (c != 0xFF);
		do {                            /* skip repeated FFs  	  */
			c = ReadOneByteInHex (pictureFile);
		} while (c == 0xFF);
	} while (c == 0);                 /* repeat if FF/00 	      	  */

  return c;

}

int 
ReadOneByteInHex (FILE *pictureFile)
{
int hexNumber;

	RTFGetToken ();
	hexCount++;
	hexNumber = 16 * RTFCharToHex(rtfTextBuf[0]);
	if (hexCount > 60)
	{
		fprintf (pictureFile, "\n");
		hexCount = 0;
	}
	fprintf (pictureFile, "%s", rtfTextBuf);
	RTFGetToken ();
	hexCount++;
	if (hexCount > 60)
	{
		fprintf (pictureFile, "\n");
		hexCount = 0;
	}
	fprintf (pictureFile, "%s", rtfTextBuf);
	hexNumber += RTFCharToHex(rtfTextBuf[0]);
	return (hexNumber);
}

long 
ReadTwoBytesInHex (FILE *pictureFile)
{
int hexNumber;

	hexNumber = 256 * ReadOneByteInHex (pictureFile);
	hexNumber += ReadOneByteInHex (pictureFile);
	return (hexNumber);
}
