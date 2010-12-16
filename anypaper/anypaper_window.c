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

/**
 * SECTION:anypaper_window
 * @short_description: builds the user interface and controls the operation of anyPaper
 * @see_also: #AnypaperImage, #AnypaperParameters, #AnypaperPreview
 * @stability: Stable
 * @include: anypaper_window.h
 *
 * The application class that builds the user interface and controls the operation of anyPaper. This class also links the objects required for the implementation of the anyPaper
 * 
 * Since: 2.0
 */

#include "anypaper_window.h"
#include "config.h"

#define ANYPAPER_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ANYPAPER_TYPE_WINDOW, AnypaperWindowPrivate))

struct _AnypaperWindowPrivate {
	GtkWidget *window;
	GtkWidget *preview;
	GtkWidget *file_entry;
	GtkWidget *combo;
	GtkWidget *custom_table;
	GtkWidget *spin5;
	GtkWidget *spin6;
	GtkWidget *cbutton;
	GtkWidget *label1, *spin1;
	GtkWidget *label2, *spin2;
	GtkWidget *spin3;
	GtkWidget *spin4;
	GtkWidget *def_entry;
	GtkWidget *com_entry;
	GtkWidget *combo_interpolation;
	gboolean custom_scale_connect_state;
};

G_DEFINE_TYPE (AnypaperWindow, anypaper_window, G_TYPE_OBJECT);

gboolean custom_scale_connect_state;
gboolean fullscreen, normal, tile, maximize, scale, custom, set, last, no_set;
int x_position, y_position, height, width;
double  x_scale, y_scale;
gchar *background_color, *lastwallpaperfile, *rcfile, **remaining_args;

static gboolean delete_event( GtkWidget *widget, gpointer data )
{
	gtk_main_quit();
	return FALSE;
}

static void
anypaper_window_finalize (GObject *gobject)
{
	AnypaperWindow *self = ANYPAPER_WINDOW (gobject);

	G_OBJECT_CLASS (anypaper_window_parent_class)->finalize (gobject);
}

static void
anypaper_window_class_init (AnypaperWindowClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->finalize = anypaper_window_finalize;

	g_type_class_add_private (klass, sizeof (AnypaperWindowPrivate));
}

static void
anypaper_window_init (AnypaperWindow *self)
{
	AnypaperWindowPrivate *priv;

	self->priv = priv = ANYPAPER_WINDOW_GET_PRIVATE (self);

	priv->custom_scale_connect_state = TRUE;
}

static void show_window_preview_cb( GtkWidget *widget, AnypaperWindow *window );

static void set_image_style_cb ( GtkWidget *widget, AnypaperWindow *window );

static void set_image_position_cb ( GtkWidget *widget, AnypaperWindow *window );

static void set_image_scale_cb ( GtkWidget *widget, AnypaperWindow *window );

void block_callback(AnypaperWindow *window)
{
	g_signal_handlers_block_by_func (G_OBJECT (window->priv->spin1), G_CALLBACK (set_image_position_cb), window);
	g_signal_handlers_block_by_func (G_OBJECT (window->priv->spin2), G_CALLBACK (set_image_position_cb), window);
	g_signal_handlers_block_by_func (G_OBJECT (window->priv->spin5), G_CALLBACK (set_image_scale_cb), window);
	g_signal_handlers_block_by_func (G_OBJECT (window->priv->spin6), G_CALLBACK (set_image_scale_cb), window);
	g_signal_handlers_block_by_func (G_OBJECT (window->priv->combo), G_CALLBACK (set_image_style_cb), window);
}

void unblock_callback(AnypaperWindow *window)
{
	g_signal_handlers_unblock_by_func (G_OBJECT (window->priv->spin1), G_CALLBACK (set_image_position_cb), window);
	g_signal_handlers_unblock_by_func (G_OBJECT (window->priv->spin2), G_CALLBACK (set_image_position_cb), window);
	g_signal_handlers_unblock_by_func (G_OBJECT (window->priv->spin5), G_CALLBACK (set_image_scale_cb), window);
	g_signal_handlers_unblock_by_func (G_OBJECT (window->priv->spin6), G_CALLBACK (set_image_scale_cb), window);
	g_signal_handlers_unblock_by_func (G_OBJECT (window->priv->combo), G_CALLBACK (set_image_style_cb), window);
}

static void show_window_preview_cb( GtkWidget *widget, AnypaperWindow *window )
{
	anypaper_preview_create(window->preview, window->parameters, window->image);
}

static void set_image_common( AnypaperWindow *window )
{
	GtkWidget *dialog_error;

	block_callback(window);
	if (!anypaper_image_make (window->image, window->parameters))
	{
		dialog_error = gtk_message_dialog_new (GTK_WINDOW (window->priv->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Could not open image file");
		gtk_window_set_title (GTK_WINDOW (dialog_error), "Error");
	 	g_signal_connect_swapped (dialog_error, "response", G_CALLBACK (gtk_widget_destroy), dialog_error);
		gtk_widget_show(dialog_error);
	}
	anypaper_window_set_position_range(window, window->parameters->positionx, window->parameters->positiony);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON (window->priv->spin1), (gdouble) window->parameters->positionx);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON (window->priv->spin2), (gdouble) window->parameters->positiony);
	anypaper_image_move (window->image, window->parameters);
	gtk_image_set_from_pixbuf(GTK_IMAGE(window->priv->preview), window->image->preview);
	unblock_callback(window);
}

static void update_preview_cb (GtkFileChooser *file_chooser, gpointer data)
{
	GtkWidget *preview;
	char *filename;
	GdkPixbuf *pixbuf;
	gboolean have_preview;

	preview = GTK_WIDGET (data);
	filename = gtk_file_chooser_get_preview_filename (file_chooser);

	pixbuf = gdk_pixbuf_new_from_file_at_size (filename, 128, 128, NULL);
	have_preview = (pixbuf != NULL);
	g_free (filename);

	gtk_image_set_from_pixbuf (GTK_IMAGE (preview), pixbuf);
	if (pixbuf) g_object_unref (pixbuf);

	gtk_file_chooser_set_preview_widget_active (file_chooser, have_preview);
}

