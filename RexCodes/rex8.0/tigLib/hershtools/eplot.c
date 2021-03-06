
#include <stdio.h>
#define TRUE 1
#define FALSE 0

extern char *term_name;
char w300 = FALSE;

#define STSIZE	32000
#define KEEPSIZE 80

#define getnam(nm) cook_tty(tty_fid);scanf("%s",nm);uncook_tty(tty_fid);

int	tty_fid;

char penstate, debug, /*ibuf[BUFSIZ],*/ ins[90];
FILE *ibuf;
int cnum;

int itempt, co1, co2, co3, lebear, ribear, x, y;
int deltax, deltay, xinc, yinc;
int lx,llx,ly,lly, cx,ccx,cy,ccy, x1,x2,y1,y2, a1,a2,d1,d2, i1,i2;
int minx, miny, maxx, maxy, base, obase, xheight, ytop, ybot, serifs;
int begy,begx;
char itemtype, letter, linelet, retrace, openflag, lighten, hairflag;

int xheavy, xlight, yheavy, ylight, penwidth, pxangle, pyangle,
	squarepen, shading, xshade, yshade;


#define ESC	27
#define RIGHTCOL 79
#define LEFTCOL  0
#define BOTROW  23

#define PRPTNL  2

#define BLACK	'1'
#define BLUE	'2'
#define GREEN	'3'
#define CYAN	'4'
#define RED	'5'
#define MAGENTA '6'
#define YELLOW	'7'
#define WHITE	'8'

#define UPPRDOT	','
#define LOWRDOT	'-'
#define BOTHDOT '7'
#define NODOT	' '
char upperdot=UPPRDOT, lowerdot=LOWRDOT, bothdot=BOTHDOT;

char	currlet, reallet;
unsigned currlines[96];
unsigned xpt, penplace,oldpplace, lastplace;


char	cmd, currch, cch, cbyte, cbyteo, mxflag;
int	col, row, xpos, ypos, currlen;
char	erow,ecol, pxc,pyc, penchar;

char	ckeep[6][KEEPSIZE], gxoff, gyoff, ib;

int i, fd;
char ftname[30];
char  duplflag;

/******************/


/* char obuf[BUFSIZ];*/
FILE *obuf;
int cocnt;
unsigned char sletter;
unsigned svpt;

/***************/

char gr[100][100];

char *_stsz;
unsigned stsize, cindex[128], storept;
unsigned char psxoff[128], pswidth[128];
char store[STSIZE];

/*****************/

savechr()
{	char fname[30];

	if (storept< 2) {printf("nothing to save\r\n"); return; }

	strcpy(fname,ftname);
	strcat(fname,".chr");
	if ((obuf = fopen(fname, "w")) == NULL) exit(1);

	fprintf(obuf, "Height(%d)\n", ytop);
	if (xheight) fprintf(obuf, "x Height(%d)\n", xheight);
	if (base) fprintf(obuf, "Base(%d)\n", base);


	cocnt = 0;

	for (sletter = 0; sletter < 128 ; sletter++)
	{
		if (!(svpt = cindex[sletter])) continue;
		/*if (kbhit()) return;*/

		if (sletter < ' ') fprintf(obuf, "^%c': ", sletter+64);
		else if (sletter >= 127) fprintf(obuf, "\\%o: ", sletter);
		else fprintf(obuf, "'%c': ", sletter);

		while (!(dspc(store[svpt], store[svpt+1]))) svpt +=2;

	}

	fprintf(obuf, "\n");
	fclose(obuf);

}

dspc(b1, b2)
char b1, b2;
{
	co1 = b1 - 64; co2 = b2 - 64;

	if (co1 == -64 && co2 == -64)
	{	fprintf(obuf," End Char\n");
		cocnt = 0;
		return(TRUE);
	}


	if (co1 == -64)
	{	if (!co2) fprintf(obuf," Pen Up\n    ");
		else if (b2 == 'H') fprintf(obuf,"\n   Hair ");
		cocnt = 0;
	}
	else
	{	if (cocnt >8) {cocnt = 0; fprintf(obuf,"\n    ");}

		fprintf(obuf,"(%d,%d)", co1,co2);
		cocnt++;
	}

	return(FALSE);
}



saveasm()
{	char fname[30];

	if (storept< 2) {printf("nothing to save\r\n"); return; }

	strcpy(fname,ftname);
	strcat(fname,".a");
	if ((obuf = fopen(fname, "w")) == NULL) exit(1);

	fprintf(obuf, "\n\tsection  font,data\n");
	fprintf(obuf, "\tdc.w %d,%d\n", ytop, base);
	fprintf(obuf, "\txdef\t%s\n%s\n\n", ftname, ftname);
	fprintf(obuf, "\tinclude\t\"chrdir\"\n\n");
	fprintf(obuf, ".h20\n\tdc.b -6,6\n  endchar\n");



	for (sletter = 33; sletter < 128 ; sletter++)
	{

		if ( (sletter >= 'A' && sletter <= 'Z')
		  || (sletter >= 'a' && sletter <= 'z') )
			fprintf(obuf, ".%c\n", sletter);
		else if (sletter >= '0' && sletter <= '9')
			fprintf(obuf, ".d%c\n", sletter);
		else fprintf(obuf, ".h%02.2x\n", sletter);

		if (!(svpt = cindex[sletter]))
		{	fprintf(obuf, "\tdc.b 0,0\n  endchar\n");
			continue;
		}


		minx = 63; maxx =  -63;
		while ( (x=store[svpt]) + (y=store[svpt+1]) )
		{	svpt += 2;
			if (!(x)) continue;
			minx = min(minx, x-64);
			maxx = max(maxx, x-64);
		}
		fprintf(obuf, "\tdc.b %d,%d\n", minx-lebear, maxx+ribear);

		svpt = cindex[sletter];

		while (!(dspca(store[svpt], store[svpt+1]))) svpt +=2;

	}

	fprintf(obuf, "\n\tend\n");
	fclose(obuf);

}

