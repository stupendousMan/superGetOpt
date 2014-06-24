
CC=gcc
CFLAGS = -Wall -ggdb -O3
#CC=/opt/gcc-4.0.2-bc/bin/gcc
#CFLAGS += --bounds-checking

TEMPFILES = core *.core 

PROGS = libSuperGet.a testSuperGetOpt

LIB_OBJS = \
	superGetOpt.o 

TEST_OBJS = testSuperGetOpt.o

all:    ${PROGS}

libSuperGet.a:	${LIB_OBJS}
	ar ruv $@ $?
	ranlib $@
	
testSuperGetOpt:	${TEST_OBJS} libSuperGet.a
	${CC} -o $@ ${CFLAGS} ${TEST_OBJS} -L./ -lSuperGet 

clean:
	rm -f ${PROGS} ${LIB_OBJS} ${TEMPFILES}