void open_image_file_cb ( GtkWidget *widget, AnypaperWindow *window )
{
	GtkWidget *dialog;
	GtkWidget *preview;
	GtkFileFilter *filterImages, *filterAll;

	preview = gtk_image_new ();

	dialog = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW(window->priv->window), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

	filterImages = gtk_file_filter_new ();
	gtk_file_filter_set_name (GTK_FILE_FILTER(filterImages),"JPEG images");
	gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filterImages), "image/jpeg/*");
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filterImages);

	filterAll = gtk_file_filter_new ();
	gtk_file_filter_set_name (GTK_FILE_FILTER(filterAll),"All files");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filterAll), "*");
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filterAll);

	gtk_file_chooser_set_preview_widget (GTK_FILE_CHOOSER (dialog), preview);
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER (dialog), gtk_entry_get_text (GTK_ENTRY (window->priv->file_entry)));
	g_signal_connect (dialog, "update-preview", G_CALLBACK (update_preview_cb), preview);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) 
	{
		g_free(window->parameters->file);
		window->parameters->file = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		gtk_entry_set_text (GTK_ENTRY (window->priv->file_entry), window->parameters->file);
		set_image_common(window);
	}

	gtk_widget_destroy (dialog);
}

static void set_image_file_cb ( GtkWidget *widget, AnypaperWindow *window )
{
	gchar *file, *down_filename;
	GtkWidget *dialog_error;

	file = g_strdup(gtk_entry_get_text(GTK_ENTRY(window->priv->file_entry)));
	down_filename = g_ascii_strdown (file, -1);

	if (g_file_test (file, G_FILE_TEST_EXISTS))
	{
		g_free (window->parameters->file);
		window->parameters->file = file;
		set_image_common (window);
	}
	else
	{
		dialog_error = gtk_message_dialog_new (GTK_WINDOW (window->priv->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Such file does not exists");
		gtk_window_set_title (GTK_WINDOW (dialog_error), "Error");
	 	g_signal_connect_swapped (dialog_error, "response", G_CALLBACK (gtk_widget_destroy), dialog_error);
		gtk_widget_show(dialog_error);
	}
	g_free(down_filename);
}

static void set_image_style_cb ( GtkWidget *widget, AnypaperWindow *window )
{
	window->parameters->style = gtk_combo_box_get_active (GTK_COMBO_BOX (window->priv->combo));
	if (window->parameters->style == 5) gtk_widget_show (window->priv->custom_table);
	else gtk_widget_hide (window->priv->custom_table);
	set_image_common (window);
}

static void change_other_scale_cb ( GtkWidget *widget, AnypaperWindow *window )
{
	gdouble value;
	g_signal_handlers_block_by_func (G_OBJECT (window->priv->spin5), G_CALLBACK (change_other_scale_cb), window);
	g_signal_handlers_block_by_func (G_OBJECT (window->priv->spin6), G_CALLBACK (change_other_scale_cb), window);
	block_callback(window);
	value = gtk_spin_button_get_value (GTK_SPIN_BUTTON (widget));
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (window->priv->spin5), value);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (window->priv->spin6), value);
	window->parameters->scalex = window->parameters->scaley = value;
	g_signal_handlers_unblock_by_func (G_OBJECT (window->priv->spin5), G_CALLBACK (change_other_scale_cb), window);
	g_signal_handlers_unblock_by_func (G_OBJECT (window->priv->spin6), G_CALLBACK (change_other_scale_cb), window);
	unblock_callback(window);
	set_image_common (window);
}

static void set_image_scale_cb ( GtkWidget *widget, AnypaperWindow *window )
{
	window->parameters->scalex = gtk_spin_button_get_value (GTK_SPIN_BUTTON (window->priv->spin5));
	window->parameters->scaley = gtk_spin_button_get_value (GTK_SPIN_BUTTON (window->priv->spin6));
	set_image_common (window);
}

static void connect_custom_scale_cb ( GtkWidget *widget, AnypaperWindow *window )
{
	if (window->priv->custom_scale_connect_state == TRUE)
	{
		g_signal_handlers_block_by_func (G_OBJECT (window->priv->spin5), G_CALLBACK (change_other_scale_cb), window);
		g_signal_handlers_block_by_func (G_OBJECT (window->priv->spin6), G_CALLBACK (change_other_scale_cb), window);
		window->priv->custom_scale_connect_state = FALSE;
	}
	else
	{
		g_signal_handlers_unblock_by_func (G_OBJECT (window->priv->spin5), G_CALLBACK (change_other_scale_cb), window);
		g_signal_handlers_unblock_by_func (G_OBJECT (window->priv->spin6), G_CALLBACK (change_other_scale_cb), window);
		window->priv->custom_scale_connect_state = TRUE;
	}
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(widget), window->priv->custom_scale_connect_state);
}

static void set_image_background_cb ( GtkWidget *widget, AnypaperWindow *window )
{
	gchar *background_buffer;
	GdkColor color;
	gtk_color_button_get_color (GTK_COLOR_BUTTON (window->priv->cbutton), &color);
	background_buffer = gdk_color_to_string (&color);
	g_free(window->parameters->background);
	window->parameters->background = g_strdup_printf("#%c%c%c%c%c%c", background_buffer[1], background_buffer[2], background_buffer[5], background_buffer[6], background_buffer[9], background_buffer[10]);
	/*g_print("%s", &(window->parameters->background)[1]);*/
	set_image_common (window);
}

static void set_image_position_cb ( GtkWidget *widget, AnypaperWindow *window )
{
	block_callback(window);
	window->parameters->positionx = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (window->priv->spin1));
	window->parameters->positiony = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (window->priv->spin2));
	anypaper_image_move (window->image, window->parameters);
	gtk_image_set_from_pixbuf(GTK_IMAGE(window->priv->preview), window->image->preview);
	unblock_callback(window);
}

static void set_image_size_cb ( GtkWidget *widget, AnypaperWindow *window )
{
	window->parameters->width = gtk_spin_button_get_value (GTK_SPIN_BUTTON (window->priv->spin3));
	window->parameters->height = gtk_spin_button_get_value (GTK_SPIN_BUTTON (window->priv->spin4));
	set_image_common (window);
}

static void center_image_cb ( GtkWidget *widget, AnypaperWindow *window )
{
	set_image_common(window);
}

