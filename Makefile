CC = gcc
BIN = server
OBJ = player.o board.o game.o main.o


$(BIN): $(OBJ)
	$(CC) *.o -o ${BIN} -lpthread

%.o: %.c
	$(CC) -c *.c
