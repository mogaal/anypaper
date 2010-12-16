/*
 * anypaper_parameters.c
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

/**
 * SECTION:anypaper_parameters
 * @short_description: manipulates the parameters used in anyPaper
 * @stability: Stable
 * @include: anypaper_parameters.h
 *
 * This application class manipulates the parameters used in anyPaper, also loads and saves configuration files
 * 
 * Since: 2.0
 */

#include "anypaper_parameters.h"
#include "config.h"

G_DEFINE_TYPE (AnypaperParameters, anypaper_parameters, G_TYPE_OBJECT);

/**
 * anypaper_parameters_position_test:
 * @parameters: a #AnypaperParameters
 * @rangex: half of the displacement allowed in x direction
 * @rangey: half of the displacement allowed in y direction
 * @positionx: the selected position on x position
 * @positiony: the selected position on x position
 * 
 * Sets the new position values if they are valid.
 */

void anypaper_parameters_position_test (AnypaperParameters *parameters, gint rangex, gint rangey, gint positionx, gint positiony)
{
	if (((positionx < 0) && (positionx >= 2*rangex)) || ((positionx >= 0) && (positionx <= 2*rangex))) parameters->positionx=positionx;
	if (((positiony < 0) && (positiony >= 2*rangey)) || ((positiony >= 0) && (positiony <= 2*rangey))) parameters->positiony=positiony;
}

static int option_style (char *buffer)
{
	int c;
	if ((buffer[0]=='F')||(buffer[0]=='f')) c=0;
	if ((buffer[0]=='N')||(buffer[0]=='n')) c=1;
	if ((buffer[0]=='T')||(buffer[0]=='t')) c=2;
	if ((buffer[0]=='A')||(buffer[0]=='a')) c=3;
	if ((buffer[0]=='S')||(buffer[0]=='s')) c=4;
	if ((buffer[0]=='C')||(buffer[0]=='c')) c=5;
	return c;
}

static GdkInterpType option_interpolation (char *buffer)
{
	GdkInterpType interpolation;
	if ((buffer[11]=='N')||(buffer[11]=='n')) interpolation=GDK_INTERP_NEAREST;
	if ((buffer[11]=='T')||(buffer[11]=='t')) interpolation=GDK_INTERP_TILES;
	if ((buffer[11]=='B')||(buffer[11]=='b')) interpolation=GDK_INTERP_BILINEAR;
	if ((buffer[11]=='H')||(buffer[11]=='h')) interpolation=GDK_INTERP_HYPER;
	return interpolation;
}

void anypaper_parameters_write (AnypaperParameters *parameters, gchar *lastWallpaperFile, gchar *rcFile);

static void
anypaper_parameters_finalize (GObject *gobject)
{
	AnypaperParameters *self = ANYPAPER_PARAMETERS (gobject);

	G_OBJECT_CLASS (anypaper_parameters_parent_class)->finalize (gobject);
}

static void
anypaper_parameters_class_init (AnypaperParametersClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->finalize = anypaper_parameters_finalize;
}

static void anypaper_parameters_init (AnypaperParameters *self)
{
	self->file = g_strdup_printf(ANYPAPER_DATA "noimage.png");
	self->style = 3;
	self->positionx = 0;
	self->positiony = 0;
	self->scalex = 1;
	self->scaley = 1;
	self->width = 1;
	self->height = 1;
	self->background = g_strdup_printf("#000000");
	self->command = g_strdup_printf("Set_Command");
	self->defaultfile = g_strdup_printf("%s/.anypaper/output.png", g_get_home_dir ());
	self->interpolation = GDK_INTERP_BILINEAR;
}

/**
 * anypaper_parameters_load:
 * @parameters: a #AnypaperParameters
 * @filename: name of file with the parameters to be loaded
 * 
 * Sets the values of parameters on file in #AnypaperParameters.
 */