static void set_image_interpolation_cb ( GtkWidget *widget, AnypaperWindow *window )
{
	int i, positionx, positiony;
	block_callback(window);
	i = gtk_combo_box_get_active (GTK_COMBO_BOX (window->priv->combo_interpolation));
	if (i == 0) window->parameters->interpolation=GDK_INTERP_NEAREST;
	if (i == 1) window->parameters->interpolation=GDK_INTERP_TILES;
	if (i == 2) window->parameters->interpolation=GDK_INTERP_BILINEAR;
	if (i == 3) window->parameters->interpolation=GDK_INTERP_HYPER;	
	positionx = window->parameters->positionx;
	positiony = window->parameters->positiony;
	anypaper_image_make (window->image, window->parameters);
	window->parameters->positionx = positionx;
	window->parameters->positiony = positiony;
	anypaper_image_move (window->image, window->parameters);
	gtk_image_set_from_pixbuf(GTK_IMAGE(window->priv->preview), window->image->preview);
	unblock_callback(window);
}

gboolean test_command_exists(gchar *command)
{
	gchar **field;
	field = g_strsplit (command, " ", -1);
	if (g_find_program_in_path (field[0]) == NULL)
	{
		if (!g_file_test (field[0], G_FILE_TEST_EXISTS)) return FALSE;
		else TRUE;
	}
	else TRUE;
}

/*gboolean set_wallpaper_common ( AnypaperWindow *window )
{
	GtkWidget *dialog;
	char *buffer, *down_filename;
	gboolean result = TRUE;

	if (rcfile == NULL) rcfile = g_strdup_printf("%s/.anypaper/anypaperrc", g_get_home_dir ());
	if (lastwallpaperfile == NULL) lastwallpaperfile = g_strdup_printf("%s/.anypaper/lastwallpaper", g_get_home_dir ());
	window->parameters->defaultfile = gtk_entry_get_text (GTK_ENTRY (window->priv->def_entry));
	window->parameters->command = gtk_entry_get_text (GTK_ENTRY (window->priv->com_entry));
	down_filename = g_ascii_strdown (window->parameters->defaultfile, -1);

	if (!g_file_test (window->parameters->file, G_FILE_TEST_EXISTS))
	{
		dialog = gtk_message_dialog_new (GTK_WINDOW (window->priv->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "No such file exists");
		gtk_window_set_title (GTK_WINDOW (dialog), "Error");
		g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);
		gtk_widget_show(dialog);
		result = FALSE;
	}
	else
	{
		if ((g_str_has_suffix (down_filename, ".jpg")) || (g_str_has_suffix (down_filename, ".jpeg"))) 
		{
			gdk_pixbuf_save (window->image->image, window->parameters->defaultfile, "jpeg", NULL, "quality", "100", NULL);
			buffer=g_strdup_printf("%s \"%s\"", window->parameters->command, window->parameters->defaultfile);
			if (system (buffer))
			{
				dialog = gtk_message_dialog_new (GTK_WINDOW (window->priv->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Invalid command");
				gtk_window_set_title (GTK_WINDOW (dialog), "Error");
			 	g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);
				gtk_widget_show(dialog);
			result = FALSE;
			}
		}
		else if(g_str_has_suffix (down_filename, ".png"))
		{
			gdk_pixbuf_save (window->image->image, window->parameters->defaultfile, "png", NULL, NULL);
			buffer=g_strdup_printf("%s \"%s\"", window->parameters->command, window->parameters->defaultfile);
			if (system (buffer))
			{
				dialog = gtk_message_dialog_new (GTK_WINDOW (window->priv->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Invalid command");
				gtk_window_set_title (GTK_WINDOW (dialog), "Error");
			 	g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);
				gtk_widget_show(dialog);
			result = FALSE;
			}
		}
		else
		{
			dialog = gtk_message_dialog_new (GTK_WINDOW (window->priv->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Invalid format");
			gtk_window_set_title (GTK_WINDOW (dialog), "Error");
		 	g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);
			gtk_widget_show(dialog);
			result = FALSE;
		}
		g_free (down_filename);
		if (result == TRUE) anypaper_parameters_write ( window->parameters, lastwallpaperfile, rcfile);
	}
	return result;
}*/

