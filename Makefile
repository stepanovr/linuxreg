#Project's make file
INCLUDE=
CFLAGS=         $(INCLUDE) -g   -DDEBUG  -Xlinker -Map=output.map 
TARGET=         out
LIBS =		 -lpthread -lmenu -lform -lncurses -lpanel

CC=gcc
DEPS = win_interface.h mmi.h debug.h
OBJ = mmi_core.o  panel.o win_0.o win_1.o win_2.o win_3.o win_init.o

MKDIR_P = mkdir -p

.PHONY: directories

all: directories try_mmi


directories: ${TARGET}

${TARGET}:
	${MKDIR_P} ${TARGET}

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


EXECUTABLE	= try_mmi


try_mmi: $(OBJ)
#	mkdir out
	+$(MAKE) -C app
	gcc -o $(TARGET)/$@ $^ $(CFLAGS)   $(LIBS) app/*.o


clean:
	rm *.o app/*.o $(TARGET)/$(EXECUTABLE)
