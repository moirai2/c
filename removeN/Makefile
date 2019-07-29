PREFIX			= /usr/local/bin
TEST				= test/
CC					= gcc  
CFLAGS			= -O3 -Wall -m64 -std=gnu99
LD					= -lpthread -lm
DEBUGFLAGS		= -ggdb -Wall -m64 -std=gnu99

SOURCES			= advanced_io.c io.c interface.c main.c reader.c regexp.c vector.c

PROGS				= removeN
DEBUGPROGS	= removeN_debug
OBJECTS			= $(SOURCES:.c=.o)
DEBUGOBJECTS	= $(SOURCES:.c=_debug.o)

MANDIR      = /usr/share/man/
MANSUFFIX = 1

all: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LD) -o $(PROGS)
	
%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

debug: $(DEBUGOBJECTS)
	$(CC) $(DEBUGOBJECTS) $(DEBUGFLAGS) $(LD) -o $(DEBUGPROGS)

%_debug.o:	%.c
	$(CC) $(DEBUGFLAGS) -c $< -o $@

clean: 
	@\rm -f $(PROGS) $(OBJECTS)
	@\rm -f $(DEBUGPROGS) $(DEBUGOBJECTS)
	@\rm -f $(THREADPROGS) $(THREADOBJECTS)
	@\rm -f *~
	
