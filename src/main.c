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


# include	<stdio.h>
# include	<string.h>
# include	<stdlib.h>
# include	"rtf.h"
# include	"rtf2LaTeX2e.h"

extern long	groupLevel;
extern char outputMapName[];

FILE *OpenLibFile (char *name, char *mode);

FILE *ifp, *ofp;
extern	char fileCreator[];
extern long groupLevel;
static char rtf2latex2eDir[rtfBufSiz];

static void usage(void)
{
	printf ("* rtf2latex2e usage: rtf2latex2e [-t <TeX-map>] <rtf file>\n");
	printf ("* output file will be *.tex.\n");
}

int main(int argc, char **argv)
{
char buf[rtfBufSiz], *buf1, buf2[rtfBufSiz];
int i, bufLength, fileCounter;
long cursorPos;
int optind;

	/* install function to open library files needed by the RTF reader */
	RTFSetOpenLibFileProc (OpenLibFile);
	
	optind = 1;
	
	
	if (!getenv("RTF2LATEX2E_DIR"))
	{
    	FILE* tfp;
    	
#if defined(UNIX)
    	strcpy (rtf2latex2eDir, PREFDIR);  /* PREF DIRECTORY fed from Makefile */
		bufLength = strlen (rtf2latex2eDir);
		if (rtf2latex2eDir[bufLength-1] != PATH_SEP)
		{
			rtf2latex2eDir[bufLength] = PATH_SEP;
			rtf2latex2eDir[bufLength+1] = '\0';
		}
    
#else
		/* use Win32 call to find location of rtf2latex2e */
		/* wish that Unix/Linux had a similar call */
		GetModuleFileName(NULL, rtf2latex2eDir, rtfBufSiz);
		/* strip out program name from path */
		cPtr = strrchr(rtf2latex2eDir, PATH_SEP);
		if (cPtr != NULL)
			*(cPtr + 1) = '\0';
#endif

		/* try to find the preferences file */
    	tfp = OpenLibFile( "r2l-pref", "r" );
    	if (tfp == NULL) {
        
      		RTFMsg ("* The environment variable RTF2LATEX2E_DIR has not been set.\n");
  			RTFMsg ("* This variable needs to point to the directory containing\n");
  			RTFMsg ("* the rtf2latex2e binary, the character set map files, and\n");
  			RTFMsg ("* the output map TeX-map file.\n");
  			RTFMsg ("* Depending on your shell, you can set this variable using\n");
  			RTFMsg ("* export RTF2LATEX2E_DIR=directory (bash) or \n");
  			RTFMsg ("* setenv RTF2LATEX2E_DIR directory (csh) or \n");
  			RTFMsg ("* SET RTF2LATEX2E_DIR=directory (DOS) or \n");
  			RTFMsg ("* You should also add this directory to your search path.\n");
  			RTFMsg ("* You can set these variables in your .bashrc, .login, or autoexec.bat file.\n\n");
  			RTFMsg ("* This program will now exit.\n");
  			return (0);
    	}
    	else
    	{
      		fclose(tfp);  /* OK, we found the preference files somewhere */
    	}
	}
	else
	{
		strcpy (rtf2latex2eDir, "");
		strcpy (rtf2latex2eDir, getenv("RTF2LATEX2E_DIR"));
		bufLength = strlen (rtf2latex2eDir);
		if (rtf2latex2eDir[bufLength-1] != PATH_SEP)
		{
			rtf2latex2eDir[bufLength] = PATH_SEP;
			rtf2latex2eDir[bufLength+1] = '\0';
		}
	}
	
	/* This is my cheesy command-line arguments parser. */
	/* I wanted to use getopt, but it varied too much   */
	/* among Unix/Linux platforms.                      */
	if (argc == 1)
	{
		usage();
		return 0;
	}
	
	/* initialize the outputMapName */
	strcpy(outputMapName, "");

	for (i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "-version"))
		{
			printf ("* This is rtf2latex2e version %s\n", rtf2latex2e_version);
			return (0);
		}
		else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-help"))
		{
			usage();
			return (0);		
		}
		/* this is for selecting a different TeX-map file */
		else if (!strcmp(argv[i], "-t"))
		{
			/* go to the next argument */
			i++;
			if(i >= argc-1)
			{
				usage();
				return (0);
			}
			strcpy(outputMapName, argv[i]);			
		}				
		else
			break; /* end of command-line options */
	}
	optind = i;
		
	/* one time writer initialization */
	WriterInit ();
	
	
	for (fileCounter = optind; fileCounter < argc; fileCounter++)
	{	
		RTFInit ();	
	
		/* open first file, set it as the input file, and enable global 
		 * access to input file name */
		/* strip .rtf from file name if found */
		if ((ifp = fopen(argv[fileCounter], "r")) == NULL) 
		{
			RTFPanic("* Cannot open input file %s\n", argv[fileCounter]);
			exit (1);
		}
		RTFSetStream (ifp);
		
		/* look at second token to check if input file is of type rtf */
		cursorPos = ftell (ifp);
		RTFGetToken ();
		RTFGetToken ();
		if (rtfMajor != rtfVersion)
		{
			RTFMsg ("* Oops! %s is not an rtf file!\n", argv[fileCounter]);
			if (fclose(ifp) != 0) printf("* error closing input file %s\n", 
			    argv[fileCounter]);
			continue;
		}
		fseek (ifp, cursorPos, 0);
		RTFInit ();

		
		strcpy(buf2, argv[fileCounter]);
		buf1 = buf2;
		bufLength = strlen(buf1);
		if (bufLength > 3)
		{
			buf1 += (bufLength - 4);
			/* strip .rtf by terminating string */
			if (strcmp(buf1, ".rtf") == 0 || strcmp(buf1, ".RTF") == 0) 
				buf2[bufLength - 4] = '\0'; 
		
		}
		
		/* replace any spaces in input file name by a dash - */
		buf1 = strrchr(buf2, PATH_SEP); /* strip away path info */
		if (buf1 == (char *)NULL)
			buf1 = buf2;
		bufLength = strlen(buf1);
		for (i = 0; i < bufLength; i++)
		{
			if (buf1[i] == ' ' || buf1[i] == '_' || buf1[i] == '.')
				buf1[i] = '-';
		}
		
		/* set input name to modified name of input file */
		RTFSetInputName (buf2);
		
		/* open output file, set it as the output file, and enable 
		 * global access to output file name */
		strcpy(buf, buf2);
		strcat(buf, ".tex");

		#if __POWERPC__ || __CFM68K__ || __MC68K__
			/* defined in unix.mac.h --- sets the creator */
   		 	_fcreator = Str2OSType (fileCreator);   
   		 	/* defined in unix.mac.h --- sets the type */ 
   		 	_ftype = 'TEXT';       
   		#endif
		if ((ofp = fopen(buf, "w+")) == NULL) 
			RTFPanic("Cannot open output file %s\n", buf);

		RTFSetOutputStream (ofp);
		RTFSetOutputName (buf);
	
		if (BeginLaTeXFile ())
		{
			RTFRead ();
			EndLaTeXFile ();
		}

		if (fclose(ifp) != 0) 
			printf("* error closing input file %s\n", argv[fileCounter]);
		if (fclose(ofp) != 0) 
			printf("* error closing output file %s\n", buf);
	}

