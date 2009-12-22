/*
 * anypaper_window.c
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

#ifndef __ANYPAPER_PREVIEW_H__
#define __ANYPAPER_PREVIEW_H__

#include <gtk/gtk.h>
#include "config.h"
#include <gdk/gdkkeysyms.h>
#include "anypaper_image.h"

#define ANYPAPER_TYPE_PREVIEW                  (anypaper_preview_get_type ())
#define ANYPAPER_PREVIEW(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ANYPAPER_TYPE_PREVIEW, AnypaperPreview))
#define ANYPAPER_IS_PREVIEW(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ANYPAPER_TYPE_PREVIEW))
#define ANYPAPER_PREVIEW_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), ANYPAPER_TYPE_PREVIEW, AnypaperPreviewClass))
#define ANYPAPER_IS_PREVIEW_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), ANYPAPER_TYPE_PREVIEW))
#define ANYPAPER_PREVIEW_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), ANYPAPER_TYPE_PREVIEW, AnypaperPreviewClass))

typedef struct _AnypaperPreview        AnypaperPreview;
typedef struct _AnypaperPreviewClass   AnypaperPreviewClass;

typedef struct _AnypaperPreviewPrivate AnypaperPreviewPrivate;

struct _AnypaperPreview
{
	GObject parent_instance;

	/*< private >*/
	AnypaperPreviewPrivate *priv;
};

struct _AnypaperPreviewClass
{
	GObjectClass parent_class;

	/* class members */
};

GType anypaper_preview_get_type (void);

void anypaper_preview_create (AnypaperPreview *preview, AnypaperParameters *parameters, AnypaperImage *image);

#endif /* __ANYPAPER_PREVIEW_H__ */