gboolean set_wallpaper_common ( AnypaperWindow *window )
{
	GtkWidget *dialog;
	char *buffer, *down_filename;
	gboolean result = TRUE;

	if (rcfile == NULL) rcfile = g_strdup_printf("%s/.anypaper/anypaperrc", g_get_home_dir ());
	if (lastwallpaperfile == NULL) lastwallpaperfile = g_strdup_printf("%s/.anypaper/lastwallpaper", g_get_home_dir ());
	g_free(window->parameters->defaultfile);
	g_free(window->parameters->command);
	window->parameters->defaultfile = g_strdup(gtk_entry_get_text (GTK_ENTRY (window->priv->def_entry)));
	window->parameters->command = g_strdup(gtk_entry_get_text (GTK_ENTRY (window->priv->com_entry)));
	down_filename = g_ascii_strdown (window->parameters->defaultfile, -1);

	if (!g_file_test (window->parameters->file, G_FILE_TEST_EXISTS))
	{
		dialog = gtk_message_dialog_new (GTK_WINDOW (window->priv->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "No such file exists");
		gtk_window_set_title (GTK_WINDOW (dialog), "Error");
		g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);
		gtk_widget_show(dialog);
		result = FALSE;
	}
	else
	{
		if ((g_str_has_suffix (down_filename, ".jpg")) || (g_str_has_suffix (down_filename, ".jpeg"))) 
		{
			if (test_command_exists(window->parameters->command) == FALSE)
			{
				dialog = gtk_message_dialog_new (GTK_WINDOW (window->priv->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Command not found");
				gtk_window_set_title (GTK_WINDOW (dialog), "Error");
			 	g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);
				gtk_widget_show(dialog);
			result = FALSE;
			}
			else
			{
				gdk_pixbuf_save (window->image->image, window->parameters->defaultfile, "jpeg", NULL, "quality", "100", NULL);
				buffer=g_strdup_printf("%s \"%s\"", window->parameters->command, window->parameters->defaultfile);
				g_spawn_command_line_async (buffer, NULL);
			}
		}
		else if(g_str_has_suffix (down_filename, ".png"))
		{
			if (test_command_exists(window->parameters->command) == FALSE)
			{
				dialog = gtk_message_dialog_new (GTK_WINDOW (window->priv->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Command not found");
				gtk_window_set_title (GTK_WINDOW (dialog), "Error");
			 	g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);
				gtk_widget_show(dialog);
			result = FALSE;
			}
			else
			{
				gdk_pixbuf_save (window->image->image, window->parameters->defaultfile, "png", NULL, NULL);
				buffer=g_strdup_printf("%s \"%s\"", window->parameters->command, window->parameters->defaultfile);
				g_spawn_command_line_async (buffer, NULL);
			}
		}
		else
		{
			dialog = gtk_message_dialog_new (GTK_WINDOW (window->priv->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Invalid format");
			gtk_window_set_title (GTK_WINDOW (dialog), "Error");
		 	g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);
			gtk_widget_show(dialog);
			result = FALSE;
		}
		g_free (down_filename);
		if (result == TRUE) anypaper_parameters_write ( window->parameters, lastwallpaperfile, rcfile);
	}
	return result;
}

void save_default_file_cb( GtkWidget *widget, AnypaperWindow *window )
{
	GtkWidget *dialog, *dialog_error;
	GtkFileFilter *filterImages, *filterAll;

	dialog = gtk_file_chooser_dialog_new ("Default file",
					      GTK_WINDOW(window->priv->window),
					      GTK_FILE_CHOOSER_ACTION_SAVE,
					      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
					      NULL);

	filterImages = gtk_file_filter_new ();
	gtk_file_filter_set_name (GTK_FILE_FILTER(filterImages),"JPEG images");
	gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filterImages), "image/jpeg/*");
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filterImages);

	filterAll = gtk_file_filter_new ();
	gtk_file_filter_set_name (GTK_FILE_FILTER(filterAll),"All files");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filterAll), "*");
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filterAll);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), FALSE);

	gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), gtk_entry_get_text (GTK_ENTRY (window->priv->def_entry)));

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename, *down_filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		down_filename = g_ascii_strdown (filename, -1);

		if ((g_str_has_suffix (down_filename, ".jpg")) || (g_str_has_suffix (down_filename, ".jpeg")) || (g_str_has_suffix (down_filename, ".png")))
		{
			g_free(window->parameters->defaultfile);
			gtk_entry_set_text (GTK_ENTRY (window->priv->def_entry),filename);
			window->parameters->defaultfile = filename;
		}
		else
		{
			dialog_error = gtk_message_dialog_new (GTK_WINDOW (window->priv->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Invalid format");
			gtk_window_set_title (GTK_WINDOW (dialog_error), "Error");
		 	g_signal_connect_swapped (dialog_error, "response", G_CALLBACK (gtk_widget_destroy), dialog_error);
			gtk_widget_show(dialog_error);
		}
		g_free (filename);
		g_free (down_filename);
	}

	gtk_widget_destroy (dialog);
}

void save_file_as_cb( GtkWidget *widget, AnypaperWindow *window )
{
	GtkWidget *dialog, *dialog_error;
	GtkFileFilter *filterImages, *filterAll;


	dialog = gtk_file_chooser_dialog_new ("Save as...",
					      GTK_WINDOW(window->priv->window),
					      GTK_FILE_CHOOSER_ACTION_SAVE,
					      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
					      NULL);

	filterImages = gtk_file_filter_new ();
	gtk_file_filter_set_name (GTK_FILE_FILTER(filterImages),"JPEG images");
	gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filterImages), "image/jpeg/*");
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filterImages);

	filterAll = gtk_file_filter_new ();
	gtk_file_filter_set_name (GTK_FILE_FILTER(filterAll),"All files");
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(filterAll), "*");
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filterAll);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), FALSE);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);

	gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), g_path_get_dirname (gtk_entry_get_text (GTK_ENTRY (window->priv->file_entry))));
	gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), "Untitled image.png");

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename, *down_filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		down_filename = g_ascii_strdown (filename, -1);

		if ((g_str_has_suffix (down_filename, ".jpg")) || (g_str_has_suffix (down_filename, ".jpeg"))) gdk_pixbuf_save (window->image->image, filename, "jpeg", NULL, "quality", "100", NULL);
		else if(g_str_has_suffix (down_filename, ".png")) gdk_pixbuf_save (window->image->image, filename, "png", NULL, NULL);
		else
		{
			dialog_error = gtk_message_dialog_new (GTK_WINDOW (window->priv->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Invalid format");
			gtk_window_set_title (GTK_WINDOW (dialog_error), "Error");
		 	g_signal_connect_swapped (dialog_error, "response", G_CALLBACK (gtk_widget_destroy), dialog_error);
			gtk_widget_show(dialog_error);
		}
		g_free (filename);
		g_free (down_filename);
	}

	gtk_widget_destroy (dialog);
}

void load_lastwallpaperfile_cb ( GtkWidget *widget, AnypaperWindow *window )
{
	GtkWidget *dialog;
	GdkColor color;
	gint positionx, positiony, rangex, rangey;

	dialog = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW(window->priv->window), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER (dialog), lastwallpaperfile);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) 
	{
		block_callback(window);
		g_free(lastwallpaperfile);
		lastwallpaperfile = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		anypaper_parameters_load(window->parameters, lastwallpaperfile);
		positionx = window->parameters->positionx;
		positiony = window->parameters->positiony;
		anypaper_image_make (window->image, window->parameters);
		rangex = window->parameters->positionx;
		rangey = window->parameters->positiony;
		anypaper_parameters_position_test(window->parameters, rangex, rangey, positionx, positiony);
		anypaper_image_move (window->image, window->parameters);
		gtk_entry_set_text (GTK_ENTRY (window->priv->file_entry), window->parameters->file);
		anypaper_window_set_position_range(window, rangex, rangey);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON (window->priv->spin1), window->parameters->positionx);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON (window->priv->spin2), window->parameters->positiony);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON (window->priv->spin3), window->parameters->width);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON (window->priv->spin4), window->parameters->height);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON (window->priv->spin5), window->parameters->scalex);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON (window->priv->spin6), window->parameters->scaley);
		gtk_combo_box_set_active (GTK_COMBO_BOX (window->priv->combo), window->parameters->style);
		gdk_color_parse (window->parameters->background, &color);
		gtk_color_button_set_color (GTK_COLOR_BUTTON(window->priv->cbutton), &color);
		gtk_image_set_from_pixbuf(GTK_IMAGE(window->priv->preview), window->image->preview);
		unblock_callback(window);
	}
	gtk_widget_destroy (dialog);
}

