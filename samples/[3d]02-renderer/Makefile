CC	=	gcc
C++	=	g++
CFLAGS	= -O3 -Wall -mfpmath=sse -msse

TARGET	 =	02-renderer
LIBS	 =  -lge
 # -mwindows

OBJS = main.o

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS) $(DEFS)



%.o: %.c
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $@
