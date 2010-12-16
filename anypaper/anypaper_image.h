/*
 * anypaper_image.h
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

#ifndef __ANYPAPER_IMAGE_H__
#define __ANYPAPER_IMAGE_H__

#include <gtk/gtk.h>
#include "config.h"
#include "anypaper_parameters.h"

#define ANYPAPER_TYPE_IMAGE                  (anypaper_image_get_type ())
#define ANYPAPER_IMAGE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ANYPAPER_TYPE_IMAGE, AnypaperImage))
#define ANYPAPER_IS_IMAGE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ANYPAPER_TYPE_IMAGE))
#define ANYPAPER_IMAGE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), ANYPAPER_TYPE_IMAGE, AnypaperImageClass))
#define ANYPAPER_IS_IMAGE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), ANYPAPER_TYPE_IMAGE))
#define ANYPAPER_IMAGE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), ANYPAPER_TYPE_IMAGE, AnypaperImageClass))

typedef struct _AnypaperImage        AnypaperImage;
typedef struct _AnypaperImageClass   AnypaperImageClass;

typedef struct _AnypaperImagePrivate AnypaperImagePrivate;

/**
 * AnypaperImage:
 * @image: contains the real size image created by anyPaper
 * @preview: contains a thumbnail of the image created by anyPaper
 * 
 * Contains the generated images used by the program
 */

struct _AnypaperImage
{
	GObject parent_instance;
	/* instance members */
	/*< public >*/
	GdkPixbuf *image;
	GdkPixbuf *preview;

	/*< private >*/
	AnypaperImagePrivate *priv;
};

struct _AnypaperImageClass
{
	GObjectClass parent_class;
};

GType anypaper_image_get_type (void);

gboolean anypaper_image_make (AnypaperImage *image, AnypaperParameters *parameters);

void anypaper_image_move (AnypaperImage *image, AnypaperParameters *parameters);

#endif
