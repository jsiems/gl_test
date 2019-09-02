# main make file

# library directories
LDIR=-Llib -Ilib

# libraries
ifdef SYSTEMROOT
	#windows libraries
	LIBS= -lglfw3_win -lgdi32 -lopengl32
else
	#linux libraries
	LIBS= -lglfw3_linux -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm -lXxf86vm -lXinerama -lXcursor -lrt
endif

IDIR=src
CC=gcc
CFLAGS=-Wall -msse3 -g -I$(IDIR) $(LDIR) $(LIBS)

# hide .o files in obj directory
ODIR=obj

_DEPS = helper/wfc.h render/camera.h render/model.h render/shader.h render/texman.h helper/dirfuncs.h helper/easing.h render/text.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o helper/wfc.o render/shader.o render/model.o render/glad.o render/camera.o render/texman.o helper/dirfuncs.o helper/easing.o render/text.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# tells make to check include directory for dependencies
# without this, you get a 'cannot find name.o' error
VPATH = $(IDIR)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ models/**/*.vrt
	find ./$(ODIR) -name "*.o" -delete

