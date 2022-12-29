INCLUDE_DIRS = 
LIB_DIRS = 
CC=gcc

CDEFS= 
CFLAGS= -O3 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lpthread -lrt -lm

PRODUCT=deadlock_sim

HFILES=
CFILES= deadlock.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	${PRODUCT}

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f ${PRODUCT} ${GARBAGE}

attitude_calculator:	deadlock.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ deadlock.o $(LIBS)

depend:

.c.o:
	$(CC) -MD $(CFLAGS) -c $<
