CC	=	mingw32-gcc
C++	=	mingw32-g++
CFLAGS	= -g -O3 -Wall -mfpmath=sse -msse

TARGET	 =	"01-image.exe"
LIBS	 =  -lge -mwindows

OBJS = main.o

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS) $(DEFS)



%.o: %.c
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $@
