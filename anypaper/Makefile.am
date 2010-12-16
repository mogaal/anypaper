bin_PROGRAMS = anypaper

noinst_LTLIBRARIES = libanypaper.la

INCLUDES =						\
	$(ANYPAPER_CFLAGS)				\
	$(DISABLE_DEPRECATED_CFLAGS)			\
	-DANYPAPER_DATA=\""$(datadir)/anypaper/"\"

anypaper_SOURCES = anypaper.c

anypaper_LDADD = libanypaper.la $(ANYPAPER_LIBS)

libanypaper_la_LIBADD = $(ANYPAPER_LIBS)

NOINST_H_FILES =	anypaper_image.h\
			anypaper_command.h\
			anypaper_window.h\
			anypaper_parameters.h\
			anypaper_preview.h\
			anypaper_wallpapersetter.h

libanypaper_la_SOURCES = anypaper_image.c\
			anypaper_command.c\
			anypaper_window.c\
			anypaper_parameters.c\
			anypaper_preview.c\
			anypaper_wallpapersetter.c\
			$(NOINST_H_FILES)

