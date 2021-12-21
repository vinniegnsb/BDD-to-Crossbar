CC 	= gcc
IDIR	= include
ODIR	= obj
SDIR	= src
SRC	= $(wildcard $(SDIR)/%.c)
CFLAGS	= -I$(IDIR)
_DEPS 	= general_functions.h xbar.h bdd.h
DEPS	= $(patsubst %, $(IDIR)/%, $(_DEPS))
_OBJ 	= main.o general_functions.o xbar.o bdd.o
OBJ	= $(patsubst %, $(ODIR)/%, $(_OBJ))
TARGET	= bddxbar

# Note: $@ and $^ mean left and right side of : respectively
# 	$< means first item in DEPS list (in this case)

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(TARGET)
