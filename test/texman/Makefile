# texmantest makefile

LDIR=-L../../lib -I../../lib

#LIBS= -lglfw3 -lgdi32 -lopengl32

IDIR =../../src
CC=gcc
CFLAGS=-Wall -g -I$(IDIR) $(LDIR) $(LIBS)

# hide .o files in obj directory
ODIR=../../obj

_DEPS = texman.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = texmantest.o texman.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

VPATH = $(IDIR)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

texmantest: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)


clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

