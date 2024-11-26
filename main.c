#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <openssl/aes.h>
#include "aes.h" // Seu algoritmo customizado

void print_hex(const char *label, const unsigned char *data, int len) {
    printf("%s:\n", label);
    for (int i = 0; i < len; i++) {
        printf("%02x ", data[i]);
    }
    printf("\n");
}

// Funções OpenSSL
void openssl_encrypt(const unsigned char *input, unsigned char *output, const unsigned char *key) {
    AES_KEY encryptKey;
    AES_set_encrypt_key(key, 128, &encryptKey);
    AES_encrypt(input, output, &encryptKey);
}

void openssl_decrypt(const unsigned char *input, unsigned char *output, const unsigned char *key) {
    AES_KEY decryptKey;
    AES_set_decrypt_key(key, 128, &decryptKey);
    AES_decrypt(input, output, &decryptKey);
}

int main() {
    char *input_text = "Este é um texto de exemplo que pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppserá74596463 criptografado e pode ter qualquer tamanho.";
    int input_len = strlen(input_text);
    int padded_len = ((input_len + 15) / 16) * 16;

    unsigned char *input = (unsigned char *)calloc(padded_len, sizeof(unsigned char));
    unsigned char *encrypted = (unsigned char *)calloc(padded_len, sizeof(unsigned char));
    unsigned char *decrypted = (unsigned char *)calloc(padded_len, sizeof(unsigned char));
    unsigned char *openssl_encrypted = (unsigned char *)calloc(padded_len, sizeof(unsigned char));
    unsigned char *openssl_decrypted = (unsigned char *)calloc(padded_len, sizeof(unsigned char));

    memcpy(input, input_text, input_len);

    unsigned char key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                             0xab, 0xf7, 0x09, 0xcf, 0x4f, 0x3c, 0x76, 0x2e};

    CUSTOM_AES_KEY custom_aesKey;
   
    if (CUSTOM_AES_set_encrypt_key(key, 128, &custom_aesKey) != 0) {
        printf("Erro ao configurar a chave de criptografia.\n");
        return 1;
    }

    // Custom AES - Criptografia
    clock_t start = clock();
    for (int i = 0; i < padded_len; i += 16) {
        CUSTOM_AES_encrypt(input + i, encrypted + i, &custom_aesKey);
    }
    clock_t end = clock();
    double custom_encrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Custom AES - Tempo de criptografia: %.6f segundos\n", custom_encrypt_time);

    // OpenSSL AES - Criptografia
    start = clock();
    for (int i = 0; i < padded_len; i += 16) {
        openssl_encrypt(input + i, openssl_encrypted + i, key);
    }
    end = clock();
    double openssl_encrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("OpenSSL AES - Tempo de criptografia: %.6f segundos\n", openssl_encrypt_time);

    // Custom AES - Descriptografia
    start = clock();
    for (int i = 0; i < padded_len; i += 16) {
        CUSTOM_AES_decrypt(encrypted + i, decrypted + i, &custom_aesKey);
    }
    end = clock();
    double custom_decrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\nCustom AES - Tempo de descriptografia: %.6f segundos\n", custom_decrypt_time);
    
    // OpenSSL AES - Descriptografia
    start = clock();
    for (int i = 0; i < padded_len; i += 16) {
        openssl_decrypt(openssl_encrypted + i, openssl_decrypted + i, key);
    }
    end = clock();
    double openssl_decrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("OpenSSL AES - Tempo de descriptografia: %.6f segundos\n", openssl_decrypt_time);


    // Verificação do Custom AES
    if (memcmp(input, decrypted, input_len) == 0) {
        printf("\nCustom AES - Sucesso: o texto descriptografado é igual ao original.\n");
    } else {
        printf("\nCustom AES - Erro: o texto descriptografado é diferente do original.\n");
    }

    
    // Verificação do OpenSSL AES
    if (memcmp(input, openssl_decrypted, input_len) == 0) {
        printf("OpenSSL AES - Sucesso: o texto descriptografado é igual ao original.\n");
    } else {
        printf("OpenSSL AES - Erro: o texto descriptografado é diferente do original.\n");
    }

    // Limpar memória
    free(input);
    free(encrypted);
    free(decrypted);
    free(openssl_encrypted);
    free(openssl_decrypted);

    return 0;
}
