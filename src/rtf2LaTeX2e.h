/*
 * header file for RTF-to-LaTeX2e translation writer code.
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


# define	rtf2latex2e_version "1.1"

# define	WRAP_LIMIT	120
# define	PACKAGES	9

#ifndef boolean
typedef unsigned char boolean;
#endif
# define	New(t)	((t *) RTFAlloc (sizeof (t)))
# define	Swap(a, b) {double tmp;\
						tmp = a;\
						a = b;\
						b = tmp;}

#if defined(UNIX)
	# define	PATH_SEP	'/'
#else 
	# define	PATH_SEP	'\\'
#endif

#ifndef false
	#define false 0
#endif

#ifndef true
	#define true 1
#endif

void	WriterInit (void);
int	BeginLaTeXFile (void);
void EndLaTeXFile (void);

void RTFSetOutputStream (FILE *ofp);

enum pictType {unknownPict, pict, wmf, gif, jpeg, bin, png};
enum objClass {unknownObj, Equation, equation, WordPicture, MSGraphChart};
enum word97ObjectClass {unknownWord97Object, word97Object, standardObject, word97Picture, word97ObjText};
enum {left, center, right};
enum {singleSpace, oneAndAHalfSpace, doubleSpace};
typedef enum {tinySize, scriptSize, footNoteSize, smallSize, normalSize, 
			largeSize, LargeSize, LARGESize, giganticSize, GiganticSize} fontSize;
enum cellMergeFlag {none, first, previous};

typedef struct 
{
	int	count;
	int	type;
	long	width;
	long	height;
	long	goalWidth;
	long	goalHeight;
	long	scaleX;
	long	scaleY;
	int		llx;
	int		lly;
	int		urx;
	int		ury;
	char	name[rtfBufSiz];
} pictureStruct;

typedef struct
{
	int count;
} equationStruct;


typedef struct
{
	boolean	newStyle;
	int		boldGroupLevel;
	int		noBoldGroupLevel;
	boolean	wroteBold;
	boolean	wroteNoBold;
	int		italicGroupLevel;
	int		noItalicGroupLevel;
	boolean	wroteItalic;
	boolean	wroteNoItalic;
	int		underlinedGroupLevel;
	int		noUnderlinedGroupLevel;
	boolean	wroteUnderlined;
	int		dbUnderlinedGroupLevel;
	int		noDbUnderlinedGroupLevel;
	boolean	wroteDbUnderlined;
	int		shadowedGroupLevel;
	int		noShadowedGroupLevel;
	boolean	wroteShadowed;
	int		allCapsGroupLevel;
	boolean	wroteAllcaps;
	int		smallCapsGroupLevel;
	boolean	wroteSmallCaps;
	long	foreColor;
	int		foreColorGroupLevel;
	boolean	wroteForeColor;
	long	backColor;
	int		backColorGroupLevel;
	boolean	wroteBackColor;
	int		subScriptGroupLevel;
	int		noSubScriptGroupLevel;
	boolean	wroteSubScript;
	boolean	wroteNoSubScript;
	int		superScriptGroupLevel;
	int		noSuperScriptGroupLevel;
	boolean wroteSuperScript;
	boolean wroteNoSuperScript;
	long	fontSize;
	int		fontSizeGroupLevel;
	boolean	wroteFontSize;
	boolean	open;
} textStyleStruct;


typedef struct cell
{
	long	x;
	long	y;
	long	left;
	long	right;
	double	width;
	int		columnSpan;
	int		index;
	int		mergePar;	
	long	textColor;
	boolean	textBold;
	boolean	textItalic;
	boolean	textUnderlined;
	struct cell	*nextCell;
} cell;

typedef struct
{
	boolean	inside;
	int		rows;
	int		cols;
	int		cellCount;
	long	leftEdge;
	cell	*cellInfo;
	int		rowInfo[rtfBufSiz];
	long	*columnBorders;
	int		cellMergePar;
	long	previousColumnValue;
	boolean	newRowDef;
    boolean multiCol;
    boolean multiRow;
} tableStruct;

short ReadPrefFile (char *file);


