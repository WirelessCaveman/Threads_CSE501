CC=gcc
CFLAGS=-g 
SRC=mythread.c mythread.h que.h 
OBJ=mythread.o
AR=ar
ARFLAGS=rcs
mythread.a(*.o): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)
tar:
	tar czvf mythread.tar.gz $(SRC) Makefile 

clean:
	\rm $(OBJ) mythread.a 
