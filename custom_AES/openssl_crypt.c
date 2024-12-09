#include <openssl/aes.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BLOCK_SIZE 16

// Função para aplicar padding PKCS#7
int apply_pkcs7_padding(unsigned char *buffer, int data_len) {
    int padding_size = BLOCK_SIZE - (data_len % BLOCK_SIZE);
    for (int i = data_len; i < data_len + padding_size; i++) {
        buffer[i] = padding_size;
    }
    return data_len + padding_size;
}

// Função para remover padding PKCS#7
int remove_pkcs7_padding(unsigned char *buffer, int data_len) {
    int padding_size = buffer[data_len - 1];
    if (padding_size < 1 || padding_size > BLOCK_SIZE) {
        fprintf(stderr, "Erro no padding!\n");
        exit(1);
    }
    return data_len - padding_size;
}

// Função para criptografar arquivo
void encrypt_file(const char *input_filename, const char *output_filename, const unsigned char *key) {
    FILE *input_file = fopen(input_filename, "rb");
    FILE *output_file = fopen(output_filename, "wb");
    if (!input_file || !output_file) {
        perror("Erro ao abrir arquivos");
        exit(EXIT_FAILURE);
    }

    AES_KEY aes_key;
    AES_set_encrypt_key(key, 128, &aes_key);

    unsigned char buffer_in[BLOCK_SIZE];
    unsigned char buffer_out[BLOCK_SIZE];
    int bytes_read;

    while ((bytes_read = fread(buffer_in, 1, BLOCK_SIZE, input_file)) > 0) {
        if (bytes_read < BLOCK_SIZE) {
            int padded_len = apply_pkcs7_padding(buffer_in, bytes_read);
            AES_encrypt(buffer_in, buffer_out, &aes_key);
            fwrite(buffer_out, 1, BLOCK_SIZE, output_file);
            break;
        }
        AES_encrypt(buffer_in, buffer_out, &aes_key);
        fwrite(buffer_out, 1, BLOCK_SIZE, output_file);
    }

    fclose(input_file);
    fclose(output_file);
}

// Função para descriptografar arquivo
void decrypt_file(const char *input_filename, const char *output_filename, const unsigned char *key) {
    FILE *input_file = fopen(input_filename, "rb");
    FILE *output_file = fopen(output_filename, "wb");
    if (!input_file || !output_file) {
        perror("Erro ao abrir arquivos");
        exit(EXIT_FAILURE);
    }

    AES_KEY aes_key;
    AES_set_decrypt_key(key, 128, &aes_key);

    unsigned char buffer_in[BLOCK_SIZE];
    unsigned char buffer_out[BLOCK_SIZE];
    int bytes_read;

    while ((bytes_read = fread(buffer_in, 1, BLOCK_SIZE, input_file)) > 0) {
        AES_decrypt(buffer_in, buffer_out, &aes_key);

        // Verifica se é o último bloco e remove padding
        if (feof(input_file)) {
            int final_len = remove_pkcs7_padding(buffer_out, bytes_read);
            fwrite(buffer_out, 1, final_len, output_file);
        } else {
            fwrite(buffer_out, 1, BLOCK_SIZE, output_file);
        }
    }

    fclose(input_file);
    fclose(output_file);
}

// Função para comparar dois arquivos
int compare_files(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");
    if (!f1 || !f2) {
        perror("Erro ao abrir arquivos para comparação");
        return 0;
    }

    int result = 1;
    unsigned char buffer1[BLOCK_SIZE], buffer2[BLOCK_SIZE];
    size_t bytes1, bytes2;

    while ((bytes1 = fread(buffer1, 1, BLOCK_SIZE, f1)) > 0 &&
           (bytes2 = fread(buffer2, 1, BLOCK_SIZE, f2)) > 0) {
        if (bytes1 != bytes2 || memcmp(buffer1, buffer2, bytes1) != 0) {
            result = 0;
            break;
        }
    }

    fclose(f1);
    fclose(f2);
    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <arquivo_entrada> <arquivo_saida> <encrypt/decrypt>\n", argv[0]);
        return 1;
    }

    unsigned char key[BLOCK_SIZE];
    if (!RAND_bytes(key, sizeof(key))) {
        fprintf(stderr, "Erro ao gerar chave.\n");
        return 1;
    }

    printf("Chave gerada: ");
    for (int i = 0; i < BLOCK_SIZE; i++) {
        printf("%02x", key[i]);
    }
    printf("\n");

    if (strcmp(argv[3], "encrypt") == 0) {
        encrypt_file(argv[1], "encrypted_file.aes", key);
        printf("Arquivo criptografado com sucesso.\n");
    } else if (strcmp(argv[3], "decrypt") == 0) {
        decrypt_file("encrypted_file.aes", argv[2], key);
        printf("Arquivo descriptografado com sucesso.\n");

        // Comparação entre o arquivo original e o descriptografado
        if (compare_files(argv[1], argv[2])) {
            printf("Sucesso: Arquivo original e descriptografado são idênticos.\n");
        } else {
            printf("Erro: Arquivo descriptografado difere do original.\n");
        }
    } else {
        fprintf(stderr, "Operação inválida: use 'encrypt' ou 'decrypt'.\n");
        return 1;
    }

    return 0;
}
