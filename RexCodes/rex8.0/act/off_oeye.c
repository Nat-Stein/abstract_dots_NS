/*
 *-----------------------------------------------------------------------*
 * NOTICE:  This code was developed by the US Government.  The original
 * versions, REX 1.0-3.12, were developed for the pdp11 architecture and
 * distributed without restrictions.  This version, REX 4.0, is a port of
 * the original version to the Intel 80x86 architecture.  This version is
 * distributed only under license agreement from the National Institutes 
 * of Health, Laboratory of Sensorimotor Research, Bldg 10 Rm 10C101, 
 * 9000 Rockville Pike, Bethesda, MD, 20892, (301) 496-9375.
 *-----------------------------------------------------------------------*
 */

/*
 * Action to specify eye offsets.
 */
#include <stdio.h>
#include <x86/inline.h>
#include <sys/types.h>
#include "rexhdrs.h"
#include "int.h"

int
off_oeye(long hoff, long voff)
{
	int menu= m_offmenu;
	unsigned eflags;

	if((eflags= pswget()) & INTR_IF) InterruptDisable();

	if(menu && (m_oeho != NULLI)) i_b->oEyeHoff = m_oeho << TEN_TO_IC;
	else if(hoff != NULLI) i_b->oEyeHoff = hoff << TEN_TO_IC;

	if(menu && (m_oevo != NULLI)) i_b->oEyeVoff = m_oevo << TEN_TO_IC;
	else if(voff != NULLI) i_b->oEyeVoff = voff << TEN_TO_IC;

	oeho = i_b->oEyeHoff;
	oevo = i_b->oEyeVoff;

	if(eflags & INTR_IF) InterruptEnable();
	return(0);
}

int Poff_oeye(long *hoff, long *voff)
{
	off_oeye(*hoff, *voff);
	return(0);
}