void anypaper_parameters_load (AnypaperParameters *parameters, gchar *filename)
{
	gchar *other, **field, **subfield;
	int i,j=0,k,t=0;
	int par_counter=0;
	
	if (g_file_get_contents (filename, &other, NULL, NULL))
	{
		for (i=0;other[i]!=0;i++) if (other[i]=='\n') j++;
		field = g_strsplit (other, "\n", j);
	
		for (i=0;i!=j;i++)
		{
			for(k=0;(field[i][k]!=0);k++) if (field[i][k]==':')
			{
				t=1;
				break;
			}
			if((field[i][0]!='#')&&(t!=0))
			{
				subfield = g_strsplit (field[i], ":", 2);
				g_strstrip(subfield[0]);
				g_strstrip(subfield[1]);
				if (!g_ascii_strcasecmp(subfield[0],"File"))
				{
					parameters->file = g_strdup_printf("%s", subfield[1]);
					par_counter++;
				}
				if (!g_ascii_strcasecmp(subfield[0],"Style"))
				{
					parameters->style = option_style(subfield[1]);
					par_counter++;
				}
				if (!g_ascii_strcasecmp(subfield[0],"PositionX"))
				{
					parameters->positionx=g_ascii_strtoll(subfield[1], NULL, 10);//parameters->positionx=spi(subfield[1], 10);
					par_counter++;
				}
				if (!g_ascii_strcasecmp(subfield[0],"PositionY"))
				{
					parameters->positiony=g_ascii_strtoll(subfield[1], NULL, 10);//parameters->positiony=spi(subfield[1], 10);
					par_counter++;
				}
				if (!g_ascii_strcasecmp(subfield[0],"ScaleX"))
				{
					parameters->scalex=g_strtod (subfield[1],NULL);
					par_counter++;
				}
				if (!g_ascii_strcasecmp(subfield[0],"ScaleY"))
				{
					parameters->scaley=g_strtod (subfield[1],NULL);
					par_counter++;
				}
				if (!g_ascii_strcasecmp(subfield[0],"Width"))
				{
					parameters->width=g_ascii_strtoll(subfield[1], NULL, 10);//parameters->width=spi(subfield[1], 10);
					par_counter++;
				}
				if (!g_ascii_strcasecmp(subfield[0],"Height"))
				{
					parameters->height=g_ascii_strtoll(subfield[1], NULL, 10);//parameters->height=spi(subfield[1], 10);
					par_counter++;
				}
				if (!g_ascii_strcasecmp(subfield[0],"BackgroundColor"))
				{
					parameters->background = g_strdup_printf("%s", subfield[1]);
					par_counter++;
				}
				if (!g_ascii_strcasecmp(subfield[0],"Command")) parameters->command = g_strdup_printf("%s", subfield[1]);
				if (!g_ascii_strcasecmp(subfield[0],"DefaultFile")) parameters->defaultfile = g_strdup_printf("%s", subfield[1]);
				if (!g_ascii_strcasecmp(subfield[0],"Interpolation")) parameters->interpolation = option_interpolation(subfield[1]);
				g_strfreev (subfield);
			
			}
			t=0;
		}
		g_free (other);
		g_strfreev (field);
	}
}

/**
 * anypaper_parameters_write:
 * @parameters: a #AnypaperParameters
 * @lastWallpaperFile: name of file to be written with the lastwallpaper parameters in #AnypaperParameters
 * @rcFile: name of file to be written with the resource parameters in #AnypaperParameters
 * 
 * Saves the values of parameters in #AnypaperParameters on file.
 */

void anypaper_parameters_write (AnypaperParameters *parameters, gchar *lastWallpaperFile, gchar *rcFile)
{
	gchar	*buffer, *style, *interpolation;

	if (parameters->style == 0) style=g_strdup_printf("Fullscreen");
	if (parameters->style == 1) style=g_strdup_printf("Normal");
	if (parameters->style == 2) style=g_strdup_printf("Tile");
	if (parameters->style == 3) style=g_strdup_printf("Adjust");
	if (parameters->style == 4) style=g_strdup_printf("Scale");
	if (parameters->style == 5) style=g_strdup_printf("Custom_Scale");
	buffer=g_strdup_printf("\
File:                   %s\n\
Style:                  %s\n\
PositionX:              %d\n\
PositionY:              %d\n\
ScaleX:			%f\n\
ScaleY:			%f\n\
Width:                  %d\n\
Height:                 %d\n\
BackgroundColor:        %s\n", parameters->file, style, parameters->positionx, parameters->positiony, parameters->scalex, parameters->scaley, parameters->width, parameters->height, parameters->background);
	g_file_set_contents (lastWallpaperFile, buffer, -1, NULL);
	g_free (buffer);
	if (parameters->interpolation == GDK_INTERP_NEAREST) interpolation=g_strdup_printf("GDK_INTERP_NEAREST");
	if (parameters->interpolation == GDK_INTERP_TILES) interpolation=g_strdup_printf("GDK_INTERP_TILES");
	if (parameters->interpolation == GDK_INTERP_BILINEAR) interpolation=g_strdup_printf("GDK_INTERP_BILINEAR");
	if (parameters->interpolation == GDK_INTERP_HYPER) interpolation=g_strdup_printf("GDK_INTERP_HYPER");
	buffer=g_strdup_printf("\
Command:                   %s\n\
DefaultFile:               %s\n\
Interpolation:             %s\n", parameters->command, parameters->defaultfile, interpolation);
	g_file_set_contents (rcFile, buffer, -1, NULL);
	g_free (buffer);
}

