# main make file

# library directories
LDIR=-Llib -Ilib

# libraries
LIBS= -lglfw3 -lgdi32 -lopengl32

IDIR=src
CC=gcc
CFLAGS=-Wall -msse3 -I$(IDIR) $(LDIR) $(LIBS)

# hide .o files in obj directory
ODIR=obj

_DEPS = wfc.h camera.h model.h shader.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o wfc.o shader.o model.o glad.o camera.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# tells make to check include directory for dependencies
# without this, you get a 'cannot find name.o' error
VPATH = $(IDIR)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)


clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

