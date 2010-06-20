/*
 * anypaper_wallpapersetter.h
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

#ifndef __ANYPAPER_WALLPAPERSETTER_H__
#define __ANYPAPER_WALLPAPERSETTER_H__

#include <gtk/gtk.h>
#include "config.h"
#include "anypaper_parameters.h"

#define ANYPAPER_TYPE_WALLPAPERSETTER                  (anypaper_wallpapersetter_get_type ())
#define ANYPAPER_WALLPAPERSETTER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ANYPAPER_TYPE_WALLPAPERSETTER, AnypaperWallpapersetter))
#define ANYPAPER_IS_WALLPAPERSETTER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ANYPAPER_TYPE_WALLPAPERSETTER))
#define ANYPAPER_WALLPAPERSETTER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), ANYPAPER_TYPE_WALLPAPERSETTER, AnypaperWallpapersetterClass))
#define ANYPAPER_IS_WALLPAPERSETTER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), ANYPAPER_TYPE_WALLPAPERSETTER))
#define ANYPAPER_WALLPAPERSETTER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), ANYPAPER_TYPE_WALLPAPERSETTER, AnypaperWallpapersetterClass))

typedef struct _AnypaperWallpapersetter        AnypaperWallpapersetter;
typedef struct _AnypaperWallpapersetterClass   AnypaperWallpapersetterClass;

typedef struct _AnypaperWallpapersetterPrivate AnypaperWallpapersetterPrivate;

/**
 * AnypaperWallpapersetter:
 * @wallpapersetter: contains a list of wallpapersetter found
 * @command: contains a list of commands used for the wallpapersetter found
 * 
 * Contains a list of wallpapersetter found and a list of commands used for them
 */

struct _AnypaperWallpapersetter
{
	GObject parent_instance;
	/* instance members */
	/*< public >*/
	GList	*wallpapersetter;
	GList	*command;

	/*< private >*/
	AnypaperWallpapersetterPrivate *priv;
};

struct _AnypaperWallpapersetterClass
{
	GObjectClass parent_class;
};

GType anypaper_wallpapersetter_get_type (void);

void anypaper_wallpapersetter_detect (AnypaperWallpapersetter *wallpapersetter, gchar *filename);

void anypaper_wallpapersetter_file(gchar *wallpapersetterFile);

#endif
