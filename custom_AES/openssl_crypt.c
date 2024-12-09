#include <openssl/aes.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define AES_BLOCK_SIZE 16

void handleErrors(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void generate_random_bytes(unsigned char *buffer, size_t size) {
    if (!RAND_bytes(buffer, size)) {
        handleErrors("Erro ao gerar bytes aleatórios");
    }
}

void aes_encrypt_file(const char *input_filename, const char *output_filename, unsigned char *key, unsigned char *iv, int key_size) {
    FILE *input_file = fopen(input_filename, "rb");
    FILE *output_file = fopen(output_filename, "wb");

    if (!input_file || !output_file) {
        handleErrors("Erro ao abrir arquivos");
    }

    AES_KEY encrypt_key;
    AES_set_encrypt_key(key, key_size, &encrypt_key);

    unsigned char input_buffer[AES_BLOCK_SIZE];
    unsigned char output_buffer[AES_BLOCK_SIZE];
    int bytes_read;

    while ((bytes_read = fread(input_buffer, 1, AES_BLOCK_SIZE, input_file)) > 0) {
        if (bytes_read < AES_BLOCK_SIZE) {
            memset(input_buffer + bytes_read, AES_BLOCK_SIZE - bytes_read, AES_BLOCK_SIZE - bytes_read);
        }
        AES_cbc_encrypt(input_buffer, output_buffer, AES_BLOCK_SIZE, &encrypt_key, iv, AES_ENCRYPT);
        fwrite(output_buffer, 1, AES_BLOCK_SIZE, output_file);
    }

    fclose(input_file);
    fclose(output_file);
}

void aes_decrypt_file(const char *input_filename, const char *output_filename, unsigned char *key, unsigned char *iv, int key_size) {
    FILE *input_file = fopen(input_filename, "rb");
    FILE *output_file = fopen(output_filename, "wb");

    if (!input_file || !output_file) {
        handleErrors("Erro ao abrir arquivos");
    }

    AES_KEY decrypt_key;
    AES_set_decrypt_key(key, key_size, &decrypt_key);

    unsigned char input_buffer[AES_BLOCK_SIZE];
    unsigned char output_buffer[AES_BLOCK_SIZE];
    int bytes_read;

    while ((bytes_read = fread(input_buffer, 1, AES_BLOCK_SIZE, input_file)) > 0) {
        AES_cbc_encrypt(input_buffer, output_buffer, AES_BLOCK_SIZE, &decrypt_key, iv, AES_DECRYPT);
        if (bytes_read < AES_BLOCK_SIZE) {
            int padding_length = output_buffer[AES_BLOCK_SIZE - 1];
            fwrite(output_buffer, 1, AES_BLOCK_SIZE - padding_length, output_file);
        } else {
            fwrite(output_buffer, 1, AES_BLOCK_SIZE, output_file);
        }
    }

    fclose(input_file);
    fclose(output_file);
}

int compare_files(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");

    if (!f1 || !f2) {
        handleErrors("Erro ao abrir arquivos para comparação");
    }

    int result = 1; // Assume files are identical
    unsigned char buffer1[AES_BLOCK_SIZE];
    unsigned char buffer2[AES_BLOCK_SIZE];
    size_t bytes_read1, bytes_read2;

    while ((bytes_read1 = fread(buffer1, 1, AES_BLOCK_SIZE, f1)) > 0 &&
           (bytes_read2 = fread(buffer2, 1, AES_BLOCK_SIZE, f2)) > 0) {
        if (bytes_read1 != bytes_read2 || memcmp(buffer1, buffer2, bytes_read1) != 0) {
            result = 0; // Files are different
            break;
        }
    }

    fclose(f1);
    fclose(f2);
    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_entrada> <tamanho_chave>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_file = argv[1];
    const char *openssl_encrypted_file = "openssl_encrypted.aes";
    const char *openssl_decrypted_file = "openssl_decrypted.txt";
    int key_size = atoi(argv[2]);

    if (key_size != 128 && key_size != 192 && key_size != 256) {
        fprintf(stderr, "Tamanho da chave deve ser 128, 192 ou 256 bits\n");
        return EXIT_FAILURE;
    }

    unsigned char key[key_size / 8];
    unsigned char iv[AES_BLOCK_SIZE];

    // Gerar chave e IV aleatórios
    generate_random_bytes(key, sizeof(key));
    generate_random_bytes(iv, sizeof(iv));

    // Criptografar arquivo
    clock_t start = clock();
    aes_encrypt_file(input_file, openssl_encrypted_file, key, iv, key_size);
    clock_t end = clock();
    double openssl_encrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Tempo de criptografia OpenSSL: %f segundos\n", openssl_encrypt_time);

    // Descriptografar arquivo
    start = clock();
    aes_decrypt_file(openssl_encrypted_file, openssl_decrypted_file, key, iv, key_size);
    end = clock();
    double openssl_decrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Tempo de descriptografia OpenSSL: %f segundos\n", openssl_decrypt_time);

    // Comparar arquivos
    if (compare_files(input_file, openssl_decrypted_file)) {
        printf("Os arquivos são idênticos.\n");
    } else {
        printf("Os arquivos são diferentes.\n");
    }

    return 0;
}