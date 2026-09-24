 CC = gcc
AR = ar

CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -Llib
LDLIBS = -lschedule -lsqlite3

TARGET = schedule_generator.exe
LIBRARY = lib/libschedule.a

MAIN_OBJ = build/main.o

LIB_OBJS = \
	build/database.o \
	build/parser.o \
	build/schedule.o \
	build/office_hours.o \
	build/output.o

all: $(TARGET)

$(TARGET): $(MAIN_OBJ) $(LIBRARY)
	$(CC) $(MAIN_OBJ) $(LDFLAGS) $(LDLIBS) -o $(TARGET)

$(LIBRARY): $(LIB_OBJS)
	$(AR) rcs $(LIBRARY) $(LIB_OBJS)

build/main.o: src/main.c include/database.h include/parser.h include/output.h
	$(CC) $(CFLAGS) -c src/main.c -o build/main.o

build/database.o: src/database.c include/database.h
	$(CC) $(CFLAGS) -c src/database.c -o build/database.o

build/parser.o: src/parser.c include/parser.h
	$(CC) $(CFLAGS) -c src/parser.c -o build/parser.o

build/schedule.o: src/schedule.c include/schedule.h
	$(CC) $(CFLAGS) -c src/schedule.c -o build/schedule.o

build/office_hours.o: src/office_hours.c include/office_hours.h
	$(CC) $(CFLAGS) -c src/office_hours.c -o build/office_hours.o

build/output.o: src/output.c include/output.h
	$(CC) $(CFLAGS) -c src/output.c -o build/output.o

run: $(TARGET)
	.\$(TARGET)

clean:
	-del /Q build\*.o 2>NUL
	-del /Q lib\libschedule.a 2>NUL
	-del /Q schedule_generator.exe 2>NUL

.PHONY: all run clean