void load_rcfile_cb ( GtkWidget *widget, AnypaperWindow *window )
{
	GtkWidget *dialog;
	gint positionx, positiony, rangex, rangey;

	dialog = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW(window->priv->window), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER (dialog), rcfile);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) 
	{
		block_callback(window);
		g_free(rcfile);
		rcfile = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		anypaper_parameters_load(window->parameters, rcfile);
		positionx = window->parameters->positionx;
		positiony = window->parameters->positiony;
		anypaper_image_make (window->image, window->parameters);
		rangex = window->parameters->positionx;
		rangey = window->parameters->positiony;
		anypaper_parameters_position_test(window->parameters, rangex, rangey, positionx, positiony);
		anypaper_image_move (window->image, window->parameters);
		gtk_entry_set_text (GTK_ENTRY (window->priv->def_entry), window->parameters->defaultfile);
		gtk_entry_set_text (GTK_ENTRY (window->priv->com_entry), window->parameters->command);
		gtk_combo_box_set_active (GTK_COMBO_BOX (window->priv->combo_interpolation), interpolation_int (window->parameters->interpolation));
		gtk_image_set_from_pixbuf(GTK_IMAGE(window->priv->preview), window->image->preview);
		unblock_callback(window);
		}
	gtk_widget_destroy (dialog);
}

void apply_cb( GtkWidget *widget, AnypaperWindow *window )
{
	set_wallpaper_common (window);
}

void ok_cb( GtkWidget *widget, AnypaperWindow *window )
{
	if (set_wallpaper_common (window) == TRUE) gtk_main_quit();
}

void about_window_cb( GtkWidget *widget, gpointer data )
{
	GtkWidget *window;

	window = GTK_WIDGET (data);

	static const gchar * const authors[] = {
		"Sergio Rodrigues de Oliveira Filho <cdvdt@users.sourceforge.net>",
		NULL
	};

	static const gchar * const documenters[] = {
		"Ninguém",
		NULL
	};

	static const gchar copyright[] = \
		"Copyright \xc2\xa9 2007-2009 Sergio Rodrigues de Oliveira Filho";

	static const gchar comments[] = \
		"anyPaper adjusts the image that will be used as wallpaper and set it";

	static const gchar license[] = LICENSE;

	GdkPixbuf *logo;

	logo = gdk_pixbuf_new_from_file (ANYPAPER_DATA "anypaper.png",
					 NULL);

	gtk_show_about_dialog (GTK_WINDOW (window),
				"authors", authors,
				"comments", comments,
				"copyright", copyright,
				"logo", logo,
				"version", VERSION,
				"license", license,
				"website", "anypaper.sourceforge.net",
				"name", "nPaper",
				"wrap-license",TRUE,
				NULL);

	if (logo)
		g_object_unref (logo);
}

