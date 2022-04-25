CFLAGS = -std=c99 -g -Wall -Ofast -ffast-math
LIBS   =
ODIR   = bin
SRC    = src

_OBJ = steric_gas container event_chain particle
OBJ  = $(patsubst %, $(ODIR)/%.o, $(_OBJ))

.PHONY: all clean debug

all: $(ODIR)/EventChain

debug: CFLAGS=-std=c99 -g -O0 -Wall -DDEBUG
debug: $(ODIR)/EventChain

$(ODIR)/EventChain: $(SRC)/main.c $(OBJ) | $(ODIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(ODIR)/%.o: $(SRC)/%.c $(SRC)/%.h | $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR):
	mkdir $(ODIR)

clean:
	rm -rf $(ODIR)