/* 	printf ("* groupLevel is %d\n", groupLevel); */

	return (0);
}


/* function to open library files */
FILE *OpenLibFile (char *name, char *mode)
{
FILE *ifp;
char buf[rtfBufSiz];
char buf2[rtfBufSiz];
char *fileName;
char *relFileName;
int bufLength;

	strcpy(buf,  name);
  	strcpy(buf2, name);
	
#if	(defined(UNIX)) /* attach path to rtf2latex2e */
	strcpy (buf, rtf2latex2eDir);
    strcat (buf, "pref");
    bufLength = strlen (buf);
    if (buf[bufLength-1] != PATH_SEP)
    {
        buf[bufLength] = PATH_SEP;
        buf[bufLength+1] = '\0';
    }
	strcat (buf, name);

  strcpy(buf2, "pref");
  bufLength = strlen (buf2);
  if (buf2[bufLength-1] != PATH_SEP)
  {
        buf2[bufLength] = PATH_SEP;
        buf2[bufLength+1] = '\0';
  }
  strcat (buf2, name);
#else
	sprintf(buf,  "pref%c%s", PATH_SEP, name);	// probably silly
	sprintf(buf2, "%cpref%c%s", PATH_SEP, PATH_SEP, name);	
#endif
	
	fileName    = buf;
  relFileName = buf2;
	
	if ((ifp = fopen(name, mode)) == NULL) /* try to locate file in 
											current working directory first */
	{
    if ((ifp = fopen(relFileName, mode)) == NULL) /* try in relative path to cwd */
    {
      if ((ifp = fopen(fileName, mode)) == NULL) 
		  {
			  /* silently return if r2l-head is not found */
			  if (strcmp (name, "r2l-head") != 0 && strcmp (name, "r2l-map") != 0)
			  {
				  RTFMsg ("* Unable to open library file %s\n", fileName);
				  #if	(RTF2LATEX2E_UNIX || RTF2LATEX2E_DOS)
				  RTFMsg ("* Make sure the environment variable\n");
				  RTFMsg ("* RTF2LATEX2E_DIR is set and points\n");
				  RTFMsg ("* to the directory containing the rtf2latex2e\n");
				  RTFMsg ("* binary and the associated library files\n\n");
				  RTFMsg ("* current value of RTF2LATEX2E_DIR is %s\n\n",
						  getenv("RTF2LATEX2E_DIR"));
				  #endif
			  }
			  return (NULL);
		  }
		  else return (ifp);
    }
    else return (ifp);
	}	
	else return (ifp);

}

