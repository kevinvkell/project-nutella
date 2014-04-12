#Kevin Kell
#3/25/13

all: player

player: player.c server.c
	gcc -Wall -g server.c player.c -o player -I .

clean:
	rm player 
