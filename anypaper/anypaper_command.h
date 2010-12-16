/*
 * anypaper_command.h
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

#include <gtk/gtk.h>
#include <stdlib.h>
#include "config.h"
#include "anypaper_parameters.h"
#ifndef __ANYPAPER_COMMAND_H__
#define __ANYPAPER_COMMAND_H__

/**
 * LICENSE:
 * 
 * macro that defines a summary of the license used
 */

#define LICENSE \
"This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.\n\n"\
"This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.\n\n"\
"You should have received a copy of the GNU General Public License along with this program.  If not, see http://www.gnu.org/licenses/."

static void show_version (void);

void show_version_and_exit (void);

void show_license_and_exit (void);

void anypaper_command_load ( AnypaperParameters *parameters );

gboolean custom_scale_connect_state;
gboolean fullscreen, normal, tile, maximize, scale, custom, set, last, no_set;
gboolean info;
gchar *commandline;
int x_position, y_position, height, width;
double  x_scale, y_scale;
gchar *background_color, *lastwallpaperfile, *rcfile, **remaining_args, *wallpapersetterfile;

static GOptionEntry entries[] = 
{
	{ "version", 'v', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, (GOptionArgFunc) show_version_and_exit, "Display version information and exit", NULL },
	{ "license", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, (GOptionArgFunc) show_license_and_exit, "Display license information and exit", NULL },
	{ "fullscreen", 'f', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_NONE, &fullscreen, "Set fullscreen position", NULL },
	{ "normal", 'n', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_NONE, &normal, "Set normal position", NULL },
	{ "tile", 't', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_NONE, &tile, "Set tile position", NULL },
	{ "maximize", 'm', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_NONE, &maximize, "Set maximize position", NULL },
	{ "scale", 's', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_NONE, &scale, "Set scale position", NULL },
	{ "custom", 'c', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_NONE, &custom, "Set custom scale position", NULL },
	{ "set", 'S', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_NONE, &set, "Set wallpaper configuration or command line configuration", NULL },
	{ "last", 'l', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_NONE, &last, "Set the last wallpaper", NULL },
	{ "x-position", 'x', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_INT, &x_position, "Set x position of wallpaper", NULL },
	{ "y-position", 'y', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_INT, &y_position, "Set y position of wallpaper", NULL },
	{ "x-scale", 0, G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_DOUBLE, &x_scale, "Set x scale of wallpaper", NULL },
	{ "y-scale", 0, G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_DOUBLE, &y_scale, "Set y scale of wallpaper", NULL },
	{ "height", 'h', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_INT, &height, "Set x position of wallpaper", NULL },
	{ "width", 'w', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_INT, &width, "Set y position of wallpaper", NULL },
	{ "background", 'b', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_STRING, &background_color, "Set background color", NULL },
	{ "info", 'i', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_NONE, &info, "Show a list of installed wallpapersetter", NULL },
	{ "command", 'u', G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_STRING, &commandline, "Set wallpapersetter command", NULL },
	{ "no-set", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_NONE, &no_set, "Disable setting function", NULL },
	{ "lastwallpaperfile", 'p', G_OPTION_ARG_FILENAME, G_OPTION_ARG_STRING, &lastwallpaperfile, "lastwallpaper file (default: ~/.anypaper/lastwallpaper)", NULL },
	{ "rc", 0, G_OPTION_ARG_FILENAME, G_OPTION_ARG_STRING, &rcfile, "resource file (default: ~/.anypaper/anypaperrc)", NULL },
	{ G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_FILENAME_ARRAY, &remaining_args, NULL, "[FILE...]" },
	{ NULL }
};
#endif

