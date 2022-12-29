INCLUDE_DIRS = 
LIB_DIRS = 
CC=gcc

CDEFS= 
CFLAGS= -O3 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lpthread -lrt -lm -Wall

PRODUCT=attitude_calculator

HFILES=
CFILES= attitude.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	${PRODUCT}

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f ${PRODUCT} ${GARBAGE}

attitude_calculator:	attitude.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ attitude.o $(LIBS)

depend:

.c.o:
	$(CC) -MD $(CFLAGS) -c $<
