/*
 * anypaper_parameters.h
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

#ifndef __ANYPAPER_PARAMETERS_H__
#define __ANYPAPER_PARAMETERS_H__

#include <gtk/gtk.h>
#include "config.h"

#define ANYPAPER_TYPE_PARAMETERS                  (anypaper_parameters_get_type ())
#define ANYPAPER_PARAMETERS(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ANYPAPER_TYPE_PARAMETERS, AnypaperParameters))
#define ANYPAPER_IS_PARAMETERS(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ANYPAPER_TYPE_PARAMETERS))
#define ANYPAPER_PARAMETERS_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), ANYPAPER_TYPE_PARAMETERS, AnypaperParametersClass))
#define ANYPAPER_IS_PARAMETERS_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), ANYPAPER_TYPE_PARAMETERS))
#define ANYPAPER_PARAMETERS_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), ANYPAPER_TYPE_PARAMETERS, AnypaperParametersClass))

typedef struct _AnypaperParameters        AnypaperParameters;
typedef struct _AnypaperParametersClass   AnypaperParametersClass;

typedef struct _AnypaperParametersPrivate AnypaperParametersPrivate;

/**
 * AnypaperParameters:
 * @file: filemane of source image
 * @style: selected style
 * @positionx: position of image in x direction
 * @positiony: position of image in y direction
 * @scalex: scale factor in x direction
 * @scaley: scale factor in y direction
 * @width: width of image
 * @height: height of image
 * @background: background color
 * @command: command line used for setting the wallpaper
 * @defaultfile: default file for saving the built wallpaper
 * @interpolation: type of interpolation used
 * 
 * Contains the parameters used to build the images created by anyPaper
 */

struct _AnypaperParameters
{
	GObject parent_instance;

	/* instance members */
	/*< public >*/
	gchar		*file;
	gint		style;
	gint		positionx;
	gint		positiony;
	gdouble		scalex;
	gdouble		scaley;
	gint		width;
	gint		height;
	gchar		*background;
	gchar		*command;
	gchar		*defaultfile;
	GdkInterpType	interpolation;
	/*< private >*/
};

struct _AnypaperParametersClass
{
  GObjectClass parent_class;
};

GType anypaper_parameters_get_type (void);

void anypaper_parameters_position_test (AnypaperParameters *parameters, gint rangex, gint rangey, gint positionx, gint positiony);

void anypaper_parameters_load (AnypaperParameters *parameters, gchar *filename);

void anypaper_parameters_write (AnypaperParameters *parameters, gchar *lastWallpaperFile, gchar *rcFile);

#endif /* __ANYPAPER_PARAMETERS_H__ */
