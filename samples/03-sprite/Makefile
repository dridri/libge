CC	=	mingw32-gcc
C++	=	mingw32-g++
CFLAGS	= -O3 -Wall -mfpmath=sse -msse

TARGET	 =	"03-sprite.exe"
LIBS	 =  -lge -mwindows
# -lopengl32 -lglu32 -lwin32k -luser32 -lm -lwinmm -lgdi32
#  -lopencl

OBJS = main.o

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS) $(DEFS)



%.o: %.c
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $@
