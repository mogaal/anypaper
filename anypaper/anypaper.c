/*
 * anypaper.c
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

#include "config.h"
#include "anypaper_command.h"
#include "anypaper_parameters.h"
#include "anypaper_image.h"
#include "anypaper_window.h"
#include "anypaper_preview.h"
#include "anypaper_wallpapersetter.h"

gboolean custom_scale_connect_state =TRUE;
gboolean fullscreen = FALSE, normal = FALSE, tile = FALSE, maximize = FALSE, scale = FALSE, custom = FALSE, set  = FALSE, last = FALSE, no_set = FALSE;
gboolean info = FALSE;
gchar *commandline = NULL;
int x_position = -65536, y_position = -65536, height = -65536, width = -65536;
double  x_scale = -1, y_scale = -1;
gchar *background_color = NULL, *lastwallpaperfile = NULL, *rcfile = NULL, **remaining_args = NULL, *wallpapersetterfile = NULL;

int main( int argc, char *argv[] )
{
	AnypaperParameters *parameters;
	AnypaperImage *image;
	AnypaperPreview *preview;
	AnypaperWindow *window;
	AnypaperWallpapersetter *wallpapersetter;
	gchar *buffer, *down_filename;
	GdkScreen *screen;
	GError *error = NULL;
	GOptionContext *context;
	gint rangex, rangey, positionx, positiony, screenw, screenh;

	context = g_option_context_new ("");
	g_option_context_add_main_entries (context, entries, NULL);
	g_option_context_add_group (context, gtk_get_option_group (TRUE));
	g_option_context_parse (context, &argc, &argv, &error);

	gtk_init(&argc, &argv);

	screen = gdk_screen_get_default();
	screenw = gdk_screen_get_width(screen);
	screenh = gdk_screen_get_height(screen);

	parameters = g_object_new (ANYPAPER_TYPE_PARAMETERS, NULL);
	parameters->width = screenw;
	parameters->height = screenh;

	image = g_object_new (ANYPAPER_TYPE_IMAGE, NULL);

	preview = g_object_new (ANYPAPER_TYPE_PREVIEW, NULL);

	wallpapersetter = g_object_new (ANYPAPER_TYPE_WALLPAPERSETTER, NULL);

	window = g_object_new (ANYPAPER_TYPE_WINDOW, NULL);
	window->parameters = parameters;
	window->image = image;
	window->preview = preview;
	window->wallpapersetter = wallpapersetter;

	buffer = g_strdup_printf("%s/.anypaper", g_get_home_dir ());
	if (!g_file_test (buffer, G_FILE_TEST_EXISTS)) g_mkdir(buffer, 0777);
	g_free(buffer);

	if (rcfile == NULL) rcfile = g_strdup_printf("%s/.anypaper/anypaperrc", g_get_home_dir ());
	if (lastwallpaperfile == NULL) lastwallpaperfile = g_strdup_printf("%s/.anypaper/lastwallpaper", g_get_home_dir ());
	if (g_file_test (rcfile, G_FILE_TEST_EXISTS)) anypaper_parameters_load(parameters, rcfile);

	if (wallpapersetterfile == NULL) wallpapersetterfile = g_strdup_printf("%s/.anypaper/wallpapersetters", g_get_home_dir ());
	if (!g_file_test (wallpapersetterfile, G_FILE_TEST_EXISTS))
	{
		anypaper_wallpapersetter_file(wallpapersetterfile);
		anypaper_wallpapersetter_detect (wallpapersetter, wallpapersetterfile);
		if (wallpapersetter->wallpapersetter) 
		{
			g_free(window->parameters->command);
			window->parameters->command = g_strdup_printf("%s %s", (gchar *) wallpapersetter->wallpapersetter->data, (gchar *) wallpapersetter->command->data);
		}
		else g_print("No wallpapersetter found. A list of known wallpapersetter can be found in %s. You can install a wallpapersetter from that list, manually add a command to it or use directly a command of your preference\n", wallpapersetterfile);
	}

	if (info == TRUE)
	{
		GList *current=NULL, *current_com=NULL;
		anypaper_wallpapersetter_detect (wallpapersetter, wallpapersetterfile);
		current = g_list_first (wallpapersetter->wallpapersetter);
		current_com = g_list_first (wallpapersetter->command);
		if (current)
			while (current)
			{
				g_print("found: %s, options: %s\n", (gchar *) current->data, (gchar *) current_com->data);
				current = g_list_next(current);
				current_com = g_list_next(current_com);
			}
		else g_print("none found");
		exit (EXIT_SUCCESS);
	}

	if (last == FALSE)
	{
		if (g_file_test (lastwallpaperfile, G_FILE_TEST_EXISTS))
		{
			anypaper_parameters_load(parameters, lastwallpaperfile);
			anypaper_command_load(parameters);
			positionx = parameters->positionx;
			positiony = parameters->positiony;
			anypaper_image_make (image, parameters);
			rangex = parameters->positionx;
			rangey = parameters->positiony;
			anypaper_parameters_position_test(parameters, rangex, rangey, positionx, positiony);
		}
		else
		{
			anypaper_image_make (image, parameters);
			rangex = parameters->positionx;
			rangey = parameters->positiony;
		}
		anypaper_image_move (image, parameters);
	}

	if (last == TRUE)
	{
		if (!g_file_test (parameters->defaultfile, G_FILE_TEST_EXISTS)) g_printerr("Default output file not found\n");
		else 
		{
			gchar *std_out, *std_err;
			/*GError *err = NULL;*/
			gint exitStatus;

			buffer = g_strdup_printf("%s %s", parameters->command, parameters->defaultfile);
			if (g_spawn_command_line_sync (buffer, &std_out, &std_err, &exitStatus, NULL) == FALSE) g_printerr("Invalid command\n");
			else
			{
				if (*std_out != NULL)
				{
					g_print ("Output:\n%s", std_out);
					g_free (std_out);
				}
				if (*std_err != NULL)
				{
					g_printerr ("Error:\n%s", std_err);
					g_free (std_err);
				}
			}

			/*if (system(buffer)) g_print("Invalid command\n");*/
		 	g_free (buffer);
		}

	}
	else if (set == TRUE)
	{
		gchar *std_out, *std_err;
		GError *err = NULL;
		gint exitStatus;

		down_filename = g_ascii_strdown (window->parameters->defaultfile, -1);
		if ((g_str_has_suffix (down_filename, ".jpg")) || (g_str_has_suffix (down_filename, ".jpeg"))) 
		{
			gdk_pixbuf_save (window->image->image, window->parameters->defaultfile, "jpeg", NULL, "quality", "100", NULL);
			buffer=g_strdup_printf("%s \"%s\"", window->parameters->command, window->parameters->defaultfile);
			if (g_spawn_command_line_sync (buffer, &std_out, &std_err, &exitStatus, &err) == FALSE)
			{
				g_print("%s\n", err->message);
				g_clear_error(&err);
			}
			/*if (system (buffer)) g_print("Invalid command\n");*/
			else
			{
				if (*std_out != NULL)
				{
					g_print ("Output:\n%s", std_out);
					g_free (std_out);
				}
				if (*std_err != NULL)
				{
					g_printerr ("Error:\n%s", std_err);
					g_free (std_err);
				}
				else anypaper_parameters_write ( window->parameters, lastwallpaperfile, rcfile);
			}
		 	g_free (buffer);
		}
		else if(g_str_has_suffix (down_filename, ".png"))
		{
			gdk_pixbuf_save (window->image->image, window->parameters->defaultfile, "png", NULL, NULL);
			buffer=g_strdup_printf("%s \"%s\"", window->parameters->command, window->parameters->defaultfile);
			if (g_spawn_command_line_sync (buffer, &std_out, &std_err, &exitStatus, &err) == FALSE)
			{
				g_print("%s\n", err->message);
				g_clear_error(&err);
			}
			/*if (system (buffer)) g_print("Invalid command\n");*/
			else
			{
				if (*std_out != NULL)
				{
					g_print ("Output:\n%s", std_out);
					g_free (std_out);
				}
				if (*std_err != NULL)
				{
					g_printerr ("Error:\n%s", std_err);
					g_free (std_err);
				}
				else anypaper_parameters_write ( window->parameters, lastwallpaperfile, rcfile);
			}
		 	g_free (buffer);
		}
		else g_printerr("Could not open image file");
		g_free (down_filename);
		buffer = g_strdup_printf("%s %s", parameters->command, parameters->defaultfile);
		if (g_spawn_command_line_sync (buffer, &std_out, &std_err, &exitStatus, &err) == FALSE)/*(system(buffer))*/
		{
			g_print("%s\n", err->message);
			g_clear_error(&err);
		}
		else
		{
			if (*std_out != NULL)
			{
				g_print ("Output:\n%s", std_out);
				g_free (std_out);
			}
			if (*std_err != NULL)
			{
				g_printerr ("Error:\n%s", std_err);
				g_free (std_err);
			}
			else anypaper_parameters_write ( window->parameters, lastwallpaperfile, rcfile);
		}
	 	g_free (buffer);

	}
	else
	{
		anypaper_window_create (window);
		anypaper_window_set_position_range(window, rangex, rangey);
		gtk_main();
	}
}
