CC=gcc
CFLAGE=-Wall -O2 -g
ALL=server client
OBJECT=server.o client.o
all:server client
server:server.o
	$(CC) $(CFLAGE) $< -o $@ -lpthread -lsqlite3
client:client.o
	$(CC) $(CFLAGE) $< -o $@
%*.o:%*.c
	$(CC) $(CFLAGE) -c $< -o $@
.PHONY:clean
clean:
	rm -rf $(ALL) $(OBJECT)



