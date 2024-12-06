CFLAGS = -Wall -Wextra -fopenmp
LDFLAGS = -lssl -lcrypto

# Alvo principal
aes: aes.o main.o
	gcc $(CFLAGS) -o aes aes.o main.o $(LDFLAGS)

# Alvo para aes.o
aes.o: aes.c aes.h
	gcc $(CFLAGS) -c aes.c -o aes.o

# Alvo para main.o
main.o: main.c aes.h
	gcc $(CFLAGS) -c main.c -o main.o

# Limpeza de arquivos temporários
clean:
	rm -f *.o aes