.PHONY:clean
.PHONY:test

LIBS:= -lhdf5 -lhdf5_hl
CFLAGS:= -Wall -ggdb

h5test: main.c
	g++ -o $@ $(CFLAGS) $< $(LIBS)

test: h5test
	./h5test && octave --no-gui show_h5.m

clean:
	rm -f h5test *.h5 *.png octave-workspace
