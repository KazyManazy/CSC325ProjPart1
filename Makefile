CC = gcc
serverclientmake: server.c client.c
	$(CC) -o server server.c
	$(CC) -o client client.c