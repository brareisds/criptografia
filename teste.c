// gcc -o aes_test aes.c teste.c

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "aes.h" // Substitua pelo nome do arquivo que contém sua implementação

int main() {
    // Entrada de teste (16 bytes para AES-128)
    unsigned char input[16] = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
                               0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34};

    // Chave de teste (16 bytes para AES-128)
    unsigned char key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                             0xab, 0xf7, 0x09, 0xcf, 0x4f, 0x3c, 0x76, 0x2e};

    // Buffers para os dados criptografados e descriptografados
    unsigned char encrypted[16];
    unsigned char decrypted[16];

    // Estrutura para armazenar as chaves expandidas
    AES_KEY aesKey;

    // Configurar a chave para criptografia
    if (AES_set_encrypt_key(key, 128, &aesKey) != 0) {
        printf("Erro ao configurar a chave de criptografia.\n");
        return 1;
    }

    // Criptografar
    AES_encrypt(input, encrypted, &aesKey);
    printf("Texto criptografado:\n");
    for (int i = 0; i < 16; i++) {
        printf("%02x ", encrypted[i]);
    }
    printf("\n");

    // Configurar a chave para descriptografia
    if (AES_set_decrypt_key(key, 128, &aesKey) != 0) {
        printf("Erro ao configurar a chave de descriptografia.\n");
        return 1;
    }

    // Descriptografar
    AES_decrypt(encrypted, decrypted, &aesKey);
    printf("Texto descriptografado:\n");
    for (int i = 0; i < 16; i++) {
        printf("%02x ", decrypted[i]);
    }
    printf("\n");

    // Verificar se o texto descriptografado é igual ao original
    if (memcmp(input, decrypted, 16) == 0) {
        printf("Sucesso: o texto descriptografado é igual ao original.\n");
    } else {
        printf("Erro: o texto descriptografado é diferente do original.\n");
    }

    return 0;
}
