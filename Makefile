all: UDP-Chat

UDP-Chat: LIST.o UDP-Chat.o
	gcc -o UDP-Chat -pthread LIST.o UDP-Chat.o

LIST.o: LIST.c LIST.h
	gcc -c LIST.c

UDP-Chat.o: UDP-Chat.c
	gcc -c UDP-Chat.c

clean:
	rm -f UDP-Chat *.o
