INCLUDE_DIRS = 
LIB_DIRS = 
CC=gcc

CDEFS= 
CFLAGS= -O3 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lpthread -lrt -ltr

PRODUCT=posix_mq

HFILES=
CFILES= posix_mq.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	${PRODUCT}

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f ${PRODUCT} ${GARBAGE}

attitude_calculator:	posix_mq.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ posix_mq.o $(LIBS)

depend:

.c.o:
	$(CC) -MD $(CFLAGS) -c $<
