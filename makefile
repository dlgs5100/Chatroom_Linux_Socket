all: serverChatroom.c clientChatroom.c
	gcc serverChatroom.c -o server -lpthread
	gcc clientChatroom.c -o client -lpthread
clean:
	rm -f server
	rm -f client
