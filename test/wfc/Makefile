
# cmake tutorial here: http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# ver gud

# library settings untested
# library directories
# LDIR=

# libraries
# LIBS=

IDIR =../../src
CC=gcc
CFLAGS=-Wall -g -I$(IDIR) #-L$(LDIR)

# hide .o files in obj directory
ODIR=../../obj

_DEPS = wfc.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = wfctest.o wfc.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# tells make to check include directory for dependencies
# without this, you get a 'cannot find name.o' error
VPATH = $(IDIR)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) #maybe libs here

wfctest: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) #$(LIBS)


clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

