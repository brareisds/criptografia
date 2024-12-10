#ifndef AES_H_CUSTOM
#define AES_H_CUSTOM

#include <stdint.h>  // Para os tipos u32, u64
#include <string.h>  // Para memcpy
#include <assert.h>  // Para assert
#include <stddef.h>  // Para size_t, se necessário
#include <stdio.h>
#include <stdlib.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

// Macro para manipulação de valores de 64 bits
#define U64(x) ((u64)(x))
typedef unsigned long long u64;
typedef unsigned int u32;

// Union para manipulação de dados
typedef union {
    u64 d;
    uint8_t b[8];
    u32 w[2];
} uni;

// Estrutura para armazenar a chave expandida e o número de rodadas
typedef struct {
    u64 rd_key[30];
    int rounds;
    u64 seed;  // Seed do PRNG
} CUSTOM_AES_KEY;

int CUSTOM_AES_set_encrypt_key(const unsigned char *userKey, const int bits, CUSTOM_AES_KEY *key);
int CUSTOM_AES_set_decrypt_key(const unsigned char *userKey, const int bits, CUSTOM_AES_KEY *key);

void CUSTOM_AES_encrypt(const unsigned char *in, unsigned char *out, const CUSTOM_AES_KEY *key);
void CUSTOM_AES_decrypt(const unsigned char *in, unsigned char *out, const CUSTOM_AES_KEY *key);

void openssl_encrypt(const unsigned char *input, unsigned char *output, const unsigned char *key, FILE *output_file);
void openssl_decrypt(const unsigned char *input, unsigned char *output, const unsigned char *key, FILE *output_file);

void openssl_encrypt_file(FILE *input_file, FILE *encrypted_file, unsigned char* key);
void openssl_decrypt_file(FILE *encrypted_file, FILE *output_file, unsigned char* key);

#endif 
