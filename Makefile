# Nome do executável
TARGET = aes

# Compilador
CC = gcc

# Flags do compilador
CFLAGS = -Wall -Wextra -Werror

# Arquivos fonte
SRCS = aes.c main.c

# Arquivos objeto
OBJS = $(SRCS:.c=.o)

# Regra padrão
all: $(TARGET)

# Regra para criar o executável
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Regra para criar os arquivos objeto
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza dos arquivos objeto e do executável
clean:
	rm -f $(OBJS) $(TARGET)
