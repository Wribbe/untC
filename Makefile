CFLAGS := -g -std=c11 -Wall -Wextra -pedantic -Wwrite-strings

GENERAL_LIBRARIES := -lm -lpthread
GRAPHICS_FLAGS := -lGLEW -lglfw3 -lGL -lX11 -lXrandr -lXi -lXxf86vm \
				  -ldl -lXinerama -lXcursor -lrt

INCLUDES_CLONED := -Igl3w/include
INCLUDES := -Ilib

CC := gcc

DIR_BIN := bin
DIR_LIB := lib
DIR_SRC := src

EXECUTABLES := $(patsubst $(DIR_SRC)/%.c,$(DIR_BIN)/%,$(wildcard $(DIR_SRC)/*.c))
LIBSOURCES := $(wildcard $(DIR_LIB)/*.c)

all: dirs $(EXECUTABLES)

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

$(DIR_BIN)/% : $(DIR_SRC)/%.c gl3w.c $(LIBSOURCES)
	$(CC) $(filter %.c,$^) -o $@ $(CFLAGS) $(INCLUDES) $(INCLUDES_CLONED) $(GENERAL_LIBRARIES) $(GRAPHICS_FLAGS)
