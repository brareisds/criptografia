// gcc -o aes aes.c main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "aes.h"

void print_hex(const char *label, const unsigned char *data, int len) {
    printf("%s:\n", label);
    for (int i = 0; i < len; i++) {
        printf("%02x ", data[i]);
    }
    printf("\n");
}

int main() {
    // Entrada de teste dinâmica
    char *input_text = "Este é um texto de exemplo que será criptografado e pode ter qualquer tamanho.";
    int input_len = strlen(input_text);

    // Ajustar o tamanho para múltiplos de 16 bytes
    int padded_len = ((input_len + 15) / 16) * 16;

    // Alocar memória para os buffers
    unsigned char *input = (unsigned char *)calloc(padded_len, sizeof(unsigned char));
    unsigned char *encrypted = (unsigned char *)calloc(padded_len, sizeof(unsigned char));
    unsigned char *decrypted = (unsigned char *)calloc(padded_len, sizeof(unsigned char));

    // Copiar o texto de entrada para o buffer de entrada
    memcpy(input, input_text, input_len);

    // Chave de teste (16 bytes para AES-128)
    unsigned char key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                             0xab, 0xf7, 0x09, 0xcf, 0x4f, 0x3c, 0x76, 0x2e};

    // Estrutura para armazenar as chaves expandidas
    AES_KEY aesKey;

    // Configurar a chave para criptografia
    if (AES_set_encrypt_key(key, 128, &aesKey) != 0) {
        printf("Erro ao configurar a chave de criptografia.\n");
        free(input);
        free(encrypted);
        free(decrypted);
        return 1;
    }

    // Criptografar cada bloco de 16 bytes
    for (int i = 0; i < padded_len; i += 16) {
        AES_encrypt(input + i, encrypted + i, &aesKey);
    }
    print_hex("Texto criptografado", encrypted, padded_len);

    // Configurar a chave para descriptografia
    if (AES_set_decrypt_key(key, 128, &aesKey) != 0) {
        printf("Erro ao configurar a chave de descriptografia.\n");
        free(input);
        free(encrypted);
        free(decrypted);
        return 1;
    }

    // Descriptografar cada bloco de 16 bytes
    for (int i = 0; i < padded_len; i += 16) {
        AES_decrypt(encrypted + i, decrypted + i, &aesKey);
    }
    print_hex("Texto descriptografado", decrypted, padded_len);

    // Verificar se o texto descriptografado é igual ao original
    if (memcmp(input, decrypted, input_len) == 0) {
        printf("Sucesso: o texto descriptografado é igual ao original.\n");
    } else {
        printf("Erro: o texto descriptografado é diferente do original.\n");
    }

    // Liberar a memória alocada
    free(input);
    free(encrypted);
    free(decrypted);

    return 0;
}