dspca(b1, b2)
char b1, b2;
{
	co1 = b1 - 64; co2 = b2 - 64;

	if (co1 == -64 && co2 == -64)
	{	fprintf(obuf,"  endchar\n");
		return(TRUE);
	}


	if (co1 == -64)
	{	if (!co2) fprintf(obuf,"  penup\n");
		/* else if (b2 != 'H')
		   fprintf(obuf,"\tdc.b -%d,%d\n", co2,co2); */
	}
	else
	{
		fprintf(obuf,"\tdc.b %d,%d\n", co1,co2);
	}

	return(FALSE);
}



saveps()
{	char fname[30];
	char *nameoflet();
	int letcount;
	int fbb[4];

	if (storept< 2) {printf("nothing to save\r\n"); return; }

	strcpy(fname,ftname);
	strcat(fname,".ps");
	if ((obuf = fopen(fname, "w")) == NULL) exit(1);

	fprintf(obuf, "10 dict dup begin\n");
	fprintf(obuf, "/FontType 3 def\n");
	fprintf(obuf, "/FontMatrix [.03 0 0 .03 0 0] def\n");
	fprintf(obuf, "/Encoding 256 array def\n");
	fprintf(obuf, "0 1 255 {Encoding exch/.notdef put} for\n");
	fprintf(obuf, "Encoding\n");
	for (sletter = 0; sletter < 128 ; sletter++)
	{	if (!(svpt = cindex[sletter])) continue;
		fprintf(obuf, "dup %d /%s put\n", sletter, nameoflet(sletter));
	}
	fprintf(obuf, "pop\n");

#define HLWIDTH 1
	letcount = 0;
	for (sletter = 0; sletter < 128 ; sletter++)
	{	if (!(svpt = cindex[sletter])) continue;
		letcount++;
	}

	fprintf(obuf, "/BBox %d dict def\nBBox begin\n", letcount+1);
	fprintf(obuf, " /.notdef [0 0 0 0] def\n");
	fbb[0] = fbb[1] = 63; fbb[2] = fbb[3] = -63;
	for (sletter = 0; sletter < 128 ; sletter++)
	{	if (!(svpt = cindex[sletter])) continue;
		co2 = minx = miny = 63; co3 = maxx = maxy = -63;
		while ( (x=store[svpt]) + (y=store[svpt+1]) )
		{	svpt += 2;
			if (!(co1=x)) continue;
			if (i1 && i2) co1 += fadjust(-base+y-64,i1,i2)-50;
			co2 = min(co2, co1-64);
			minx = min(minx, x-64);
			miny = min(miny, y-64);
			co3 = max(co3, co1-64);
			maxx = max(maxx, x-64);
			maxy = max(maxy, y-64);
		}
		psxoff[sletter] = lebear - minx;
		pswidth[sletter] = lebear + maxx - minx + 1 + ribear;
		fprintf(obuf, " /%s [%d %d %d %d] def\n", nameoflet(sletter),
			co2 + psxoff[sletter] - HLWIDTH,
			-maxy + base - HLWIDTH,
			co3 + psxoff[sletter] + HLWIDTH,
			-miny + base + HLWIDTH);
		fbb[0] = min(fbb[0], co2 + psxoff[sletter] - HLWIDTH);
		fbb[1] = min(fbb[1], -maxy + base - HLWIDTH);
		fbb[2] = max(fbb[2], co3 + psxoff[sletter] + HLWIDTH);
		fbb[3] = max(fbb[3], -miny + base + HLWIDTH);
	}
	fprintf(obuf, "end\n");
	fprintf(obuf, "/FontBBox [%d %d %d %d] def\n",fbb[0],fbb[1],
				fbb[2],fbb[3]);

	fprintf(obuf, "/Metrics %d dict def\nMetrics begin\n", letcount+1);
	fprintf(obuf, " /.notdef 0 def\n");
	for (sletter = 0; sletter < 128 ; sletter++)
	{	if (!(svpt = cindex[sletter])) continue;
		fprintf(obuf, " /%s %d def\n", nameoflet(sletter),
			pswidth[sletter]);
	}
	fprintf(obuf, "end\n");


	fprintf(obuf, "/CharacterDefs %d dict def\nCharacterDefs begin\n",
		letcount+1);
	fprintf(obuf, "/.notdef {} def\n");
	cocnt = 0;
	for (sletter = 0; sletter < 128 ; sletter++)
	{	if (!(svpt = cindex[sletter])) continue;
		fprintf(obuf, "/%s\n { newpath\n   ", nameoflet(sletter));
		penstate = 0;
		while (!(dsps(store[svpt], store[svpt+1]))) svpt +=2;
	}
	fprintf(obuf, "end\n");

	pbuildchar();

	fprintf(obuf, "/UniqueID %d def\n\nend\n", ftname[0]);

	fprintf(obuf, "/%s exch definefont pop\n", ftname);

	fclose(obuf);

}


