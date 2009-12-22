/*
 * func.c
 * This file is part of anyPaper
 *
 * Copyright (C) 2008 - Sergio Rodrigues de Oliveira Filho 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * "You should have received a copy of the GNU General Public License 
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

static int len (char *t)
{
	int i;
	for (i=0;t[i]!=0;i++);
	return i;
}

int spi(char *t, int b)
{
	int c,k=1,i,r=0;
	c=len(t);
	for (i=0;i!=c;i++)
	{
		if(t[c-1-i]=='-') r=(-1)*r;
		else
		{
			if(t[c-1-i]<0x40) r=(t[c-1-i]-0x30)*k+r;
			else
			{
				if(t[c-1-i]>=0x60) r=(t[c-1-i]-0x60+9)*k+r;
				else r=(t[c-1-i]-0x40+9)*k+r;
			}
		}
		k=b*k;
	}
	return r;
}

