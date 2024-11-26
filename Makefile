CFLAGS = -Wall -Wextra
LDFLAGS = -lssl -lcrypto

aes: aes.o main.o
	gcc $(CFLAGS) -o aes aes.o main.o $(LDFLAGS)

aes.o: aes.c aes.h
	gcc $(CFLAGS) -c aes.c -o aes.o

main.o: main.c aes.h
	gcc $(CFLAGS) -c main.c -o main.o

clean:
	rm -f *.o aes