dsps(b1, b2)
char b1, b2;
{
	co1 = b1 - 64; co2 = b2 - 64;

	if (co1 == -64 && co2 == -64)
	{
		fprintf(obuf, " stroke } def\n");
		cocnt = 0;
		return(TRUE);
	}


	if (co1 == -64)
	{	if (!co2) penstate = 0;
		/*else if (b2 == 'H') fprintf(obuf,"\n   Hair ");*/
	}
	else
	{	if (cocnt >3) {cocnt = 0; fprintf(obuf,"\n   ");}

		if (i1 && i2) co1 += fadjust(-base+co2,i1,i2) - 50;

		fprintf(obuf," %d %d ", co1+psxoff[sletter],base-co2);
		if (penstate) fprintf(obuf,"lineto");
		else fprintf(obuf,"moveto");
		penstate = 1;
		cocnt++;
	}

	return(FALSE);
}

char *nameoflet(c)
char c;
{ static char *npc[] = {
	"A", "ctl@",
	"esc", "fs", "gs", "rs", "us",
	"space", "exclam", "quotedbl", "numbersign", "dollar",
	"percent", "ampersand", "quoteright", "parenleft",
	"parenright", "asterisk", "plus", "comma", "hyphen",
	"period", "slash",
	"zero", "one", "two", "three", "four", "five", "six", "seven",
	"eight", "nine", "colon", "semicolon", "less", "equal",
	"greater", "question", "at",
	"bracketleft", "backslash", "bracketright", "asciicircum",
	"underscore", "quoteleft",
	"braceleft", "bar", "braceright", "asciitilde", "del"
  };

	if (c < 0x1B) { npc[1][3] = c + 0x40; return(npc[1]); }
	if (c < 'A')	return(npc[c - 0x19]);
	if (c <= 'Z') {	npc[0][0] = c; return(npc[0]); }
	if (c < 'a')  {	return(npc[c + 39 - 'Z']); }
	if (c <= 'z') {	npc[0][0] = c; return(npc[0]); }
	return(npc[c + 45 - 'z']);
}

pbuildchar()
{
	fprintf(obuf,"/BuildChar\n");
	fprintf(obuf,"{ 0 begin\n");
	fprintf(obuf," /char exch def\n /fontdict exch def\n");
	fprintf(obuf," /charname fontdict /Encoding get char get def\n");
	fprintf(obuf," fontdict begin\n");
	fprintf(obuf,"  Metrics charname get 0\n");
	fprintf(obuf,"  BBox charname get aload pop\n");
	fprintf(obuf,"  setcachedevice\n");
	fprintf(obuf,"  %1.1f setlinewidth\n", 2.0*HLWIDTH);
	fprintf(obuf,"  CharacterDefs charname get exec\n");
	fprintf(obuf," end\nend\n} def\n");
	fprintf(obuf,"/BuildChar load 0 3 dict put\n");
}


savetfm()
{	char fname[30];
	char *nameoflet();

	if (storept< 2) {printf("nothing to save\r\n"); return; }

	strcpy(fname,ftname);
	strcat(fname,".pl");
	if ((obuf = fopen(fname, "w")) == NULL) exit(1);

	fprintf(obuf, "\n");

	fprintf(obuf,"(DESIGNSIZE R 10.0)\n");
	fprintf(obuf,"(COMMENT DESIGNSIZE IS IN POINTS)\n");
	fprintf(obuf,"(COMMENT OTHER SIZES ARE MULTIPLES OF DESIGNSIZE)\n");
	fprintf(obuf,"(FONTDIMEN\n");
	if (i1 && i2)
		fprintf(obuf,"   (SLANT R %1.6f)\n", (float)-i1/i2);
	fprintf(obuf,"   (SPACE R 0.357776)\n");
	fprintf(obuf,"   (STRETCH R 0.153333)\n");
	fprintf(obuf,"   (SHRINK R 0.1022215)\n");
	if (xheight)
	fprintf(obuf,"   (XHEIGHT R %1.6f)\n", (float)xheight/33.33);
	else
	fprintf(obuf,"   (XHEIGHT R 0.430555)\n");
	fprintf(obuf,"   (QUAD R 1.022217)\n");
	fprintf(obuf,"   (EXTRASPACE R 0.1022215)\n");
	fprintf(obuf,"   )\n");

	for (sletter = 0; sletter < 128 ; sletter++)
	{	if (!(svpt = cindex[sletter])) continue;
		fprintf(obuf,"(CHARACTER O %o\n", sletter);
		minx = miny = 63; co2 = maxx = maxy = -63;
		while ( (x=store[svpt]) + (y=store[svpt+1]) )
		{	svpt += 2;
			if (!(co1=x)) continue;
			if (i1 && i2) co1 += fadjust(-base+y-64,i1,i2)-50;
			minx = min(minx, x-64);
			miny = min(miny, y-64);
			maxx = max(maxx, x-64);
			maxy = max(maxy, y-64);
			co2 = max(co2, co1-64);
		}
		if ((x = lebear + maxx - minx + 1 + ribear) > 0)
		  fprintf(obuf, "   (CHARWD R %1.6f)\n", (float)x/33.33);
		if ((x = -miny + base + 1 + HLWIDTH) > 0)
		  fprintf(obuf, "   (CHARHT R %1.6f)\n", (float)x/33.33);
		if ((x = maxy - base ) > 0)
		  fprintf(obuf, "   (CHARDP R %1.6f)\n", (float)x/33.33);
		if ((x = co2 - maxx ) > 0)
		  fprintf(obuf, "   (CHARIC R %1.6f)\n", (float)x/33.33);
		fprintf(obuf, "   )\n");
	}

	fclose(obuf);

}

