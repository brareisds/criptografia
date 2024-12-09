#include "aes.h"
#include <openssl/evp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <openssl/rand.h>

#define BLOCK_SIZE 16

// Função para comparar os primeiros n bytes de dois arquivos
void compare_file_bytes(const char *file1, const char *file2, size_t n) {
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");
    unsigned char b1[n], b2[n];
    fread(b1, 1, n, f1);
    fread(b2, 1, n, f2);
    fclose(f1);
    fclose(f2);
    for (size_t i = 0; i < n; i++) {
        if (b1[i] != b2[i]) {
            printf("Byte %zu: %02x != %02x\n", i, b1[i], b2[i]);
        }
    }
}

// Função para comparar dois arquivos byte a byte
int compare_files(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");
    if (!f1 || !f2) {
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return 0;
    }

    int result = 1;
    char b1[BLOCK_SIZE], b2[BLOCK_SIZE];
    size_t r1, r2;

    while ((r1 = fread(b1, 1, BLOCK_SIZE, f1)) > 0 &&
           (r2 = fread(b2, 1, BLOCK_SIZE, f2)) > 0) {
        if (r1 != r2 || memcmp(b1, b2, r1) != 0) {
            result = 0;
            break;
        }
    }

    fclose(f1);
    fclose(f2);
    return result;
}

