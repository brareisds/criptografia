#include "aes.h" // Implementação customizada do AES
#include <openssl/evp.h> // OpenSSL para EVP API
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h> // Para multithreading

#define BLOCK_SIZE (128 * 1024) // 128 KB

// Função para criptografar usando OpenSSL (EVP API)
void openssl_encrypt(const unsigned char *input, unsigned char *output, const unsigned char *key) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len;
    EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL);
    EVP_EncryptUpdate(ctx, output, &len, input, 16);
    EVP_EncryptFinal_ex(ctx, output + len, &len);
    EVP_CIPHER_CTX_free(ctx);
}

// Função para descriptografar usando OpenSSL (EVP API)
void openssl_decrypt(const unsigned char *input, unsigned char *output, const unsigned char *key) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len;
    EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL);
    EVP_DecryptUpdate(ctx, output, &len, input, 16);
    EVP_DecryptFinal_ex(ctx, output + len, &len);
    EVP_CIPHER_CTX_free(ctx);
}

// Função para verificar se os arquivos são idênticos
int compare_buffers(const unsigned char *buffer1, const unsigned char *buffer2, size_t size) {
    return memcmp(buffer1, buffer2, size) == 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <arquivo_entrada> <chave>\n", argv[0]);
        return 1;
    }

    // Abrir arquivo de entrada no modo leitura binária
    FILE *input_file = fopen(argv[1], "rb");
    if (!input_file) {
        printf("Erro: O arquivo de entrada '%s' não pôde ser aberto.\n", argv[1]);
        return 1;
    }

    // Preparar chave
    unsigned char key[16];
    strncpy((char *)key, argv[2], 16);


    CUSTOM_AES_KEY custom_aesKey;

    // Configura a chave de criptografia AES personalizada com uma chave de 128 bits
    // A função CUSTOM_AES_set_encrypt_key realiza as seguintes ações:
    // 1. Verifica se a chave do usuário ou a estrutura da chave são nulas.
    // 2. Verifica se o tamanho da chave é válido (128, 192 ou 256 bits).
    // 3. Define o número de rodadas com base no tamanho da chave (10 para 128 bits, 12 para 192 bits, 14 para 256 bits).
    // 4. Expande a chave para as rodadas de criptografia usando a função KeyExpansion.
    // 5. Deriva uma seed a partir da chave do usuário usando a função derive_seed, onde bits / 8 representa o comprimento da chave em bytes.
    if (CUSTOM_AES_set_encrypt_key(key, 128, &custom_aesKey) != 0) {
        printf("Erro ao configurar a chave.\n");
        fclose(input_file);
        return 1;
    }

    // Buffers para processamento
    unsigned char *buffer_in = malloc(BLOCK_SIZE);
    unsigned char *buffer_out = malloc(BLOCK_SIZE);
    unsigned char *buffer_decrypted = malloc(BLOCK_SIZE);
    if (!buffer_in || !buffer_out || !buffer_decrypted) {
        printf("Erro: Falha ao alocar memória para buffers.\n");
        fclose(input_file);
        return 1;
    }

    // Criptografia customizada
    size_t bytes_read;
    clock_t start = clock();
    while ((bytes_read = fread(buffer_in, 1, BLOCK_SIZE, input_file)) > 0) {
        size_t blocks = (bytes_read + 15) / 16; // Número de blocos de 16 bytes
        #pragma omp parallel for
        for (size_t i = 0; i < blocks; i++) {
            CUSTOM_AES_encrypt(buffer_in + (i * 16), buffer_out + (i * 16), &custom_aesKey);
        }
    }
    clock_t end = clock();
    double custom_encrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Descriptografia customizada
    start = clock();
    size_t blocks = (bytes_read + 15) / 16;
    #pragma omp parallel for
    for (size_t i = 0; i < blocks; i++) {
        CUSTOM_AES_decrypt(buffer_out + (i * 16), buffer_decrypted + (i * 16), &custom_aesKey);
    }
    end = clock();
    double custom_decrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Comparar buffers (custom)
    int custom_success = compare_buffers(buffer_in, buffer_decrypted, bytes_read);

    // Resetar ponteiros
    fseek(input_file, 0, SEEK_SET);

    // Criptografia com OpenSSL
    start = clock();
    while ((bytes_read = fread(buffer_in, 1, BLOCK_SIZE, input_file)) > 0) {
        size_t blocks = (bytes_read + 15) / 16;
        for (size_t i = 0; i < blocks; i++) {
            openssl_encrypt(buffer_in + (i * 16), buffer_out + (i * 16), key);
        }
    }
    end = clock();
    double openssl_encrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Descriptografia com OpenSSL
    start = clock();
    blocks = (bytes_read + 15) / 16;
    for (size_t i = 0; i < blocks; i++) {
        openssl_decrypt(buffer_out + (i * 16), buffer_decrypted + (i * 16), key);
    }
    end = clock();
    double openssl_decrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Comparar buffers (OpenSSL)
    int openssl_success = compare_buffers(buffer_in, buffer_decrypted, bytes_read);

    // Liberar buffers
    free(buffer_in);
    free(buffer_out);
    free(buffer_decrypted);

    // Fechar arquivo de entrada
    fclose(input_file);

    // Exibir resultados
    printf("Custom Encrypt: %.8f seconds\n", custom_encrypt_time);
    printf("Custom Decrypt: %.8f seconds\n", custom_decrypt_time);
    printf("Custom Decrypt Success: %s\n", custom_success ? "Yes" : "No");
    printf("OpenSSL Encrypt: %.8f seconds\n", openssl_encrypt_time);
    printf("OpenSSL Decrypt: %.8f seconds\n", openssl_decrypt_time);
    printf("OpenSSL Decrypt Success: %s\n", openssl_success ? "Yes" : "No");

    return 0;
}