#Kevin Kell
#3/25/13

all: player

player: player.c server.c msock.c
	gcc -Wall -g msock.c server.c player.c -o player -I .

clean:
	rm player 
