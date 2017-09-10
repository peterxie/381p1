SHELL := /bin/bash

CPP_FLAGS = -Wall -Wextra -pthread -ldl

EXEC_NAME = p1

CC = gcc
FILES = p1_main.c

OBJ = Collection.c Ordered_container_array.c Record.c Utility.c p1_globals.c 

all: p1
debug: CPP_FLAGS += -g
debug: p1
object: ${OBJ}

p1:
	$(CC) ${VAR} $(CPP_FLAGS) $(FILES) $(OBJ) -o ${EXEC_NAME}

Collection.o: Collection.h Utility.h p1_globals.h Ordered_container.h Record.h
	$(CC) -c $(CPP_FLAGS) Collection.c

Ordered_container_array.o: Ordered_container_array.h Utility.h p1_globals.h
	$(CC) -c $(CPP_FLAGS) Ordered_container_array.c

Record.o: Record.h Utility.h p1_globals.h
	$(CC) -c $(CPP_FLAGS) Record.c

Utility.o: Utility.h 
	$(CC) -c $(CPP_FLAGS) Utility.c

p1_globals.o: p1_globals.h
	$(CC) -c $(CPP_FLAGS) p1_globals.c

clean: 
	rm $(EXEC_NAME) $(OBJ) *~ *.gch
	
