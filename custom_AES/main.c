
#include "aes.h"
#include <openssl/aes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <openssl/rand.h>
#include <unistd.h> // Para ftruncate

#define BLOCK_SIZE 16

/*void openssl_encrypt(const unsigned char *input, unsigned char *output, const unsigned char *key) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len;
    EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL);
    EVP_CIPHER_CTX_set_padding(ctx, 1); 

    EVP_EncryptUpdate(ctx, output, &len, input, 16);
    EVP_EncryptFinal_ex(ctx, output + len, &len);
    EVP_CIPHER_CTX_free(ctx);
}

void openssl_decrypt(const unsigned char *input, unsigned char *output, const unsigned char *key) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len;
    EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL);
    EVP_CIPHER_CTX_set_padding(ctx, 1); // Ativa o padding

    EVP_DecryptUpdate(ctx, output, &len, input, 16);
    EVP_DecryptFinal_ex(ctx, output + len, &len);
    EVP_CIPHER_CTX_free(ctx);
}*/

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

// Função para criptografar usando AES
void aes_encrypt(const unsigned char *input, unsigned char *output, const unsigned char *key) {
    AES_KEY aes_key;
    if (AES_set_encrypt_key(key, 128, &aes_key) < 0) {
        fprintf(stderr, "Erro ao configurar a chave de criptografia.\n");
        exit(1);
    }
    AES_encrypt(input, output, &aes_key);
}

