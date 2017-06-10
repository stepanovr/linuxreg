#Project's make file
INCLUDE=
CFLAGS=         $(INCLUDE) -g   -DDEBUG  -Xlinker -Map=output.map 
TARGET=         out
LIBS =		 -lpthread -lmenu -lform -lncurses -lpanel

CC=gcc
DEPS = memdirect.h ../debug.h
#win_interface.h mmi.h debug.h

OBJ = list.o memdirect.o rdmem.o

#MKDIR_P = mkdir -p

.PHONY: directories

#all: directories try_mmi
all:  try_mmi


#directories: ${TARGET}

#${TARGET}:
#	${MKDIR_P} ${TARGET}

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


EXECUTABLE	= try_mmi


try_mmi: $(OBJ)
#	mkdir out
	gcc -c  $^ $(CFLAGS)  


clean:
	rm *.o 
