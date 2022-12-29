INCLUDE_DIRS = 
LIB_DIRS = 
CC=gcc

CDEFS= 
CFLAGS= -O3 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lpthread -lrt

PRODUCT=heap_mq

HFILES=
CFILES= heap_mq.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	${PRODUCT}

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f ${PRODUCT} ${GARBAGE}

attitude_calculator:	heap_mq.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ heap_mq.o $(LIBS)

depend:

.c.o:
	$(CC) -MD $(CFLAGS) -c $<