void detect_popup_cb( GtkWidget *widget, AnypaperWindow *window )
{
	if (!g_file_test (wallpapersetterfile, G_FILE_TEST_EXISTS)) anypaper_wallpapersetter_file(wallpapersetterfile);
	anypaper_wallpapersetter_detect (window->wallpapersetter, wallpapersetterfile);

	if (window->wallpapersetter->wallpapersetter)
	{
		GtkWidget *dialog, *label, *content_area, *box, *halign, *combo;
		GList *atual=NULL, *atual_com=NULL;

		dialog = gtk_dialog_new_with_buttons ("Detected wallpapersetters", GTK_WINDOW(window->priv->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
		content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

		box = gtk_vbox_new (FALSE, 0);
		gtk_container_add (GTK_CONTAINER (content_area), box);

		label = gtk_label_new ("Are listed below the wallpapersetter found,\nplease select one:");
		gtk_box_pack_start (GTK_BOX(box), label, FALSE, TRUE, 2);

		halign = gtk_alignment_new(0, 1, 0, 0);
		combo = gtk_combo_box_new_text ();
		gtk_container_add(GTK_CONTAINER(halign), combo);

		atual = g_list_first (window->wallpapersetter->wallpapersetter);
		atual_com = g_list_first (window->wallpapersetter->command);

		while (atual)
		{
			gtk_combo_box_append_text (GTK_COMBO_BOX (combo), atual->data);
			atual = g_list_next(atual);
		}

		gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
		gtk_box_pack_start (GTK_BOX(box), halign, FALSE, TRUE, 2);
		gtk_widget_show_all (dialog);

		if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
		{
			gchar *command;
			atual = g_list_nth (window->wallpapersetter->wallpapersetter, gtk_combo_box_get_active (GTK_COMBO_BOX (combo)));
			atual_com = g_list_nth (window->wallpapersetter->command, gtk_combo_box_get_active (GTK_COMBO_BOX (combo)));
			command = g_strdup_printf("%s %s", (gchar *) atual->data, (gchar *) atual_com->data);
			gtk_entry_set_text (GTK_ENTRY (window->priv->com_entry), command);
			g_free(command);
		}

		gtk_widget_destroy (dialog);
	}
	else
	{
		GtkWidget *dialog;
		dialog = gtk_message_dialog_new (GTK_WINDOW(window->priv->window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_CLOSE, "No wallpapersetter found. A list of known wallpapersetter can be found in %s. You can install a wallpapersetter from that list, manually add a command to it or use directly a command of your preference", wallpapersetterfile);
		gtk_window_set_title (GTK_WINDOW(dialog), "Warning");
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
	}
}
/**
 * anypaper_window_set_position_range:
 * @window: the #AnypaperWindow
 * @rangex: half of the displacement allowed in x direction
 * @rangey: half of the displacement allowed in y direction
 * 
 * Sets the range of displacement allowed for the image.
 */

void anypaper_window_set_position_range(AnypaperWindow *window, gint rangex, gint rangey)
{
	if (rangex < 0)	gtk_spin_button_set_range (GTK_SPIN_BUTTON (window->priv->spin1), (gdouble) 2 * rangex, 0);
	if (rangex >= 0) gtk_spin_button_set_range (GTK_SPIN_BUTTON (window->priv->spin1), 0, 2 * rangex);
	if (rangey < 0)	gtk_spin_button_set_range (GTK_SPIN_BUTTON (window->priv->spin2), (gdouble) 2 * rangey, 0);
	if (rangey >= 0) gtk_spin_button_set_range (GTK_SPIN_BUTTON (window->priv->spin2), 0, 2 * rangey);
	if (rangex == 0) 
	{
		gtk_widget_set_sensitive (window->priv->spin1, FALSE);
		gtk_widget_set_sensitive (window->priv->label1, FALSE);
	}
	else
	{
		gtk_widget_set_sensitive (window->priv->spin1, TRUE);
		gtk_widget_set_sensitive (window->priv->label1, TRUE);
	}
	if (rangey == 0) 
	{
		gtk_widget_set_sensitive (window->priv->spin2, FALSE);
		gtk_widget_set_sensitive (window->priv->label2, FALSE);
	}
	else
	{
		gtk_widget_set_sensitive (window->priv->spin2, TRUE);
		gtk_widget_set_sensitive (window->priv->label2, TRUE);
	}
}

gint interpolation_int (GdkInterpType interpolation)
{
	gint i;
	if (interpolation == GDK_INTERP_NEAREST) i = 0;
	if (interpolation == GDK_INTERP_TILES) i = 1;
	if (interpolation == GDK_INTERP_BILINEAR) i = 2;
	if (interpolation == GDK_INTERP_HYPER) i = 3;
	return i;
}

/**
 * anypaper_window_create:
 * @window: the #AnypaperWindow
 * 
 * Creates and shows the #AnypaperWindow.
 */

void anypaper_window_create (AnypaperWindow *self)
{
	AnypaperWindowPrivate *priv = self->priv;

	GtkWidget *window;
	GtkWidget *label;
	GtkWidget *image, *background;
	GtkWidget *rmenu;
	GtkWidget *button;
	GtkWidget *note, *vbox1, *vbox2, *hbox1, *table, *halign;
	gchar *filename, *buffer;
	GdkPixbuf *tempbuf;
	GdkColor color;

	self->priv->window = window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title (GTK_WINDOW (window), "anyPaper " VERSION);
	gtk_window_set_default_size(GTK_WINDOW(window), 500, 100);

	gtk_window_set_icon_from_file (GTK_WINDOW(window), ANYPAPER_DATA "anypaper.png", NULL);

	g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (gtk_main_quit), NULL);

	gtk_container_set_border_width (GTK_CONTAINER (window), 10);
	
	vbox2 = gtk_vbox_new (FALSE, 0);

	gtk_container_add (GTK_CONTAINER (window), vbox2);

	note = gtk_notebook_new ();

	vbox1 = gtk_vbox_new (FALSE, 10);

	label = gtk_label_new ("Wallpaper");
	gtk_notebook_append_page (GTK_NOTEBOOK(note), vbox1, label);
	gtk_widget_show (label);
	
	table = gtk_table_new (3, 4, FALSE);

	label = gtk_label_new ("File: ");
	gtk_misc_set_alignment (GTK_MISC(label), 0.0, 0.5);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (label);

	priv->file_entry = gtk_entry_new();
	gtk_entry_set_text (GTK_ENTRY (priv->file_entry), self->parameters->file);
	g_signal_connect (G_OBJECT (priv->file_entry), "activate", G_CALLBACK (set_image_file_cb), self);
	gtk_table_attach (GTK_TABLE (table), priv->file_entry, 1, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (priv->file_entry);

	button = gtk_button_new_from_stock (GTK_STOCK_OPEN);
	image = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image (GTK_BUTTON(button), image);
	gtk_widget_show (image);
	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (open_image_file_cb), self);
	gtk_table_attach (GTK_TABLE (table), button, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (button);

	label = gtk_label_new ("Style: ");
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
	gtk_misc_set_alignment (GTK_MISC(label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (label);

	halign = gtk_alignment_new(0, 1, 0, 0);
	priv->combo = gtk_combo_box_new_text ();
	gtk_container_add(GTK_CONTAINER(halign), priv->combo);
	gtk_combo_box_append_text (GTK_COMBO_BOX (priv->combo), "Fullscreen");
	gtk_combo_box_append_text (GTK_COMBO_BOX (priv->combo), "Normal");
	gtk_combo_box_append_text (GTK_COMBO_BOX (priv->combo), "Tiled");
	gtk_combo_box_append_text (GTK_COMBO_BOX (priv->combo), "Adjusted");
	gtk_combo_box_append_text (GTK_COMBO_BOX (priv->combo), "Scaled");
	gtk_combo_box_append_text (GTK_COMBO_BOX (priv->combo), "Custom Scale");
	gtk_combo_box_set_active (GTK_COMBO_BOX (priv->combo), self->parameters->style);
	g_signal_connect (G_OBJECT (priv->combo), "changed", G_CALLBACK (set_image_style_cb), self);
	gtk_table_attach (GTK_TABLE (table), halign, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (priv->combo);
	gtk_widget_show (halign);

	priv->custom_table = gtk_table_new (2, 3, FALSE);

	label = gtk_label_new ("Scale x: ");
	gtk_misc_set_alignment (GTK_MISC(label), 0.0, 0.5);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (priv->custom_table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (label);

	priv->spin5 = gtk_spin_button_new_with_range (0, 100, .1);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON (priv->spin5), 4);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON (priv->spin5), self->parameters->scalex);
	g_signal_connect (G_OBJECT (priv->spin5), "value-changed", G_CALLBACK (change_other_scale_cb), self);
	g_signal_connect_after (G_OBJECT (priv->spin5), "value-changed", G_CALLBACK (set_image_scale_cb), self);
	gtk_table_attach (GTK_TABLE (priv->custom_table), priv->spin5, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (priv->spin5);

	button = gtk_toggle_button_new_with_mnemonic("linked");
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(button), priv->custom_scale_connect_state);
	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (connect_custom_scale_cb), self);
	gtk_table_attach (GTK_TABLE (priv->custom_table), button, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (button);

	label = gtk_label_new ("Scale y: ");
	gtk_misc_set_alignment (GTK_MISC(label), 0.0, 0.5);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (priv->custom_table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (label);

	priv->spin6 = gtk_spin_button_new_with_range (0, 100, .1);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON (priv->spin6), 4);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON (priv->spin6), self->parameters->scaley);
	g_signal_connect (G_OBJECT (priv->spin6), "value-changed", G_CALLBACK (change_other_scale_cb), self);
	g_signal_connect_after (G_OBJECT (priv->spin6), "value-changed", G_CALLBACK (set_image_scale_cb), self);
	gtk_table_attach (GTK_TABLE (priv->custom_table), priv->spin6, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (priv->spin6);

	gtk_table_attach (GTK_TABLE (table), priv->custom_table, 2, 4, 1, 3, GTK_FILL, GTK_FILL, 0, 0);
	if (self->parameters->style == 5) gtk_widget_show (priv->custom_table);
	else gtk_widget_hide (priv->custom_table);

	label = gtk_label_new ("Background: ");
	gtk_misc_set_alignment (GTK_MISC(label), 0.0, 0.5);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (label);

	gdk_color_parse (self->parameters->background, &color);
	halign = gtk_alignment_new(0, 1, 0, 0);
	priv->cbutton = gtk_color_button_new_with_color (&color);
	gtk_container_add(GTK_CONTAINER(halign), priv->cbutton);
	g_signal_connect (G_OBJECT (priv->cbutton), "color-set", G_CALLBACK (set_image_background_cb), self);
	gtk_table_attach (GTK_TABLE (table), halign, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (priv->cbutton);

	gtk_widget_show (halign);

	gtk_box_pack_start (GTK_BOX(vbox1), table, FALSE, TRUE, 0);
	gtk_widget_show (table);

	hbox1 = gtk_hbox_new (FALSE, 0);

	priv->preview = gtk_image_new_from_pixbuf(self->image->preview);
	gtk_box_pack_start (GTK_BOX(hbox1), priv->preview, TRUE, FALSE, 2);
	gtk_widget_show (priv->preview);

	table = gtk_table_new (5, 2, FALSE);

	priv->label1 = gtk_label_new ("Position x: ");
	gtk_misc_set_alignment (GTK_MISC(priv->label1), 0.0, 0.5);
	gtk_label_set_justify (GTK_LABEL (priv->label1), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), priv->label1, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (priv->label1);

	priv->spin1 = gtk_spin_button_new_with_range (-65535, 65535, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON (priv->spin1), self->parameters->positionx);
	g_signal_connect (G_OBJECT (priv->spin1), "value-changed", G_CALLBACK (set_image_position_cb), self);
	gtk_table_attach (GTK_TABLE (table), priv->spin1, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (priv->spin1);

	priv->label2 = gtk_label_new ("Position y: ");
	gtk_misc_set_alignment (GTK_MISC(priv->label2), 0.0, 0.5);
	gtk_label_set_justify (GTK_LABEL (priv->label2), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), priv->label2, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (priv->label2);

	priv->spin2 = gtk_spin_button_new_with_range (-65535, 65535, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON (priv->spin2), self->parameters->positiony);
	g_signal_connect (G_OBJECT (priv->spin2), "value-changed", G_CALLBACK (set_image_position_cb), self);
	gtk_table_attach (GTK_TABLE (table), priv->spin2, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (priv->spin2);

	label = gtk_label_new ("Width: ");
	gtk_misc_set_alignment (GTK_MISC(label), 0.0, 0.5);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (label);

	priv->spin3 = gtk_spin_button_new_with_range (0, 65535, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON (priv->spin3), self->parameters->width);
	g_signal_connect (G_OBJECT (priv->spin3), "value-changed", G_CALLBACK (set_image_size_cb), self);
	gtk_table_attach (GTK_TABLE (table), priv->spin3, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (priv->spin3);

	label = gtk_label_new ("Height: ");
	gtk_misc_set_alignment (GTK_MISC(label), 0.0, 0.5);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (label);

	priv->spin4 = gtk_spin_button_new_with_range (0, 65535, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON (priv->spin4), self->parameters->height);
	g_signal_connect (G_OBJECT (priv->spin4), "value-changed", G_CALLBACK (set_image_size_cb), self);
	gtk_table_attach (GTK_TABLE (table), priv->spin4, 1, 2, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (priv->spin4);

	button = gtk_button_new_with_mnemonic ("_Center");
	g_signal_connect (G_OBJECT(button), "clicked", G_CALLBACK(center_image_cb), self);
	gtk_table_attach (GTK_TABLE (table), button, 0, 2, 4, 5, GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_widget_show (button);

	gtk_box_pack_start (GTK_BOX(hbox1), table, FALSE, TRUE, 0);
	gtk_widget_show (table);

	gtk_box_pack_start (GTK_BOX(vbox1), hbox1, FALSE, TRUE, 2);
	gtk_widget_show (hbox1);

	hbox1 = gtk_hbox_new (FALSE, 0);

	button = gtk_button_new_with_mnemonic ("Show _Preview");
	g_signal_connect (G_OBJECT(button), "clicked", G_CALLBACK(show_window_preview_cb), self);
	gtk_box_pack_start (GTK_BOX(hbox1), button, FALSE, FALSE, 2);
	gtk_widget_show (button);

	button = gtk_button_new_with_mnemonic ("Load lastwallpaper file");
	image = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image (GTK_BUTTON(button), image);
	gtk_widget_show (image);
	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (load_lastwallpaperfile_cb), self);
	gtk_box_pack_end (GTK_BOX(hbox1), button, FALSE, FALSE, 2);
	gtk_widget_show (button);

	gtk_box_pack_start (GTK_BOX(vbox1), hbox1, FALSE, TRUE, 2);
	gtk_widget_show (hbox1);

	gtk_widget_show (vbox1);

	vbox1 = gtk_vbox_new (FALSE, 2);

	label = gtk_label_new ("Configuration");
	gtk_notebook_append_page (GTK_NOTEBOOK(note), vbox1, label);
	gtk_widget_show (label);

	table = gtk_table_new (3, 3, FALSE);

	label = gtk_label_new ("Default output file: ");
	gtk_misc_set_alignment (GTK_MISC(label), 0.0, 0.5);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (label);

	priv->def_entry = gtk_entry_new();
	gtk_entry_set_text (GTK_ENTRY (priv->def_entry), self->parameters->defaultfile);
	gtk_table_attach_defaults (GTK_TABLE (table), priv->def_entry, 1, 2, 0, 1);
	gtk_widget_show (priv->def_entry);

	button = gtk_button_new_from_stock (GTK_STOCK_SAVE);
	image = gtk_image_new_from_stock (GTK_STOCK_SAVE, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image (GTK_BUTTON(button), image);
	gtk_widget_show (image);
	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (save_default_file_cb), self);
	gtk_table_attach (GTK_TABLE (table), button, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (button);

	label = gtk_label_new ("Command (Wallpapersetter): ");
	gtk_misc_set_alignment (GTK_MISC(label), 0.0, 0.5);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (label);
	
	button = gtk_button_new_with_mnemonic ("Detect");
	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (detect_popup_cb), self);
	gtk_table_attach (GTK_TABLE (table), button, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (button);

	priv->com_entry = gtk_entry_new();
	gtk_entry_set_text (GTK_ENTRY (priv->com_entry), self->parameters->command);
	gtk_table_attach_defaults (GTK_TABLE (table), priv->com_entry, 1, 2, 1, 2);
	gtk_widget_show (priv->com_entry);

	label = gtk_label_new ("Interpolation: ");
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
	gtk_misc_set_alignment (GTK_MISC(label), 0.0, 0.5);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (label);

	halign = gtk_alignment_new(0, 1, 0, 0);
	priv->combo_interpolation = gtk_combo_box_new_text ();
	gtk_container_add(GTK_CONTAINER(halign), priv->combo_interpolation);
	gtk_combo_box_append_text (GTK_COMBO_BOX (priv->combo_interpolation), "GDK_INTERP_NEAREST");
	gtk_combo_box_append_text (GTK_COMBO_BOX (priv->combo_interpolation), "GDK_INTERP_TILES");
	gtk_combo_box_append_text (GTK_COMBO_BOX (priv->combo_interpolation), "GDK_INTERP_BILINEAR");
	gtk_combo_box_append_text (GTK_COMBO_BOX (priv->combo_interpolation), "GDK_INTERP_HYPER");
	gtk_combo_box_set_active (GTK_COMBO_BOX (priv->combo_interpolation), interpolation_int (self->parameters->interpolation));
	g_signal_connect (G_OBJECT (priv->combo_interpolation), "changed", G_CALLBACK (set_image_interpolation_cb), self);
	gtk_table_attach (GTK_TABLE (table), halign, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_widget_show (priv->combo_interpolation);
	gtk_widget_show (halign);

	gtk_box_pack_start (GTK_BOX(vbox1), table, FALSE, TRUE, 0);
	gtk_widget_show (table);

	hbox1 = gtk_hbox_new (FALSE, 0);

	button = gtk_button_new_with_mnemonic ("Load anypaperrc file");
	image = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image (GTK_BUTTON(button), image);
	gtk_widget_show (image);
	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (load_rcfile_cb), self);
	gtk_box_pack_end (GTK_BOX(hbox1), button, FALSE, FALSE, 2);
	gtk_widget_show (button);

	gtk_box_pack_end (GTK_BOX(vbox1), hbox1, FALSE, TRUE, 2);
	gtk_widget_show (hbox1);

	gtk_widget_show (vbox1);

	gtk_box_pack_start (GTK_BOX(vbox2), note, FALSE, TRUE, 0);
	
	hbox1 = gtk_hbox_new (FALSE, 0);

	if (no_set == FALSE)
	{	
		button = gtk_button_new_from_stock (GTK_STOCK_OK);
		image = gtk_image_new_from_stock (GTK_STOCK_OK, GTK_ICON_SIZE_BUTTON);

	}
	else
	{
		button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
		image = gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_BUTTON);
	}
	gtk_button_set_image (GTK_BUTTON(button), image);
	gtk_widget_show (image);
	if (no_set == FALSE)
	{
		g_signal_connect (G_OBJECT(button), "clicked", G_CALLBACK(ok_cb), self);
	}
	else g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (delete_event), NULL);
	gtk_box_pack_end (GTK_BOX(hbox1), button, FALSE, FALSE, 2);
	gtk_widget_show (button);

	if (no_set == FALSE)
	{
		button = gtk_button_new_from_stock (GTK_STOCK_APPLY);
		image = gtk_image_new_from_stock (GTK_STOCK_APPLY, GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image (GTK_BUTTON(button), image);
		gtk_widget_show (image);
		g_signal_connect (G_OBJECT(button), "clicked", G_CALLBACK(apply_cb), self);
		gtk_box_pack_end (GTK_BOX(hbox1), button, FALSE, FALSE, 2);
		gtk_widget_show (button);

		button = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
		image = gtk_image_new_from_stock (GTK_STOCK_CANCEL, GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image (GTK_BUTTON(button), image);
		gtk_widget_show (image);
		g_signal_connect (G_OBJECT (button), "clicked",	G_CALLBACK (delete_event), NULL);
		gtk_box_pack_end (GTK_BOX(hbox1), button, FALSE, FALSE, 2);
		gtk_widget_show (button);
	}

	button = gtk_button_new_from_stock (GTK_STOCK_SAVE_AS);
	image = gtk_image_new_from_stock (GTK_STOCK_SAVE_AS, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image (GTK_BUTTON(button), image);
	gtk_widget_show (image);
	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (save_file_as_cb), self);
	gtk_box_pack_end (GTK_BOX(hbox1), button, FALSE, FALSE, 2);
	gtk_widget_show (button);

	button = gtk_button_new_from_stock (GTK_STOCK_ABOUT);
	image = gtk_image_new_from_stock (GTK_STOCK_ABOUT, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image (GTK_BUTTON(button), image);
	gtk_widget_show (image);
	g_signal_connect (G_OBJECT(button), "clicked", G_CALLBACK(about_window_cb), G_OBJECT (window));
	gtk_box_pack_start (GTK_BOX(hbox1), button, FALSE, FALSE, 2);
	gtk_widget_show (button);

	gtk_box_pack_start (GTK_BOX(vbox2), hbox1, FALSE, TRUE, 0);
	gtk_widget_show (hbox1);

	gtk_widget_show (vbox2);

	gtk_widget_show (note);

	gtk_widget_show (window);
}

