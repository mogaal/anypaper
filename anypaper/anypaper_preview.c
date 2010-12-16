/*
 * anypaper_preview.c
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
 * SECTION:anypaper_preview
 * @short_description: builds a preview window
 * @see_also: #AnypaperImage, #AnypaperParameters
 * @stability: Stable
 * @include: anypaper_preview.h
 *
 * The application class that builds a preview window using the images and parameters provide by #AnypaperImage and #AnypaperParameters.
 * 
 * Since: 2.0
 */

#include "anypaper_preview.h"
#include "config.h"

#define ANYPAPER_PREVIEW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ANYPAPER_TYPE_PREVIEW, AnypaperPreviewPrivate))

struct _AnypaperPreviewPrivate {
	GtkWidget	*window;
	GtkWidget	*hbox;
	GtkWidget	*window2;
	GtkWidget	*button;
	GSource		*source;
	gint		state;
};

#define WINDOW_FULLSCREEN_TIMEOUT 5
#define WINDOW_FULLSCREEN_STATE 1
#define WINDOW_NORMAL_STATE 0
#define DELTA 30

G_DEFINE_TYPE (AnypaperPreview, anypaper_preview, G_TYPE_OBJECT);

static void
anypaper_preview_finalize (GObject *gobject)
{
	AnypaperPreview *self = ANYPAPER_PREVIEW (gobject);

	G_OBJECT_CLASS (anypaper_preview_parent_class)->finalize (gobject);
}

static void
anypaper_preview_class_init (AnypaperPreviewClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->finalize = anypaper_preview_finalize;

	g_type_class_add_private (klass, sizeof (AnypaperPreviewPrivate));
}

static void
anypaper_preview_init (AnypaperPreview *self)
{
	AnypaperPreviewPrivate *priv;

	self->priv = priv = ANYPAPER_PREVIEW_GET_PRIVATE (self);

	priv->state = 0;
	priv->source = NULL;
}

static void timeout_destroy (GSource *source)
{
	if (source != NULL) {
		g_source_destroy (source);
	}
	source = NULL;
}

static gboolean fullscreen_timeout_cb (AnypaperPreview *preview)
{
	gtk_widget_hide (preview->priv->hbox);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(preview->priv->window2), GTK_POLICY_NEVER, GTK_POLICY_NEVER);

	timeout_destroy (preview->priv->source);

	return FALSE;
}

static void image_fullscreen ( GtkWidget *widget, AnypaperPreview *preview)
{
	if (preview->priv->state == WINDOW_NORMAL_STATE) gtk_window_fullscreen (GTK_WINDOW(preview->priv->window));
	if (preview->priv->state == WINDOW_FULLSCREEN_STATE)
	{
		timeout_destroy(preview->priv->source);
		gtk_window_unfullscreen (GTK_WINDOW(preview->priv->window));
	}
}

static void image_fullscreen_button ( GtkWidget *widget, AnypaperPreview *preview)
{
	GtkWidget *button, *image_button;

	button = widget;

	if (preview->priv->state == WINDOW_NORMAL_STATE)
	{
		gtk_button_set_label (GTK_BUTTON(button), GTK_STOCK_LEAVE_FULLSCREEN);
		image_button = gtk_button_get_image (GTK_BUTTON(button));
		gtk_image_set_from_stock (GTK_IMAGE(image_button), GTK_STOCK_LEAVE_FULLSCREEN, GTK_ICON_SIZE_BUTTON);
		gtk_widget_show(image_button);gtk_widget_hide(preview->priv->hbox);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(preview->priv->window2), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
		preview->priv->state = WINDOW_FULLSCREEN_STATE;
	}
	else
	{
		gtk_button_set_label (GTK_BUTTON(button), GTK_STOCK_FULLSCREEN);
		image_button = gtk_button_get_image (GTK_BUTTON(button));
		gtk_image_set_from_stock (GTK_IMAGE(image_button), GTK_STOCK_FULLSCREEN, GTK_ICON_SIZE_BUTTON);
		gtk_widget_show(image_button);gtk_widget_show(preview->priv->hbox);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(preview->priv->window2), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		preview->priv->state = WINDOW_NORMAL_STATE;
	}
}

static gboolean mouse_motion (GtkWidget *widget, GdkEventMotion *event, AnypaperPreview *preview)
{
	if (!GTK_WIDGET_VISIBLE (preview->priv->hbox))
	{
		gtk_widget_show(preview->priv->hbox);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(preview->priv->window2), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	}

	if (preview->priv->state == WINDOW_FULLSCREEN_STATE)
	{

		timeout_destroy(preview->priv->source);

		preview->priv->source = g_timeout_source_new_seconds (WINDOW_FULLSCREEN_TIMEOUT);
		g_source_set_callback (preview->priv->source, fullscreen_timeout_cb, preview, NULL);
	
		g_source_attach (preview->priv->source, NULL);
	}
}