int main(int argc, char *argv[]) {
    // Verifica se o número de argumentos é válido
    if (argc < 3 || argc > 5) {
        printf("Uso: %s <arquivo_entrada> <tamanho_chave_em_bits> [operacao] [chave_hex (apenas para decrypt)]\n", argv[0]);
        printf("Operacao (opcional): encrypt, decrypt ou both (default: encrypt)\n");
        return 1;
    }

    // Abre o arquivo de entrada
    FILE *input_file = fopen(argv[1], "rb");
    if (!input_file) {
        printf("Erro: O arquivo de entrada '%s' não pôde ser aberto.\n", argv[1]);
        return 1;
    }

    // Verifica o tamanho da chave
    int key_bits = atoi(argv[2]);
    if (key_bits != 128 && key_bits != 192 && key_bits != 256) {
        printf("Erro: Tamanho da chave inválido. Use 128, 192 ou 256.\n");
        fclose(input_file);
        return 1;
    }

    // Determina a operação a ser realizada
    char *operation = (argc >= 4) ? argv[3] : "encrypt";
    int do_encrypt = strcmp(operation, "encrypt") == 0 || strcmp(operation, "both") == 0;
    int do_decrypt = strcmp(operation, "decrypt") == 0 || strcmp(operation, "both") == 0;

    if (!do_encrypt && !do_decrypt) {
        printf("Erro: Operação inválida. Use 'encrypt', 'decrypt' ou 'both'.\n");
        fclose(input_file);
        return 1;
    }

    // Gera ou lê a chave
    unsigned char *key = NULL;
    int key_bytes = key_bits / 8;
    if (do_decrypt && strcmp(operation, "decrypt") == 0) {
        if (argc < 5) {
            printf("Erro: A operação 'decrypt' requer a chave em formato hexadecimal como argumento.\n");
            fclose(input_file);
            return 1;
        }

        key = malloc(key_bytes);

        // Verifica se a chave é nula ou se o comprimento da string hexadecimal não corresponde ao esperado
        if (!key || strlen(argv[4]) != (size_t)(key_bytes * 2)) {
            printf("Erro: Chave inválida.\n");
            free(key); // Libera a memória alocada para a chave
            fclose(input_file); // Fecha o arquivo de entrada
            return 1; // Retorna 1 para indicar falha
        }

        // Converte a string hexadecimal fornecida em um array de bytes
        for (int i = 0; i < key_bytes; i++) {
            sscanf(argv[4] + 2 * i, "%2hhx", &key[i]);
        }

    } else if (do_encrypt || strcmp(operation, "both") == 0) {
        key = malloc(key_bytes);
        if (!key || RAND_bytes(key, key_bytes) != 1) {
            printf("Erro: Falha ao gerar chave aleatória.\n");
            free(key);
            fclose(input_file);
            return 1;
        }
        printf("Chave gerada (%d bits): ", key_bits);
        for (int i = 0; i < key_bytes; i++) {
            printf("%02x", key[i]);
        }
        printf("\n");
    }

    // Configura a chave para criptografia
    CUSTOM_AES_KEY custom_aesKey;
    if (CUSTOM_AES_set_encrypt_key(key, key_bits, &custom_aesKey) != 0) {
        printf("Erro ao configurar a chave.\n");
        free(key);
        fclose(input_file);
        return 1;
    }

    // Aloca buffers para criptografia e descriptografia
    unsigned char *buffer_in = malloc(BLOCK_SIZE);
    unsigned char *buffer_out = malloc(BLOCK_SIZE);
    unsigned char *buffer_decrypted = malloc(BLOCK_SIZE);

    if (!buffer_in || !buffer_out || !buffer_decrypted) {
        printf("Erro: Falha ao alocar memória.\n");
        free(buffer_in);
        free(buffer_out);
        free(buffer_decrypted);
        free(key);
        fclose(input_file);
        return 1;
    }

    size_t bytes_read;
    double custom_encrypt_time = 0, custom_decrypt_time = 0;

    // Realiza a criptografia se necessário
    if (do_encrypt) {
        FILE *encrypted_file = fopen("encrypted_file.aes", "wb");
        if (!encrypted_file) {
            printf("Erro: Não foi possível criar o arquivo criptografado.\n");
            free(buffer_in);
            free(buffer_out);
            free(buffer_decrypted);
            free(key);
            fclose(input_file);
            return 1;
        }
        fseek(input_file, 0, SEEK_SET);

        clock_t start = clock();
        while ((bytes_read = fread(buffer_in, 1, BLOCK_SIZE, input_file)) > 0) {
            size_t blocks = (bytes_read + 15) / 16;
            #pragma omp parallel for
            for (size_t i = 0; i < blocks; i++) {
                CUSTOM_AES_encrypt(buffer_in + (i * 16), buffer_out + (i * 16), &custom_aesKey);
            }
            fwrite(buffer_out, 1, bytes_read, encrypted_file);
        }

        clock_t end = clock();
        custom_encrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        fclose(encrypted_file);
        printf("Arquivo criptografado gerado: encrypted_file.aes\n");
    }

    // Realiza a descriptografia se necessário
    if (do_decrypt) {
        FILE *encrypted_file = fopen("encrypted_file.aes", "rb");
        FILE *decrypted_file = fopen("decrypted_file.txt", "wb");

        if (!encrypted_file || !decrypted_file) {
            printf("Erro: Não foi possível abrir arquivos para descriptografia.\n");
            free(buffer_in);
            free(buffer_out);
            free(buffer_decrypted);
            free(key);
            fclose(input_file);
            return 1;
        }

        clock_t start = clock();
        while ((bytes_read = fread(buffer_out, 1, BLOCK_SIZE, encrypted_file)) > 0) {
            size_t blocks = (bytes_read + 15) / 16;
            #pragma omp parallel for
            for (size_t i = 0; i < blocks; i++) {
                CUSTOM_AES_decrypt(buffer_out + (i * 16), buffer_decrypted + (i * 16), &custom_aesKey);
            }
            fwrite(buffer_decrypted, 1, bytes_read, decrypted_file);
        }

        clock_t end = clock();
        custom_decrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        fclose(encrypted_file);
        fclose(decrypted_file);
        printf("Arquivo descriptografado gerado: decrypted_file.txt\n");
    }

    // Comparação detalhada dos bytes (Custom AES e OpenSSL)
    printf("\n==== Comparação de Bytes (Depuração) ====\n");
    printf("Comparando arquivo original com descriptografado (Custom AES):\n");
    compare_file_bytes(argv[1], "decrypted_file.txt", 64); // Compara os primeiros 64 bytes

    // Exibir tempos de execução
    printf("\n==== Tempos de Execução ====\n");
    printf("Custom AES Encrypt: %.6f segundos\n", custom_encrypt_time);
    printf("Custom AES Decrypt: %.6f segundos\n", custom_decrypt_time);

    // Comparação final: verifica se a criptografia foi bem sucedida
    if (do_decrypt) {
        int custom_success = compare_files(argv[1], "decrypted_file.txt");

        printf("\n==== Resultados das Comparações ====\n");
        printf("Custom AES: %s\n", custom_success ? "Sucesso (arquivos idênticos)" : "Erro (arquivos diferentes)");

        if (custom_success) {
            printf("\nA criptografia foi bem-sucedida.\n");
        } else {
            printf("\nA criptografia falhou.\n");
        }
    }

    // Libera memória alocada e fecha arquivos
    free(buffer_in);
    free(buffer_out);
    free(buffer_decrypted);
    free(key);
    fclose(input_file);

    return 0;
}