// Função para descriptografar usando AES
void aes_decrypt(const unsigned char *input, unsigned char *output, const unsigned char *key) {
    AES_KEY aes_key;
    if (AES_set_decrypt_key(key, 128, &aes_key) < 0) {
        fprintf(stderr, "Erro ao configurar a chave de descriptografia.\n");
        exit(1);
    }
    AES_decrypt(input, output, &aes_key);
}


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
    if (argc < 3 || argc > 5) {
        printf("Uso: %s <arquivo_entrada> <tamanho_chave_em_bits> [operacao] [chave_hex (apenas para decrypt)]\n", argv[0]);
        printf("Operacao (opcional): encrypt, decrypt ou both (default: encrypt)\n");
        return 1;
    }

    FILE *input_file = fopen(argv[1], "rb");
    if (!input_file) {
        printf("Erro: O arquivo de entrada '%s' não pôde ser aberto.\n", argv[1]);
        return 1;
    }

    int key_bits = atoi(argv[2]);
    if (key_bits != 128 && key_bits != 192 && key_bits != 256) {
        printf("Erro: Tamanho da chave inválido. Use 128, 192 ou 256.\n");
        fclose(input_file);
        return 1;
    }

    char *operation = (argc >= 4) ? argv[3] : "encrypt";
    int do_encrypt = strcmp(operation, "encrypt") == 0 || strcmp(operation, "both") == 0;
    int do_decrypt = strcmp(operation, "decrypt") == 0 || strcmp(operation, "both") == 0;

    if (!do_encrypt && !do_decrypt) {
        printf("Erro: Operação inválida. Use 'encrypt', 'decrypt' ou 'both'.\n");
        fclose(input_file);
        return 1;
    }

    unsigned char *key = NULL;
    int key_bytes = key_bits / 8;
    if (do_decrypt && strcmp(operation, "decrypt") == 0) {
        if (argc < 5) {
            printf("Erro: A operação 'decrypt' requer a chave em formato hexadecimal como argumento.\n");
            fclose(input_file);
            return 1;
        }
        key = malloc(key_bytes);
        if (!key || strlen(argv[4]) != (size_t)(key_bytes * 2)) {
            printf("Erro: Chave inválida.\n");
            free(key);
            fclose(input_file);
            return 1;
        }
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

    CUSTOM_AES_KEY custom_aesKey;
    if (CUSTOM_AES_set_encrypt_key(key, key_bits, &custom_aesKey) != 0) {
        printf("Erro ao configurar a chave.\n");
        free(key);
        fclose(input_file);
        return 1;
    }

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
    double custom_encrypt_time = 0, custom_decrypt_time = 0, openssl_encrypt_time = 0, openssl_decrypt_time = 0;

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
        unsigned char resto;
        while ((bytes_read = fread(buffer_in, 1, BLOCK_SIZE, input_file)) > 0) {
            //size_t blocks = (bytes_read) / 16;
            
            // Verifica tamanho do bloco, precisa ser múltiplo de 16
            if (bytes_read < BLOCK_SIZE){
                // Verifica quanto falta para 16
                resto = (unsigned char)(BLOCK_SIZE - bytes_read);
                memset(buffer_in + bytes_read, 0, resto);
            }

            CUSTOM_AES_encrypt(buffer_in, buffer_out, &custom_aesKey);
            fwrite(buffer_out, 1, bytes_read, encrypted_file);
        }
        // Adiciona último bloco
        memset(buffer_out, 0, BLOCK_SIZE);
        buffer_out[0] = resto;

        clock_t end = clock();
        custom_encrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        fclose(encrypted_file);
        printf("Arquivo criptografado gerado: encrypted_file.aes\n");

        // OpenSSL Encrypt
        FILE *openssl_file = fopen("openssl_encrypted.aes", "wb");
        fseek(input_file, 0, SEEK_SET);
        start = clock();
        while ((bytes_read = fread(buffer_in, 1, BLOCK_SIZE, input_file)) > 0) {
            //size_t blocks = (bytes_read) / 16;
            //for (size_t i = 0; i < blocks; i++) {
            if (bytes_read < BLOCK_SIZE){
                resto = (unsigned char)(BLOCK_SIZE - bytes_read);
                memset(buffer_in + bytes_read, 0, resto);   
            }
            aes_encrypt(buffer_in, buffer_out, key);
           // }
            fwrite(buffer_out, 1, bytes_read, openssl_file);
        }
        // Adiciona último bloco
        memset(buffer_out, 0, BLOCK_SIZE);
        buffer_out[0] = resto;

        end = clock();
        openssl_encrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        fclose(openssl_file);
    }

    if (do_decrypt) {
        unsigned char resto;
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
            
            
            CUSTOM_AES_decrypt(buffer_out, buffer_decrypted, &custom_aesKey);
            if (feof(encrypted_file)){
                // Le se tem padding
                resto = buffer_out[0] + 16;
            }

            fwrite(buffer_decrypted, 1, bytes_read, decrypted_file);
        }

        // retira padding 
            // Obtém a posição atual do ponteiro (tamanho do arquivo)
        long file_size = ftell(decrypted_file);

        // Calcula o novo tamanho do arquivo
        long new_size = file_size - resto;

        // Ajusta o tamanho do arquivo
        if (ftruncate(fileno(decrypted_file), new_size) != 0) {
            perror("Erro ao truncar o arquivo");
        }

        clock_t end = clock();
        custom_decrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        fclose(encrypted_file);
        fclose(decrypted_file);
        printf("Arquivo descriptografado gerado: decrypted_file.txt\n");



        // OPENSSL
        FILE *openssl_encrypted = fopen("openssl_encrypted.aes", "rb");
        FILE *openssl_decrypted = fopen("openssl_decrypted.txt", "wb");
        size_t total_read = 0, total_written = 0;
        start = clock();
        while ((bytes_read = fread(buffer_out, 1, BLOCK_SIZE, openssl_encrypted)) > 0) {
            total_read += bytes_read;


                aes_decrypt(buffer_out, buffer_in, key);
            size_t bytes_to_write = bytes_read; // Valide aqui a lógica para blocos
            total_written += bytes_to_write;

            if (feof(openssl_encrypted)){
                // Le se tem padding
                resto = buffer_out[0] + 16;
            }

            fwrite(buffer_decrypted, 1, bytes_read, openssl_decrypted);
        }
        // retira padding 
        // Obtém a posição atual do ponteiro (tamanho do arquivo)
        file_size = ftell(openssl_decrypted);

        // Calcula o novo tamanho do arquivo
        new_size = file_size - resto;

        // Ajusta o tamanho do arquivo
        if (ftruncate(fileno(openssl_decrypted), new_size) != 0) {
            perror("Erro ao truncar o arquivo");
        }

        end = clock();
        openssl_decrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;

        printf("Total lido (OpenSSL): %zu bytes\n", total_read);
        printf("Total escrito (OpenSSL): %zu bytes\n", total_written);
        
        fclose(openssl_encrypted);
        fclose(openssl_decrypted);
    }

    // Comparação detalhada dos bytes (Custom AES e OpenSSL)
    printf("\n==== Comparação de Bytes (Depuração) ====\n");
    printf("Comparando arquivo original com descriptografado (Custom AES):\n");
    compare_file_bytes(argv[1], "decrypted_file.txt", 64); // Compara os primeiros 64 bytes
    printf("Comparando arquivo original com descriptografado (OpenSSL AES):\n");
    compare_file_bytes(argv[1], "openssl_decrypted.txt", 64); // Compara os primeiros 64 bytes


    // Exibir tempos de execução
    printf("\n==== Tempos de Execução ====\n");
    printf("Custom AES Encrypt: %.6f segundos\n", custom_encrypt_time);
    printf("Custom AES Decrypt: %.6f segundos\n", custom_decrypt_time);
    printf("OpenSSL Encrypt: %.6f segundos\n", openssl_encrypt_time);
    printf("OpenSSL Decrypt: %.6f segundos\n", openssl_decrypt_time);

    // Comparação final: verificar se ambas as criptografias foram bem-sucedidas
    if (do_decrypt) {
        int custom_success = compare_files(argv[1], "decrypted_file.txt");
        int openssl_success = compare_files(argv[1], "openssl_decrypted.txt");

        printf("\n==== Resultados das Comparações ====\n");
        printf("Custom AES: %s\n", custom_success ? "Sucesso (arquivos idênticos)" : "Erro (arquivos diferentes)");
        printf("OpenSSL AES: %s\n", openssl_success ? "Sucesso (arquivos idênticos)" : "Erro (arquivos diferentes)");

        if (custom_success && openssl_success) {
            printf("Ambas as criptografias foram bem-sucedidas.\n");
        } else {
            printf("Alguma das criptografias falhou.\n");
        }
    }


    free(buffer_in);
    free(buffer_out);
    free(buffer_decrypted);
    free(key);
    fclose(input_file);

    return 0;
}