C = gcc
CFLAGS = -O2 -Wall
ALL = sirna_finder offtarget_search

.PHONY: all clean

all: $(ALL)

sirna_finder: sirna_finder.c
		$(CC) $(CFLAGS) -o $@ $<

offtarget_search: offtarget_search.c
		$(CC) $(CFLAGS) -o $@ $<

clean:
		rm -f $(ALL) *.o