/******************/

main()
{	char sp;
	char key(), conin();
/* stuff from xum */

	if (term_setup() != 0)
	{
		printf("Warning: This program needs to have the\n");
		printf("TERM environment variable setup before\n");
		printf("being run. Make sure this is done before\n");
		printf("executing eplot again (set yourself up like\n");
		printf("you were going to use vi).\n");
		exit(1);
	}

	printf("Terminal is a %s.\n", term_name);
	if (!strcmp(term_name, "w300")) w300 = TRUE;
	if (!w300)
	{	upperdot='^'; lowerdot='j'; bothdot='g';
	}

	tty_fid = raw_tty("/dev/tty");
	if (tty_fid == -1)
	{
		printf("Error opening terminal for raw i/o\n");
		exit(1);
	}


	debug = 0;

	stsize = STSIZE;

	printf("\r\nEplot - (c) Greg Lee, Oct. 1984\r\n\n");

	base = 9;
	ytop = 22;
	lebear = ribear = 2;
	strcpy(ftname,"Work");
	letter = currch = duplflag = 0;

	storept = x1 = x2 = y1 = y2 = a2 = d2 = 1;
	serifs = 2;

	begy = 50-12;	begx = 50-39;

	do
	{	printf("\r\n");
		if (currlet > ' ') printf("%c* ",currlet);
			else printf("^%c* ",currlet+64);
		cmd = key();
	switch (cmd)
	{
	case 'e': edit(); break;
	case 'c': printf("Create letter? : ");
		  /*scanf("%c",&letter);*/ letter = conin(); putchar(letter);
		  cindex[letter] = storept;
		  sto2(0,0);
		  break;
	case 'r': rdchr(); break;
	case 'z': /* setmem(cindex, 128*2+2+stsize, 0);*/
		  bzero(cindex, 128*sizeof(int));
		  bzero(store, STSIZE);
		  storept = 1;
		  printf("zapped");
		  break;
	case 'p': plotlet(); break;
	case 'v': showgr(); showbits(); break;
	case 'b': gvar("Base",&base); break;
	case 'h': gvar("Height",&ytop); break;
	case 'd': gvar("Depth",&ybot); break;
	case 'f': printf("FACTORS:\r\n");
		  gvar("X1",&x1);
		  gvar("X2",&x2);
		  gvar("Y1",&y1);
		  gvar("Y2",&y2);
		  break;
	case 't': duplflag = !duplflag;
		  chkdupl();
		  if (!duplflag) printf("not "); printf("tall");
		  break;
	case 'L': printf("LINE STYLE:\r\n");
		  gvar("X Heavy",&xheavy);
		  gvar("X Light",&xlight);
		  gvar("Y Heavy",&yheavy);
		  gvar("Y Light",&ylight);
		  break;
	case 'I': printf("ITALIC:\r\n");
		  gvar("I1",&i1);
		  gvar("I2",&i2);
		  break;
	case 'B': printf("BEARINGS:\r\n");
		  gvar("Left",&lebear);
		  gvar("Right",&ribear);
		  break;
	case 'S': gvar("serifs",&serifs);
		  break;
	case 'n': printf(".chr name(%s) = ",ftname);
		  getnam(ftname);
		  /*strcat(ftname,".chr");*/
		  break;
	case 's': savechr(); break;
	case 'a': saveasm(); break;
	case 'P': saveps(); break;
	case 'T': savetfm(); break;
	case '?': cmdinfo();
		  printf("\r\nstore room %d",
			stsize-storept);
		  break;
	case '\\': printf("debug");
		  if (debug) printf(" off");
		  debug = !debug;
	case 'q':
	case 'x': break;
	default: conout('?'); printf("%c cmd %d?",cmd,cmd); break;
	}
	}
	while (cmd != 'x' && cmd != 'q');

	reset_tty(tty_fid);

}

