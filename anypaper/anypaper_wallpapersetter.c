/*
 * anypaper_wallpapersetter.c
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
 * SECTION:anypaper_wallpapersetter
 * @short_description: creates the wallpapersetter file and detects the wallpapersetter installed
 * @stability: Stable
 * @include: anypaper_wallpapersetter.h
 *
 * This application class creates the wallpapersetter file and detects the wallpapersetter installed
 * 
 * This application class detects the wallpapersetter listed in wallpapersetter file installed and stores this information in two lists, one for the wallpapersetter found and the other for the command (normally the fullscreen command). This class also creates the wallpapersetterfile that contains definitions for known wallpapersetters and the command used to set the wallpaper.
 * 
 * Since: 2.2
 */

#include "anypaper_wallpapersetter.h"
#include "config.h"

#define ANYPAPER_WALLPAPERSETTER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ANYPAPER_TYPE_WALLPAPERSETTER, AnypaperWallpapersetterPrivate))

struct _AnypaperWallpapersetterPrivate {
};

G_DEFINE_TYPE (AnypaperWallpapersetter, anypaper_wallpapersetter, G_TYPE_OBJECT);

static void
anypaper_wallpapersetter_finalize (GObject *gobject)
{
	AnypaperWallpapersetter *self = ANYPAPER_WALLPAPERSETTER (gobject);

	G_OBJECT_CLASS (anypaper_wallpapersetter_parent_class)->finalize (gobject);
}

static void
anypaper_wallpapersetter_class_init (AnypaperWallpapersetterClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->finalize = anypaper_wallpapersetter_finalize;
}

static void
anypaper_wallpapersetter_init (AnypaperWallpapersetter *self)
{
	AnypaperWallpapersetterPrivate *priv;

	self->wallpapersetter = NULL;
	self->command = NULL;
}

/**
 * anypaper_wallpapersetter_detect:
 * @wallpapersetter: a #AnypaperWallpapersetter
 * @filename: file that contains the wallpapersetters command definition
 * 
 * Generate a list of found wallpapersetters and other for the associated commands.
 */

void anypaper_wallpapersetter_detect (AnypaperWallpapersetter *wallpapersetter, gchar *filename)
{
	gchar *other, **field, **subfield, *wsetter, *command;
	int i,j=0,k,t=0;
	int par_counter=0;
	GList *current=NULL, *current_com=NULL;

	current = g_list_last (wallpapersetter->wallpapersetter);
	current_com = g_list_last (wallpapersetter->command);
	if (current)
		while (current)
		{
			g_free(current->data);
			g_free(current_com->data);
			current = g_list_previous(current);
			current_com = g_list_previous(current_com);
		}
	g_list_free(wallpapersetter->wallpapersetter); wallpapersetter->wallpapersetter = NULL;
	g_list_free (wallpapersetter->command); wallpapersetter->command = NULL;
	
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
				wsetter = subfield[0];
				command = subfield[1];
				if (g_find_program_in_path (wsetter) == NULL)
				{
					if (g_file_test (wsetter, G_FILE_TEST_EXISTS))
					{
						wallpapersetter->wallpapersetter = g_list_append(wallpapersetter->wallpapersetter, g_strdup(wsetter));
						wallpapersetter->command = g_list_append(wallpapersetter->command, g_strdup(command));
					} 
				}
				else
				{
					wallpapersetter->wallpapersetter = g_list_append(wallpapersetter->wallpapersetter, g_strdup(wsetter));
					wallpapersetter->command = g_list_append(wallpapersetter->command, g_strdup(command));
				}
				g_strfreev (subfield);
			
			}
			t=0;
		}
		g_free (other);
		g_strfreev (field);
	}
}

/**
 * anypaper_wallpapersetter_file:
 * @wallpapersetterFile: filename to store the wallpapersetters command definition
 * 
 * Generate a file with names of wallpapersetter and the associated commands.
 */

void anypaper_wallpapersetter_file(gchar *wallpapersetterFile)
{
	gchar *buffer;
	buffer=g_strdup_printf("\
Esetroot:	-scale\n\
wmsetbg:	-s -S\n\
feh:		--bg-scale\n\
hsetroot:	-fill\n\
chbg:		-once -mode maximize\n\
display:	-window root\n\
qiv:		--root_s\n\
xv:		-max -smooth -root -quit\n\
xsri:		--center-x --center-y --scale-width=100 --scale-height=100\n\
xli:		-fullscreen -onroot -quiet -border black\n\
xsetbg:		-fullscreen -border black\n");
	g_file_set_contents (wallpapersetterFile, buffer, -1, NULL);
	g_free (buffer);
}
