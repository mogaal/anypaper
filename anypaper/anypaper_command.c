/*
 * anypaper_command.c
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
 * SECTION:anypaper_command
 * @short_description: command-line control functions
 * @see_also: #AnypaperImage, #AnypaperParameters
 * @stability: Stable
 * @include: anypaper_preview.h
 *
 * Set of function that controls the parameters passed by the command line and the behavior of anyPaper related to them.
 * 
 * Since: 2.0
 */

#include <gtk/gtk.h>
#include "anypaper_command.h"

static void show_version (void)
{
  g_print ("%s version %s", "anyPaper", VERSION);
  g_print ("\n");
}

/**
 * show_version_and_exit:
 * 
 * shows the version of anyPaper and exits
 */
void show_version_and_exit (void)
{
  show_version ();

  exit (EXIT_SUCCESS);
}

/**
 * show_license_and_exit:
 * 
 * shows the version of anyPaper and the content of #LICENSE and exits
 */

void show_license_and_exit (void)
{
  show_version ();

  g_print ("\n" LICENSE "\n\n");

  exit (EXIT_SUCCESS);
}

static int option_command ()
{
	int c=6;
	if (custom == TRUE) c=5;
	if (scale == TRUE) c=4;
	if (maximize == TRUE) c=3;
	if (tile == TRUE) c=2;
	if (normal == TRUE) c=1;
	if (fullscreen == TRUE) c=0;
	return c;
}

/**
 * anypaper_command_load:
 * @parameters: object that contains the parameters used in anyPaper
 * 
 * This function reads the parameters passed by the command line and sets the value to the right member of #AnypaperParameters
 */
void anypaper_command_load (AnypaperParameters *parameters)
{
	int e=6;
	g_print("%s", background_color);
	e=option_command();
	if (e != 6) parameters->style = e;
	if (remaining_args != 0)
	{
		if (g_file_test(remaining_args[0], G_FILE_TEST_EXISTS)) parameters->file = g_strdup_printf("%s", remaining_args[0]);
		else g_printerr("File not found (%s)\n", remaining_args[0]);
	}
	if (x_position != -65536) parameters->positionx=x_position;
	if (y_position != -65536) parameters->positiony=y_position;
	if (x_scale != -1) parameters->scalex=x_scale;
	if (y_scale != -1) parameters->scaley=y_scale;
	if (width != -65536) parameters->width=width;
	if (height != -65536) parameters->height=height;
	if (background_color != 0) parameters->background = g_strdup_printf("%s", background_color);
	if (commandline != 0) parameters->command = g_strdup_printf("%s", commandline);
}