static gboolean window_key_press (GtkWidget *widget, GdkEventKey *event, AnypaperPreview *preview)
{
	switch (event->keyval)
	{
		case GDK_Escape:
			{
				if (preview->priv->state == 1)	gtk_widget_activate(GTK_WIDGET(preview->priv->button));
				else gtk_object_destroy(GTK_OBJECT(widget));
				timeout_destroy(preview->priv->source);
			}
		break;
	}
}

static void show_preview_quit ( GtkWidget *widget, AnypaperPreview *preview)
{
	timeout_destroy(preview->priv->source);
	gtk_object_destroy(GTK_OBJECT(preview->priv->window));
}

/**
 * anypaper_preview_create:
 * @preview: is a #AnypaperPreview
 * @parameters: contains the parameters used to make the images in #AnypaperImage
 * @image: contains the images created before
 * 
 * This function builds and shows the preview window 
 */

void anypaper_preview_create (AnypaperPreview *preview, AnypaperParameters *parameters, AnypaperImage *image_object)
{
	GtkWidget *window2, *event_box;
	GtkWidget *fixed;
	GtkWidget *image, *image_button;
	GdkScreen *screen;
	int screen_width, screen_height;

	screen = gdk_screen_get_default();
	screen_width = gdk_screen_get_width(screen);
	screen_height = gdk_screen_get_height(screen);

	preview->priv->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_add_events (GTK_WIDGET (preview->priv->window), GDK_KEY_PRESS_MASK);

	window2 = gtk_scrolled_window_new(NULL, NULL);
	gtk_window_set_title(GTK_WINDOW(preview->priv->window), "preview");

	preview->priv->window2 = window2;
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(window2), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	fixed = gtk_vbox_new(FALSE, 0);

	event_box = gtk_event_box_new ();
	gtk_widget_show (event_box);

	image = gtk_image_new_from_pixbuf(image_object->image); 
	gtk_container_add (GTK_CONTAINER (event_box), image);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW(window2), event_box);
	gtk_widget_add_events (GTK_WIDGET (event_box), GDK_POINTER_MOTION_MASK);
	gtk_box_pack_start (GTK_BOX(fixed), window2, TRUE, TRUE, 2);

	preview->priv->hbox = gtk_hbox_new(FALSE, 0);

	preview->priv->button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
	image_button = gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image (GTK_BUTTON(preview->priv->button), image_button);
	gtk_widget_show(image_button);
	g_signal_connect (G_OBJECT (preview->priv->button), "clicked",	G_CALLBACK (show_preview_quit), preview);
	gtk_box_pack_end (GTK_BOX(preview->priv->hbox), preview->priv->button, FALSE, FALSE, 2);

	preview->priv->button = gtk_button_new_from_stock (GTK_STOCK_FULLSCREEN);
	image_button = gtk_image_new_from_stock (GTK_STOCK_FULLSCREEN, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image (GTK_BUTTON(preview->priv->button), image_button);
	gtk_widget_show(image_button);
	g_signal_connect (G_OBJECT (preview->priv->button), "clicked",	G_CALLBACK (image_fullscreen), preview);
	g_signal_connect_after (G_OBJECT (preview->priv->button), "clicked",	G_CALLBACK (image_fullscreen_button), preview);
	gtk_box_pack_start (GTK_BOX(preview->priv->hbox), preview->priv->button, FALSE, FALSE, 2);

	gtk_box_pack_end (GTK_BOX(fixed), preview->priv->hbox, FALSE, FALSE, 2);
	gtk_container_add(GTK_CONTAINER(preview->priv->window), fixed);

	g_signal_connect (G_OBJECT (preview->priv->window), "key-press-event", G_CALLBACK (window_key_press), preview);
	g_signal_connect (G_OBJECT (event_box), "motion-notify-event", G_CALLBACK (mouse_motion), preview);

	gtk_widget_show_all(preview->priv->window);
	gtk_window_resize(GTK_WINDOW(preview->priv->window), MIN (screen_width/2, (preview->priv->window->allocation.width - window2->allocation.width + parameters->width + DELTA)), MIN (screen_height/2, (preview->priv->window->allocation.height - window2->allocation.height + parameters->height + DELTA)));
}
