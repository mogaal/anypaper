/*
 * anypaper_window.h
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

#ifndef __ANYPAPER_WINDOW_H__
#define __ANYPAPER_WINDOW_H__

#include <gtk/gtk.h>
#include "config.h"
#include "anypaper_command.h"
#include "anypaper_parameters.h"
#include "anypaper_image.h"
#include "anypaper_preview.h"
#include "anypaper_wallpapersetter.h"

#define ANYPAPER_TYPE_WINDOW                  (anypaper_window_get_type ())
#define ANYPAPER_WINDOW(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ANYPAPER_TYPE_WINDOW, AnypaperWindow))
#define ANYPAPER_IS_WINDOW(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ANYPAPER_TYPE_WINDOW))
#define ANYPAPER_WINDOW_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), ANYPAPER_TYPE_WINDOW, AnypaperWindowClass))
#define ANYPAPER_IS_WINDOW_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), ANYPAPER_TYPE_WINDOW))
#define ANYPAPER_WINDOW_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), ANYPAPER_TYPE_WINDOW, AnypaperWindowClass))

typedef struct _AnypaperWindow        AnypaperWindow;
typedef struct _AnypaperWindowClass   AnypaperWindowClass;

typedef struct _AnypaperWindowPrivate AnypaperWindowPrivate;

/**
 * AnypaperWindow:
 * @parameters: is a #AnypaperParameters
 * @image: is a #AnypaperImage
 * @preview: is a #AnypaperPreview
 * @wallpapersetter: is a #AnypaperWallpapersetter;
 *
 * Contains the necessary objects for operation of the anyPaper
 */

struct _AnypaperWindow
{
	GObject parent_instance;
	/* instance members */
	/*< public >*/
	AnypaperParameters		*parameters;
	AnypaperImage			*image;
	AnypaperPreview			*preview;
	AnypaperWallpapersetter		*wallpapersetter;

	/*< private >*/
	AnypaperWindowPrivate	*priv;
};

struct _AnypaperWindowClass
{
	GObjectClass parent_class;
};

GType anypaper_window_get_type (void);

void anypaper_window_set_position_range(AnypaperWindow *window, gint rangex, gint rangey);

void anypaper_window_create (AnypaperWindow *window);

#endif /* __ANYPAPER_WINDOW_H__ */