cmdinfo()
{
    printf("\r\nRead   Plot      Name .chr   mag.Factors\r\n");
	printf("Zap    Edit      Save .chr  ^Line style\r\n");
	printf("Base   Vdisplay ^Serifs     ^Ps file\r\n");
	printf("Height eXit/Quit Tall       ^Tfm file\r\n");
	printf("Depth ^Italic   ^Bearings    Asm file\r\n");
}


gvar(s,v)
char *s; int *v;
{	char is[80];

	printf(s);
	printf("(%d) = ", *v);
	cook_tty(tty_fid);
	gets(is);
	if (is[0] >= '0' && is[0] <= '9') sscanf(is,"%d",v);
	uncook_tty(tty_fid);
}

plotlet()
{
	printf("Plot a letter? : ");
	/*scanf("%c", &letter);*/
	letter = 0x7f & getchar(); putchar(letter);
	currlet = letter;
	lplot(letter);
}


edit()
{	/*if (!currlet) plotlet();
	else*/ lplot(currlet);

	penplace = 0;

	edscr();

	gxy(0,22);
}

displine(row)
char row;
{
	y = begy + row;

	for (x = begx; x <= begx+79; x++)
		if (gr[x][y])
		{	at(x,y);
			gxy(ecol, erow);
			putchar(gr[x][y]);
		}
}


at(x,y)
int x,y;
{	x -= begx;	y -= begy;
	if (x >= 0 && x <= 79 && y >= 0 && y <= 23)
	{ ecol = x; erow = y; return(TRUE); }
	return(FALSE);

}

rdchr()
{	char fname[20];


	printf(".chr file?  : ");
	getnam(fname);
	if (!fname[0]) return;
	strcat(fname, ".chr");
	if ((ibuf = fopen(fname, "r")) == NULL) return;

	while (fgets(ins, 90, ibuf))
	{	itempt = 0;
		while (itemtype = scnitem())
		switch(itemtype)
		{
		case 1: if (debug) printf("\r\nCnum %d: ", cnum);
			letter = 0;
			break;
		case 2: if (debug) printf("\r\nChar '%c': ", letter);
			cindex[letter] = storept;
			break;
		case 3: if (debug) printf("(%d,%d)", co1, co2);

			if (i1 && i2) co1 += fadjust(co2,i1,i2)-50;

			co1 = fadjust(co1,x1,x2) - 50;
			co2 = fadjust(co2,y1,y2) - 50;
			sto2(co1 + 64, co2 + 64);
			break;
		case 4: if (debug) printf(" Pen Up\r\n   ");
			sto2(0, 64);
			break;
		case 5: if (debug) printf(" End Char");
			sto2(0, 0);
			break;
		case 6: if (debug) printf(" Hair");
			sto2(0, 'H');
			break;
		case 65:/* if (debug) printf(" Bounds (-%d,%d)", co1, co2);
			co2 = fadjust(co2,x1,x2) - 50;
			sto2(0, co2 + 64);
			break; */
		case 66:if (debug) printf("(ignored)");
			break;
		}
	}


	fclose(ibuf);

	printf("\r\nUsed %u of buffer (%d remaining).", storept,stsize-storept);
}

sto2(x,y)
char x, y;
{
	if (letter < 0 || letter > 127) return;
	if (storept > stsize - 2) return;

	store[storept++] = x;
	store[storept++] = y;

}

scnitem()
{	char dc;

	while (ins[itempt] == ' ' || ins[itempt] == '\t') itempt++;

	if (!ins[itempt]) return(0);
	if (ins[itempt] == '\n') return(0);

	if (sscanf(ins + itempt, "%d:", &cnum))
	{	match(':');	return(1); }

	if (sscanf(ins + itempt, "'%c':", &letter))
	{	match(':');	return(2); }

	if (sscanf(ins + itempt, "^%c':", &letter))
	{	match(':');	letter &=  0x1F; return(2); }

	if (sscanf(ins + itempt, "\\%o:", &letter))
	{	match(':');	return(2); }

	if (sscanf(ins + itempt, "Bounds(%d,%d)", &co1, &co2))
	{	match(')');	return(65); }

	if (sscanf(ins + itempt, "s(%d,%d)", &co1, &co2))
	{	match(')');	if (serifs) return(3); return(66); }

	if (sscanf(ins + itempt, "S(%d,%d)", &co1, &co2))
	{	match(')');	if (serifs>1) return(3); return(66); }

	if (sscanf(ins + itempt, "(%d,%d)", &co1, &co2))
	{	match(')');	return(3); }

	if (sscanf(ins + itempt, "Pen Up%c", &dc))
	{	match('p');	return(4); }

	if (sscanf(ins + itempt, "End Char%c", &dc))
	{	match('r');	return(5); }

	if (sscanf(ins + itempt, "Hair%c", &dc))
	{	match('r');	return(6); }

	if (sscanf(ins + itempt, "Base(%d)", &base))
	{	match(')');	obase = base; return(66); }

	if (sscanf(ins + itempt, "x Height(%d)", &xheight))
	{	match(')');	return(66); }

	if (sscanf(ins + itempt, "Height(%d)", &ytop))
	{	match(')');	return(66); }

	if (sscanf(ins + itempt, "Depth(%d)", &ybot))
	{	match(')');	return(66); }

	return(0);

}

match(c)
char c;
{	while (ins[itempt++] != c);
}

