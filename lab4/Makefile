MATH_FLAG = -lm
PIC_FLAG = -fPIC
SHARED_FLAG = -shared
DYNAMIC_LOADING_FLAG = -ldl
EXT = c
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall

all: program1 program2 dynamic_realization1

program1: stat_main.$(EXT) dynamic_realization1 function.h
	$(CC) $(CFLAGS) stat_main.$(EXT) -L. ./libfirst_realization.so -o program1 $(MATH_FLAG)

program2: dyn_main dynamic_realization1 dynamic_realization2
	$(CC) $(CFLAGS) dyn_main.o -o program2 $(DYNAMIC_LOADING_FLAG)

dyn_main: dyn_main.$(EXT)
	$(CC) $(CFLAGS) -c dyn_main.$(EXT)

dynamic_realization2: second_realization.$(EXT) function.h
	$(CC) $(CFLAGS) $(PIC_FLAG) $(SHARED_FLAG) -o libsecond_realization.so second_realization.$(EXT) $(MATH_FLAG)

dynamic_realization1: first_realization.$(EXT) function.h
	$(CC) $(CFLAGS) $(PIC_FLAG) $(SHARED_FLAG) -o libfirst_realization.so first_realization.$(EXT) $(MATH_FLAG)

clean:
	rm *.o *.so program1 program2
