/*
 * This routine was adapted from:
 *
 *  hfont - build a vfont raster file from a hershey format vector file
 *
 *  Written by William LeFebvre, LCSE, Rice University
 */

# include <stdio.h>
# include <vfont.h>

# define  MAX_CHARS	256		/* number of chars in a vfont file */
# define  Headersize	(sizeof(struct header) + sizeof(struct dispatch) * MAX_CHARS)


vfont()
{ int  vfd, filesize, bytecnt, nrows, ncols, colbytes;
  struct header v_header;
  struct dispatch v_character[256];
  struct dispatch *character;

    if ((vfd = creat(ftname, 0666)) == -1)
    {
	perror(ftname);
	exit(1);
    }
    /* leave space for the header, character descriptors, and space bitmap */
    lseek(vfd, Headersize + 2, 0);
    filesize = 2;

    /* translate the characters */

    for (letter = 0; letter < 128; letter++)
    {

	character = &(v_character[letter]);

	/* find the character in the Hershey font */

	if (letter == 32)
	{
	    /* this is a special width space character */
	    character->nbytes = 2;
	    character->addr = 0;
	    character->up = character->down =
		character->left = character->right = 0;
	    if (cindex[letter])
	    character->width = store[cindex[letter]+2]-store[cindex[letter]];
	    else character->width = 12; /* ??? */
	}
	else if (cindex[letter] || cvflag)
	{
	    /* normal hershey character */

	if (cvflag)
	{	if (letter == 0 || letter == 127) continue;
		currch = letter;
		tokeep();
		if (!currlen) continue;
		ctovkeep();/* showbits();*/
	} else
	    lplot(letter);

	    nrows = ytop + ybot;
	    ncols = currlen;
	    colbytes = (ncols + 7)/8;
#ifdef sun
	    if (colbytes & 1) colbytes += 1;
#endif
	    /* fill in the character descriptor */
	    character->width  = ncols; /* ADD bearings */
	    character->down   = ybot; /*down_max - baseline;*/
	    character->up     = ytop; /*baseline - up_max;*/
	    character->left   = 0; /*left_ex - left_max;*/
	    character->right  = ncols; /*right_max - left_ex + 1;*/
	    character->addr   = filesize;
	    character->nbytes = bytecnt = colbytes * nrows;
		/*((right_max - left_max + 8) >> 3) *
				    (down_max - up_max);*/

	    /* write the raster */
	    for (i = 0; i < nrows; i++)
		write(vfd, vkeep[i], colbytes);

	    filesize += bytecnt;

	}

    }

    /* write the font header with the real file size */
    v_header.size = filesize;
    v_header.magic = 0436;
    v_header.maxx = v_header.maxy = v_header.xtend = 0;
    lseek(vfd, 0, 0);
    write(vfd, &v_header, sizeof(v_header));

    /* write the character dispatch area */
    write(vfd, v_character, sizeof(v_character));

    close(vfd);
}