lplot(let)
char let;
{	char c1, c2, penstate;

	currch = let;
	chkdupl();

	/*if (let <= 1 || let > 127) return;*/
	if (let < 0 || let > 127) return;

	if (!(xpt = cindex[let])) return;
	/*if (kbhit()) return;*/

	cleargr();

	for (penstate = 0 ; (c1 = store[xpt++])
			  + (c2 = store[xpt++]) ; )
	if (!c1)
	{	penstate = 0;
		if (c2 == 'H') hairflag = TRUE;
	}

	else
	{	lx = cx; ly = cy;
		cx = c1 - 64; cy = c2 - 64;

		if (penstate) drawline();

		penstate = 1;

	}

	lastplace = xpt - 2;


/*	showgr(); */

}

goline(here)
unsigned here;
{	char c1, c2;

	if (!here) return(0);

	c1 = store[here];
	c2 = store[here + 1];
	if (!c1) return(0);

	penplace = here;


	if (at(fadjust(c1-64,x1,x2), fadjust(c2-64,y1,y2)))
	{	xpos = c1-64;
		ypos = c2-64;
		return(TRUE);
	}
	return(0);
}

changeit()
{
	if (!penplace) return;

	if (!store[penplace]) return;

	store[penplace] = xpos + 64;
	store[penplace+1] = ypos + 64; 
}

delline()
{	if (!penplace) return;
	if (!store[penplace-2] || !store[penplace+2]) delpoint();
	else
	{	store[penplace] = 0;
		store[penplace+1] = 64;
		penplace = 0;
		penchar = 0;
	}
}


delpoint()
{
	if (!penplace || !store[penplace]) return;

/*	movmem(store+penplace+2, store+penplace, storept-penplace-2);*/
	bcopy(store+penplace+2, store+penplace, storept-penplace-2);
	storept -= 2;

	for (letter = 0; letter <= 127; letter++)
		if (cindex[letter] > penplace)
			cindex[letter] -= 2;

	lastplace -= 2;
}

makeroom(r)
int r;
{
	bcopy(store+penplace, store+penplace+r, storept-penplace);
	storept += r;

	for (letter = 0; letter <= 127; letter++)
		if (cindex[letter] > penplace)
			cindex[letter] += r;
	lastplace += r;
}

newpoint()
{	char puflag;


	if (!penplace)
	{
		penplace = lastplace;
		makeroom(6);

		store[penplace] = 0;
		store[penplace+1] = 64;
		store[penplace+2] = xpos + 64;
		store[penplace+3] = ypos + 64; 
		store[penplace+4] = xpos + 64;
		store[penplace+5] = ypos + 64;

		penplace += 4;
	}
	else
	{
		if (store[penplace]) penplace += 2;
		makeroom(2);
		store[penplace] = xpos + 64;
		store[penplace+1] = ypos + 64; 

	}
}


makehair()
{	char prev;


	if (!penplace || !store[penplace] || !store[penplace+2]) return;

	if (store[penplace-2]) prev = 2; else prev = 0;

	penplace += prev;
	makeroom(prev + 2);

	store[penplace] = 0;
	store[penplace+1] = 'H';

	if (prev)
	{	store[penplace+2] = store[penplace-2];
		store[penplace+3] = store[penplace-1];
	}

	penplace += 2;
}

xcontract()
{	unsigned pt;
	char key();

	key();

	for (pt = currlines['1'-' ']; store[pt] || store[pt+1]; pt += 2)
	if (store[pt])
	switch (reallet)
	{
	case 'r': if (store[pt] > xpos + 64) store[pt]--;
		  break;
	case 'R': if (store[pt] > xpos + 64) store[pt]++;
		  break;
	case 'l': if (store[pt] < xpos + 64) store[pt]++;
		  break;
	case 'L': if (store[pt] < xpos + 64) store[pt]--;
		  break;
	case 'u': if (store[pt+1] < ypos + 64) store[pt+1]++;
		  break;
	case 'U': if (store[pt+1] < ypos + 64) store[pt+1]--;
		  break;
	case 'd': if (store[pt+1] > ypos + 64) store[pt+1]--;
		  break;
	case 'D': if (store[pt+1] > ypos + 64) store[pt+1]++;
		  break;
	default:  break;
	}
}


setpos()
{
	return(at(fadjust(xpos,x1,x2), fadjust(ypos,y1,y2)));
}


cleargr()
{
	clrscrn();
	bzero(gr, 100 * 100);
	bzero(currlines, 96*2);
	linelet = '0';
	penwidth = abs(penwidth);
}


drawline()
{
	if (debug) printf("\r\nDrawing from (%d,%d) to (%d,%d).", lx,ly,cx,cy);

	linelet++;
	if (linelet > '9' && linelet < 'A') linelet = 'A';
	else if (linelet > 'Z' && linelet < 'a') linelet = 'a';
	else if (linelet > 'z') linelet = '!'+1;

	currlines[linelet-' '] = xpt-4;


	if (abs(cy) <= abs(ly))
	{	llx = lx;
		lly = ly;
		ccx = cx;
		ccy = cy;
	}
	else
	{	llx = cx;
		lly = cy;
		ccx = lx;
		ccy = ly;
	}


	x = fadjust(llx,x1,x2);
	ccx = fadjust(ccx,x1,x2);
	deltax = abs(ccx - x);
	if (ccx >= x) xinc = 1; else xinc = -1;

	y = fadjust(lly,y1,y2);
	ccy = fadjust(ccy,y1,y2);
	deltay = abs(ccy - y);
	if (ccy >= y) yinc = 1; else yinc = -1;

	drawdot();

	if (deltax >= deltay) doforx();
	else dofory();

	hairflag = 0;
}

