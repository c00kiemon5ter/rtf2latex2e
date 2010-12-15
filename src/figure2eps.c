/*
 * figure conversion to eps for rtf2latex2e
 * (c) 2000 Ujwal S. Sathyam
 * This uses the ImageMagick API.
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
# include	<stdlib.h>

#if defined(__MWERKS__) || defined(_VISUALC_)
	#include "magick/magick.h"
	#include "magick/defines.h"
#else
    #include <sys/wait.h>
    #include <unistd.h>
    static char *convertArgv[2];
#endif	

int Figure2eps (char *inputPicture, char *outputPicture);


int Figure2eps (char *inputPicture, char *outputPicture)
{
int res = 0;

/* Use statically linked IMagick library for Mac and Windows */	
#if defined(__MWERKS__) || defined(_VISUALC_)

      Image
        *image;

      ImageInfo
        image_info;

      /*
        Initialize the image info structure and read an image.
      */
      GetImageInfo(&image_info);
      (void) strcpy(image_info.filename,inputPicture);
      image=ReadImage(&image_info);
      if (image == (Image *) NULL)
        return(1);

      /*
        Write the image as EPS and destroy it.
      */
      (void) strcpy(image->filename,outputPicture);
      WriteImage(&image_info,image);
      DestroyImage(image);
      
      
/* 
 * On Unix/Linux, use the external IMagick convert utility, 
 * if available 
 */	
#elif __GNUC__
    pid_t pid;
    int err = 0;

    pid = fork();

    switch(pid)
    {
    case -1:
        printf("figure2eps: fork failed!\n");
        return (1);
    case 0:
        printf("Calling ImageMagick to convert figure to EPS...\n");
        convertArgv[0] = inputPicture;
        convertArgv[1] = outputPicture;
        err = execlp("convert", "convert", convertArgv[0], convertArgv[1], (char *)0);
        /* if we reach here, the convert utility could not be found */
        exit(1);
        break;
    default:
        break;
    }

    if(pid != 0)
    {
        int stat_val;
        pid_t child_pid;

        /* wait for the conversion process to finish */
        child_pid = wait(&stat_val);
        if(WIFEXITED(stat_val) && !WEXITSTATUS(stat_val))
            res = 0;
        else
            res = 1;
    }

/* we can't convert, get out */
#else
	res = 1;
#endif		

return (res);
}
