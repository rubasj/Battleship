BIN = server

OBJ = main.o player.o manager.o game.o

CC = gcc
CCFLAGS = -g


$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN) -lpthread

game.o: game.c
	$(CC) -c game.c $(CCFLAGS)

manager.o: manager.c
	$(CC) -c manager.c $(CCFLAGS)

client.o: player.c
	$(CC) -c player.c $(CCFLAGS)

main.o: main.c
	$(CC) -c main.c $(CCFLAGS)

clean:
	rm *.o $(BIN) -f