fadjust(u,f1,f2)
int u,f1,f2;
{
	return( (u * f1 + f2-1)/f2 + 50);
}



drawdot()
{
	if (x<0 || x>99 || y<0 || y>99) return;
	if (gr[x][y]) return;
	gr[x][y] = linelet;
	if (at(x,y))
	{	gxy(ecol,erow);
		putchar(linelet);
	}
}



doforx()
{	int err, i;

	err = deltax / 2;
	i = deltax;

	while (i--)
	{	err += deltay;
		if (err >= deltax)
		{	err -= deltax;
			y += yinc;
			if (xheavy) drawdot();
			if (xlight) {x += xinc; continue;}
		}
		x += xinc;
		if (hairflag && (i&1)) continue;
		drawdot();
		if (debug) printf("\r\n   @(%d,%d) [%d,%d]", x-50, y-50, x,y);
	}
	
}

dofory()
{	int err, i;

	gr[x][y] = linelet;

	err = deltay / 2;
	i = deltay;

	while (i--)
	{	err += deltax;
		if (err >= deltay)
		{	err -= deltay;
			x += xinc;
			if (yheavy) drawdot();
			if (ylight) { y+= yinc; continue; }
		}
		y += yinc;
		if (hairflag && (i&1)) continue;
		drawdot();
		if (debug) printf("\r\n   @(%d,%d) [%d,%d]", x-50, y-50, x,y);
	}
	
}

showgr()
{	int x, y;

	minx = miny = 99;
	maxx = maxy = 0;

	for (y = 0; y < 100; y++)
	{	for (x = 0; x < 100; x++)
		if (gr[x][y])
		{	if (x < minx) minx = x;
			if (y < miny) miny = y;
			if (x > maxx) maxx = x;
			if (y > maxy) maxy = y;
		}
	}

	if (maxx - minx > KEEPSIZE-1) maxx = minx + KEEPSIZE-1;
	rdedf();
}




char conin()
{	return (0x7f & getchar());
}

conout(c)
char c;
{	putchar(c);
}



char key()
{	char c;

	reallet = c = 0x7f & getchar();
	if (c < ' ') c += '@';
		else if (c >='A' && c <= 'Z')
		c += ' ';
	if (c == 'J') return(key());
	return(c);
}


dtrans(c, upper)
char c, upper;
{	if (c == 0x20 || c == 0x5F) return(c);

	if (c >= 0x60) c -= 0x20;
	else if (c < 0x20) c += 0x20;

	if (upper) return(c);

	if (c < 0x40) c -= 0x20;
	else c += 0x20;

	return(c);
}

chkdupl()
{
/*	duplflag = fix[0] & PRPTNL; */
	if (duplflag) currch = dtrans(currch, TRUE);
}

rdedf()
{	char c;
	int lbear, rbear;
	int i, y, lasty, dcurrlen;

	if  (duplflag)	currch = dtrans(currch, TRUE);

	lbear = rbear = 0;

	bzero(ckeep, 6*KEEPSIZE);
	row = currlen = 0; dcurrlen = 10000;

	y = base + 51 - ytop;
	lasty = y + (duplflag ? 47 : 23);
	if (lasty >= 100) lasty = 99;

	for ( ; y <= lasty; row++,y++)
	{
		for (i = minx, col = lbear; i <= maxx; col++,i++)
		 if (gr[i][y]) chngbit(TRUE);

		if (currlen < col + rbear)
			currlen = col + rbear;
	}
}

chngbit(biton)
char biton;
{	if (duplflag && row > 23) cchngbit(3, row - 24, biton);
	else cchngbit(0, row, biton);
}

cchngbit(ib, row, biton)
char ib, row;
char biton;
{	unsigned char bit, mask;
	
	bit = row / 3;
	mask = 128 >> bit; 
	cbyte = ckeep[ib + (row % 3)][col];
	if (biton) cbyte |= mask;
		else cbyte &= ~mask;
	ckeep[ib + (row % 3)][col] = cbyte;
}

gxy(x,y)
char x,y;
{	if (w300)
	{	conout(ESC); conout('=');
		conout(y+' '); conout(x+' ');
	} else mov_cur(x,y);
}

clrscrn()
{
	if (w300) conout(0x1A); else cls();
	penchar = 0;
	sleep(1);
}

showbits()
{
	clrscrn();
	graph(TRUE);
	gxoff = 4;

	if (duplflag) gyoff = 0; else gyoff = 6;
	sshowbits(0);
	if (duplflag)
	{	gyoff = 12;
		sshowbits(3);
		gyoff = 0;
	}
	graph(FALSE);
	gxy(0,22);
}

