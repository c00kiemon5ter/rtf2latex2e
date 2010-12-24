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

# include       <stdio.h>
# include       <stdlib.h>
# include <sys/wait.h>
# include <unistd.h>
static char *convertArgv[2];

int Figure2eps(char *inputPicture, char *outputPicture)
{
    int res = 0;
    pid_t pid;

    pid = fork();

    switch (pid) {
    case -1:
        printf("figure2eps: fork failed!\n");
        return (1);
    case 0:
        printf("Calling ImageMagick to convert figure to EPS...\n");
        convertArgv[0] = inputPicture;
        convertArgv[1] = outputPicture;
        execlp("convert", "convert", convertArgv[0], convertArgv[1], (char *) 0);
        /* if we reach here, the convert utility could not be found */
        exit(1);
        break;
    default:
        break;
    }

    if (pid != 0) {
        int stat_val;

        /* wait for the conversion process to finish */
        wait(&stat_val);
        if (WIFEXITED(stat_val) && !WEXITSTATUS(stat_val))
            res = 0;
        else
            res = 1;
    }

    return (res);
}
