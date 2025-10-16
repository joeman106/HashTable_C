CC     = gcc
CFLAGS = -g -Wall -Wextra -pedantic -Ihelperfunctions -Isrc
EXEC   = main
VPATH  = helperfunctions src

OBJS   = crc64.o getWord.o hashTable.o linkedList.o main.o

.PHONY: all run valgrind clean
all: ${OBJS}

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

VALFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
valgrind: $(EXEC)
	valgrind $(VALFLAGS) ./$(EXEC) $(ARGS)

run: $(EXEC)
	./$(EXEC) ${ARGS}

clean:
	rm -f $(EXEC) $(OBJS)