sshowbits(ib)
char ib;
{	unsigned char i, j, bit, mask, r;

     if (currlen > 0)
     {
	color(RED,WHITE);
	if (gyoff == 6)
	{	gxy(gxoff-1,gyoff-1);
		conout(w300 ? '2':'+');
		for (i = 0; i < currlen; i++)
			conout(w300 ? ':':'-');
		conout(w300 ? '3':'+');
	}
	for (i = gyoff; i < (gyoff+12); i++)
	  {
		gxy(gxoff-1,i); conout(w300 ? '6':'|');
		gxy(currlen+gxoff,i); conout(w300 ? '6':'|');
	  }
	if (gyoff == 6)
	{	gxy(gxoff-1,gyoff+12);
		conout(w300 ? '1':'+');
		for (i = 0; i < currlen; i++)
			conout(w300 ? ':':'-');
		conout(w300 ? '5':'+');
	}
	color(BLACK,WHITE);
     }

	for (i = 0; i < 3; i++)
	 for (j = 0; j < currlen; j++)
	  {	cbyte = ckeep[ib+i][j];
		cbyteo = ckeep[ib + ((i+1) % 3)][j];
		mask = 0x80;
		for (bit = 0; bit < 8; bit++, mask >>= 1)
		 if (
			 /* if row is even */
			!((r = i + bit*3) & 1)
			 /* and there is a dot in this row or the next */
		     && ((mask & cbyte) | ((i==2 ? mask>>1: mask) & cbyteo))
		    )
		 {	gxy(j+gxoff, r/2+gyoff);
			 /* both dots? */
			if ((mask & cbyte)&& ((i==2 ? mask>>1: mask) & cbyteo))
				conout(bothdot);
			/* dot in this row only? */
			else if (mask & cbyte)
				conout(upperdot);
			else conout(lowerdot);	/* must be dot in next row */
		 }
	  }
}


graph(yes)
char yes;
{	if (!w300) return;

	conout(ESC);	conout('H');
	if (yes) conout(2); else conout(3);
	conout(ESC);	conout('`');	/* cursor off/on	*/
	if (yes) conout('0'); else conout('1');
	if (!yes) color(BLACK, WHITE);
}

color(fc, bc)
char fc, bc;
{	if (!w300) return;

	conout(ESC);	conout('g');
	conout(fc);	conout(bc);
}

checkpoint()
{	int sx, sy;

	if (!penplace) return;
	sx = xpos; sy = ypos;
	if (!goline(penplace)) return;
	penchar = gr[begx+(pxc=ecol)][begy+(pyc=erow)];
	xpos = sx; ypos = sy;
	gxy(pxc,pyc);
	conout(penchar);
}


edscr()
{	char key();

	oldpplace = 0;
	xpos = 0; ypos = 1;
	cmd = 'E';
	while (cmd != 'M')
	{  if (cmd >= 'A' && cmd <= 'Z')
		switch (cmd)
		{
	  	case 'S': xpos--; break;
		case 'D': xpos++; break;
		case 'E': ypos--; break;
		case 'X': ypos++; break;
		case 'R': ypos -= 8; break;
		case 'C': ypos += 8; break;
	  	case 'A': xpos -= 8; break;
		case 'F': xpos += 8; break;
		case 'L': key(); currlet = reallet; penplace = 0;
		case 'N': lplot(currlet); break;
		case 'W': begy--;
			  gxy(0,0); printf("                 ");
			  conout(ESC); conout('E');
			  displine(0);
			  break;
		case 'Z': begy++;
			  gxy(0,0); conout(ESC); conout('R');
			  displine(23);
			  break;
		case 'B': if (penplace) goline(penplace-2); break;
		case 'P': changeit();
			  lplot(currlet); break;
		case 'I': newpoint();
			  lplot(currlet); break;
		case 'U': penplace = 0; break;
		case 'G': delpoint();
			  lplot(currlet); break;
		case 'Y': delline();
			  lplot(currlet); break;
		case 'T': xcontract();
			  lplot(currlet); break;
		case 'Q': begy = 50-12;	begx = 50-39;
			  lplot(currlet); break;
		case 'H': makehair();
			  lplot(currlet); break;
		default:  conout(7);
	    	}
		else switch(cmd)
		{
		case ' ': if (penplace) goline(penplace+2); break;
		default:  goline(currlines[reallet-' ']);
		}

		if (penplace != oldpplace && penchar)
		{	gxy(pxc,pyc);
			conout(penchar);
			penchar = pxc = pyc = 0;
		}

		if (penplace)
		{
			color(WHITE,BLUE);
			checkpoint();
		}
		else	color(RED,YELLOW);
		gxy(0,0);
		printf("   (%3d,%3d)    ", xpos, ypos);
		color(BLACK,WHITE);
		oldpplace = penplace;

		if (!setpos()) { xpos = 0; ypos = 0; setpos(); }

		gxy(ecol, erow);

		cmd = key();
	}
}



max(x,y)
int x,y;
{	if (x >= y) return(x);
	return(y);
}

min(x,y)
int x,y;
{	if (x >= y) return(y);
	return(x);
}

