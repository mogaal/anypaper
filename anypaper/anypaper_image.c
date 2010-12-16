/*
 * anypaper_image.c
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
 * SECTION:anypaper_image
 * @short_description: genarates the images used by anyPaper
 * @see_also: #AnypaperParameters
 * @stability: Stable
 * @include: anypaper_image.h
 *
 * This application class generates the images used by anyPaper
 * 
 * The images present in #AnypaperImage are created by anypaper_image_move() using the parameters passed to function and images previously created by anypaper_image_make(). The anypaper_image_make() function modifies some parameters passed by #AnypaperParameters (mainly positionx and positiony, for indicating the range allowed for the variables) so they must be updated.
 * 
 * Since: 2.0
 */

#include "anypaper_image.h"
#include "config.h"

#define ANYPAPER_IMAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ANYPAPER_TYPE_IMAGE, AnypaperImagePrivate))

struct _AnypaperImagePrivate {
	GdkPixbuf	*base;
	GdkPixbuf	*background;
};

G_DEFINE_TYPE (AnypaperImage, anypaper_image, G_TYPE_OBJECT);

static void
anypaper_image_finalize (GObject *gobject)
{
	AnypaperImage *self = ANYPAPER_IMAGE (gobject);

	G_OBJECT_CLASS (anypaper_image_parent_class)->finalize (gobject);
}

static void
anypaper_image_class_init (AnypaperImageClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->finalize = anypaper_image_finalize;

	g_type_class_add_private (klass, sizeof (AnypaperImagePrivate));
}

static void
anypaper_image_init (AnypaperImage *self)
{
	AnypaperImagePrivate *priv;

	self->priv = priv = ANYPAPER_IMAGE_GET_PRIVATE (self);

	priv->base = NULL;
	priv->background = NULL;
}

gint size (gint base, gint final)
{
	if (final%base == 0) return final;
	else return (final/base + 1) * base;
}

/**
 * anypaper_image_make:
 * @image: a #AnypaperImage
 * @parameters: a #AnypaperParameters
 * 
 * Generate a #GdkPixbuf background and a #GdkPixbuf base image with the parameters selected in #AnypaperParameters.
 *
 * Returns: %TRUE if the image file is successfully opened, %FALSE if an error occurs
 */

