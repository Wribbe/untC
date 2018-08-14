CFLAGS := -g -std=c11 -Wall -Wextra -pedantic -Wwrite-strings

GENERAL_LIBRARIES := -lm -lpthread
GRAPHICS_FLAGS := -lGLEW -lglfw3 -lGL -lX11 -lXrandr -lXi -lXxf86vm \
				  -ldl -lXinerama -lXcursor -lrt -lpng -lz

DIR_BIN := bin
DIR_LIB := lib
DIR_SRC := src
DIR_LIBPNG := libpng


INCLUDES_CLONED := -Igl3w/include
INCLUDES := -Ilib -I$(DIR_LIBPNG) -L$(DIR_LIBPNG)

CC := gcc

EXECUTABLES := $(patsubst $(DIR_SRC)/%.c,$(DIR_BIN)/%,$(wildcard $(DIR_SRC)/*.c))
LIBFILES := $(wildcard $(DIR_LIB)/*)

SYMLINKS := lib/png.h lib/png.c lib/pngerror.c lib/pngmem.c

all: dirs $(SYMLINKS) $(EXECUTABLES)

vpath %.c gl3w/src

dirs:
# Check if gl3w is present.
	@[ -d "./gl3w" ] || { \
		git clone https://github.com/skaslev/gl3w && \
		cd gl3w && \
		./gl3w_gen.py \
	;}
# Check if bin exists.
	@[ -d $(DIR_BIN) ] || mkdir $(DIR_BIN)
# Check if libpng exists
	@[ -d "./libpng" ] || { \
		git clone git://git.code.sf.net/p/libpng/code $(DIR_LIBPNG) && \
		cd $(DIR_LIBPNG) && \
		./autogen.sh && \
		./configure && \
		make && \
		make check \
	;}

$(DIR_BIN)/% : $(DIR_SRC)/%.c gl3w.c $(LIBFILES)
	$(CC) $(filter %.c,$^) -o $@ $(CFLAGS) $(INCLUDES) $(INCLUDES_CLONED) $(GENERAL_LIBRARIES) $(GRAPHICS_FLAGS)

lib/png.h:
	@ln -rs $(DIR_LIBPNG)/png.h $@

lib/png.c:
	@ln -rs $(DIR_LIBPNG)/png.c $@

lib/pngerror.c:
	@ln -rs $(DIR_LIBPNG)/pngerror.c $@

lib/pngmem.c:
	@ln -rs $(DIR_LIBPNG)/pngmem.c $@

.PHONY: all
