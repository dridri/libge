CC	=	mingw32-gcc
C++	=	mingw32-g++
CFLAGS	= -O3 -Wall -mfpmath=sse -msse

TARGET	 =	"[3d]01-scene.exe"
LIBS	 =  -lge
 # -mwindows

OBJS = main.o

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS) $(DEFS)



%.o: %.c
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $@
