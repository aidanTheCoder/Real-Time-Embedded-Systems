INCLUDE_DIRS = 
LIB_DIRS = 
CC=gcc

CDEFS= 
CFLAGS= -O3 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lpthread -lrt -lm -Wall

PRODUCT=timeout

HFILES=
CFILES= deadlock_timeout.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	${PRODUCT}

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f ${PRODUCT} ${GARBAGE}

attitude_calculator:	deadlock_timeout.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ deadlock_timeout.o $(LIBS)

depend:

.c.o:
	$(CC) -MD $(CFLAGS) -c $<
