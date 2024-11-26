#ifndef AES_H
#define AES_H

#include <stdint.h>  // Para os tipos u32, u64
#include <string.h>  // Para memcpy
#include <assert.h>  // Para assert
#include <stddef.h>  // Para size_t, se necessário
#include <stdio.h>


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

// Funções principais do AES
/*int AES_set_encrypt_key(const unsigned char *userKey, const int bits, CUSTOM_AES_KEY *key);
int AES_set_decrypt_key(const unsigned char *userKey, const int bits, CUSTOM_AES_KEY *key);

void AES_encrypt(const unsigned char *in, unsigned char *out, const CUSTOM_AES_KEY *key);
void AES_decrypt(const unsigned char *in, unsigned char *out, const CUSTOM_AES_KEY *key);
*/
// Funções auxiliares (para uso interno)
static void XtimeWord(u32 *w);
static void XtimeLong(u64 *w);
static void SubWord(u32 *w);
// static void SubLong(u64 *w);
// static void InvSubLong(u64 *w);

static void SubLong(u64 *w, const CUSTOM_AES_KEY *key);
static void InvSubLong(u64 *w, const CUSTOM_AES_KEY *key);

static void ShiftRows(u64 *state);
static void InvShiftRows(u64 *state);
static void MixColumns(u64 *state);
static void InvMixColumns(u64 *state);
static void AddRoundKey(u64 *state, const u64 *w);


// static void Cipher(const unsigned char *in, unsigned char *out, const u64 *w, int nr);
// static void InvCipher(const unsigned char *in, unsigned char *out, const u64 *w, int nr);

static void Cipher(const unsigned char *in, unsigned char *out, const CUSTOM_AES_KEY *key);
static void InvCipher(const unsigned char *in, unsigned char *out, const CUSTOM_AES_KEY *key);


static void RotWord(u32 *x);
static void KeyExpansion(const unsigned char *key, u64 *w, int nr, int nk);

#endif // AES_H