gboolean anypaper_image_make(AnypaperImage *image, AnypaperParameters *parameters)
{
	GdkPixbuf *tempbuf, *tempbuf2;gchar *endptr;
	gint wn, hn;

	gint md, screenw, screenh, w, h;
	gint ox=0, oy=0, x=0, y=0, dx=0, dy=0;
	gdouble sx=1, sy=1, scx = 1, scy = 1;
	gboolean openImage;

	g_object_unref(image->priv->base);
	g_object_unref(image->priv->background);

	md=parameters->style;
	screenw=parameters->width;
	screenh=parameters->height;
	
	tempbuf = gdk_pixbuf_new_from_file(parameters->file, NULL);
	if (tempbuf == NULL)
	{
		parameters->file = g_strdup_printf(ANYPAPER_DATA "noimage.png");
		tempbuf = gdk_pixbuf_new_from_file(parameters->file, NULL);
		openImage = FALSE;
	}
	else openImage = TRUE;

	if (md==5)
	{
		tempbuf2 = tempbuf;
		scx = parameters->scalex;
		scy = parameters->scaley;
		w = gdk_pixbuf_get_width(tempbuf2);
		h = gdk_pixbuf_get_height(tempbuf2);
		tempbuf = gdk_pixbuf_scale_simple(tempbuf2, scx*w, scy*h, parameters->interpolation);
		g_object_unref(tempbuf2);
		md = 1;
	}
	image->priv->base = gdk_pixbuf_add_alpha(tempbuf, FALSE, 0, 0, 0);
	g_object_unref(tempbuf);

	tempbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, (screenw), (screenh));
	gdk_pixbuf_fill(tempbuf, (guint32) g_ascii_strtoll(&(parameters->background)[1], NULL, 16)*256);//gdk_pixbuf_fill(tempbuf, spi(parameters->background,16)*256);
	image->priv->background = gdk_pixbuf_add_alpha(tempbuf, FALSE, 0, 0, 0);
	g_object_unref(tempbuf);
	w = gdk_pixbuf_get_width(image->priv->base);
	h = gdk_pixbuf_get_height(image->priv->base);

	if (md==0) 
	{
		tempbuf2 = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, (screenw), (screenh));
		tempbuf = gdk_pixbuf_add_alpha(tempbuf2, FALSE, 0, 0, 0);
		g_object_unref(tempbuf2);
		x=y=0;
		dx=screenw;
		dy=screenh;
		ox=oy=0;
		sx=(double) screenw/w;
		sy=(double) screenh/h;
		gdk_pixbuf_composite(image->priv->base, tempbuf,x, y, dx, dy, ox, oy, sx, sy, parameters->interpolation, 0xff);
		g_object_unref(image->priv->base);
		image->priv->base = tempbuf;
	}
	if (md==2)
	{
		tempbuf2 = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, size (w, screenw), size (h, screenh));
		tempbuf = gdk_pixbuf_add_alpha(tempbuf2, FALSE, 0, 0, 0);
		g_object_unref(tempbuf2);		
		while (x<(screenw))
		{
			while (y<(screenh))
			{
				gdk_pixbuf_composite(image->priv->base, tempbuf,x, y, w, h, x, y, sx, sy, parameters->interpolation, 0xff);
				y=y+(h);
			}
			x=x+(w);
			y=0;
		}
		g_object_unref(image->priv->base);
		image->priv->base = tempbuf;
		w = gdk_pixbuf_get_width(image->priv->base);
		h = gdk_pixbuf_get_height(image->priv->base);
		md = 1;
	}
	if (md==1)
	{
		if ((screenh-h)>=0) oy=((screenh-h))/2;
		if ((screenh-h)<0) oy=((screenh-h))/2;
		if ((screenw-w)>=0) ox=((screenw-w))/2;
		if ((screenw-w)<0) ox=((screenw-w))/2;
	}
	if (md==3)
	{
		hn=(h*screenw*100/w)/100;
		wn=(w*screenh*100/h)/100;
		if (hn<=((screenh)))
		{
			x=0;
			y=(screenh-hn)/2;
			dx=screenw;
			dy=hn;
			ox=0;
			oy=(screenh-hn)/2;
			sx=(double) screenw/w;
			sy=(double) screenw/w;
		}
		else
		{
			x=(screenw-wn)/2;
			y=0;
			dx=wn;
			dy=screenh;
			ox=(screenw-wn)/2;
			oy=0;
			sx=(double) screenh/h;
			sy=(double) screenh/h;
		}
		tempbuf = gdk_pixbuf_scale_simple(image->priv->base, sx*w, sy*h, parameters->interpolation);
		g_object_unref(image->priv->base);
		image->priv->base = tempbuf;
	}
	if (md==4)
	{
		hn=(h*screenw*100/w)/100;
		wn=(w*screenh*100/h)/100;
		x=y=0;
		dx=screenw;
		dy=screenh;
		if (hn<=((screenh)))
		{
			ox=(screenw-wn)/2;
			oy=0;
			sx=(double) screenh/h;
			sy=(double) screenh/h;
		}
		else
		{
			ox=0;
			oy=(screenh-hn)/2;
			sx=(double) screenw/w;
			sy=(double) screenw/w;
		}
		tempbuf = gdk_pixbuf_scale_simple(image->priv->base, sx*w, sy*h, parameters->interpolation);
		g_object_unref(image->priv->base);
		image->priv->base = tempbuf;
	}
	parameters->positionx = ox;
	parameters->positiony = oy;
	parameters->scalex = sx * scx;
	parameters->scaley = sy * scy;
	return openImage;
}

/**
 * anypaper_image_move:
 * @image: a #AnypaperImage
 * @parameters: a #AnypaperParameters
 * 
 * Generate the #GdkPixbuf image and the #GdkPixbuf preview present in #AnypaperImage struct with the parameters selected in #AnypaperParameters and the images created by anypaper_image_make()
 */

void anypaper_image_move(AnypaperImage *image, AnypaperParameters *parameters)
{
	GdkPixbuf *tempbuf;
	gdouble min, max;

	gint screenw, screenh, w, h;
	gint ox=0, oy=0, x=0, y=0, dx=0, dy=0;
	gdouble sx=1, sy=1;

	g_object_unref(image->image);
	g_object_unref(image->preview);

	screenw=parameters->width;
	screenh=parameters->height;

	w = gdk_pixbuf_get_width(image->priv->base);
	h = gdk_pixbuf_get_height(image->priv->base);

	if ((screenh-h)>=0)
	{
		y=((screenh-h))/2;
		oy=((screenh-h))/2;
		dy=h;
	}
	if ((screenh-h)<0)
	{
		y=0;
		oy=((screenh-h))/2;
		dy=(screenh);
	}
	if ((screenw-w)>=0)
	{
		x=((screenw-w))/2;
		ox=((screenw-w))/2;
		dx=w;
	}
	if ((screenw-w)<0)
	{
		x=0;
		ox=((screenw-w))/2;
		dx=(screenw);
	}

	image->image = gdk_pixbuf_copy(image->priv->background);

	ox=parameters->positionx;
	oy=parameters->positiony;
	if (ox>0) x=ox;
	else x=0;
	if (oy>0) y=oy;
	else y=0;
	gdk_pixbuf_composite(image->priv->base, image->image, x, y, dx, dy, ox, oy, sx, sy, parameters->interpolation, 0xff);
	image->preview = gdk_pixbuf_scale_simple(image->image, (gdouble)(120*screenw/screenh), (gdouble)(120), parameters->interpolation);
}

