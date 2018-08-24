server: server.o linked_list.o
	cc  server.o linked_list.o -o server

server.o: server.c list_node.h
	cc -c server.c list_node.h

client: client.o
	cc  client.o -o client

client.o: client.c
	cc -c client.c

linked_list.o: linked_list.c list_node.h
	cc -c linked_list.c list_node.h

clean:
	rm server client *.gch *.o output.jpg
