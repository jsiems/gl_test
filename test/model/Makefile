# texmantest makefile

LDIR=-L../../lib -I../../lib

LIBS= -lglfw3 -lgdi32 -lopengl32

IDIR =../../src
CC=gcc
CFLAGS=-Wall -msse3 -g -I$(IDIR) $(LDIR) $(LIBS)

# hide .o files in obj directory
ODIR=../../obj

_DEPS = texman.h wfc.h model.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = modeltest.o texman.o model.o wfc.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

VPATH = $(IDIR)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

modeltest: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)


clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

