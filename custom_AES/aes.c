/**
 * AES Implementation with Custom S-Box
 * =====================================
 * 
 * Este programa implementa o algoritmo AES (Advanced Encryption Standard) com
 * uma modificação: a substituição da Caixa-S (S-Box) original por outra cifra
 * de substituição. A implementação das outras funções foi baseada no código da biblioteca
 * OpenSSL (https://github.com/openssl/openssl/blob/master/crypto/aes/aes_core.c).
 *
 */

#include "aes.h"
#define BLOCK_SIZE 16

static void XtimeWord(u32 *w)
{
    u32 a, b;

    a = *w;
    b = a & 0x80808080u;
    a ^= b;
    b -= b >> 7;
    b &= 0x1B1B1B1Bu;
    b ^= a << 1;
    *w = b;
}

static void XtimeLong(u64 *w)
{
    u64 a, b;

    a = *w;
    b = a & U64(0x8080808080808080);
    a ^= b;
    b -= b >> 7;
    b &= U64(0x1B1B1B1B1B1B1B1B);
    b ^= a << 1;
    *w = b;
}

static void SubWord(u32 *w)
{
    u32 x, y, a1, a2, a3, a4, a5, a6;

    x = *w;
    y = ((x & 0xFEFEFEFEu) >> 1) | ((x & 0x01010101u) << 7);
    x &= 0xDDDDDDDDu;
    x ^= y & 0x57575757u;
    y = ((y & 0xFEFEFEFEu) >> 1) | ((y & 0x01010101u) << 7);
    x ^= y & 0x1C1C1C1Cu;
    y = ((y & 0xFEFEFEFEu) >> 1) | ((y & 0x01010101u) << 7);
    x ^= y & 0x4A4A4A4Au;
    y = ((y & 0xFEFEFEFEu) >> 1) | ((y & 0x01010101u) << 7);
    x ^= y & 0x42424242u;
    y = ((y & 0xFEFEFEFEu) >> 1) | ((y & 0x01010101u) << 7);
    x ^= y & 0x64646464u;
    y = ((y & 0xFEFEFEFEu) >> 1) | ((y & 0x01010101u) << 7);
    x ^= y & 0xE0E0E0E0u;
    a1 = x;
    a1 ^= (x & 0xF0F0F0F0u) >> 4;
    a2 = ((x & 0xCCCCCCCCu) >> 2) | ((x & 0x33333333u) << 2);
    a3 = x & a1;
    a3 ^= (a3 & 0xAAAAAAAAu) >> 1;
    a3 ^= (((x << 1) & a1) ^ ((a1 << 1) & x)) & 0xAAAAAAAAu;
    a4 = a2 & a1;
    a4 ^= (a4 & 0xAAAAAAAAu) >> 1;
    a4 ^= (((a2 << 1) & a1) ^ ((a1 << 1) & a2)) & 0xAAAAAAAAu;
    a5 = (a3 & 0xCCCCCCCCu) >> 2;
    a3 ^= ((a4 << 2) ^ a4) & 0xCCCCCCCCu;
    a4 = a5 & 0x22222222u;
    a4 |= a4 >> 1;
    a4 ^= (a5 << 1) & 0x22222222u;
    a3 ^= a4;
    a5 = a3 & 0xA0A0A0A0u;
    a5 |= a5 >> 1;
    a5 ^= (a3 << 1) & 0xA0A0A0A0u;
    a4 = a5 & 0xC0C0C0C0u;
    a6 = a4 >> 2;
    a4 ^= (a5 << 2) & 0xC0C0C0C0u;
    a5 = a6 & 0x20202020u;
    a5 |= a5 >> 1;
    a5 ^= (a6 << 1) & 0x20202020u;
    a4 |= a5;
    a3 ^= a4 >> 4;
    a3 &= 0x0F0F0F0Fu;
    a2 = a3;
    a2 ^= (a3 & 0x0C0C0C0Cu) >> 2;
    a4 = a3 & a2;
    a4 ^= (a4 & 0x0A0A0A0A0Au) >> 1;
    a4 ^= (((a3 << 1) & a2) ^ ((a2 << 1) & a3)) & 0x0A0A0A0Au;
    a5 = a4 & 0x08080808u;
    a5 |= a5 >> 1;
    a5 ^= (a4 << 1) & 0x08080808u;
    a4 ^= a5 >> 2;
    a4 &= 0x03030303u;
    a4 ^= (a4 & 0x02020202u) >> 1;
    a4 |= a4 << 2;
    a3 = a2 & a4;
    a3 ^= (a3 & 0x0A0A0A0Au) >> 1;
    a3 ^= (((a2 << 1) & a4) ^ ((a4 << 1) & a2)) & 0x0A0A0A0Au;
    a3 |= a3 << 4;
    a2 = ((a1 & 0xCCCCCCCCu) >> 2) | ((a1 & 0x33333333u) << 2);
    x = a1 & a3;
    x ^= (x & 0xAAAAAAAAu) >> 1;
    x ^= (((a1 << 1) & a3) ^ ((a3 << 1) & a1)) & 0xAAAAAAAAu;
    a4 = a2 & a3;
    a4 ^= (a4 & 0xAAAAAAAAu) >> 1;
    a4 ^= (((a2 << 1) & a3) ^ ((a3 << 1) & a2)) & 0xAAAAAAAAu;
    a5 = (x & 0xCCCCCCCCu) >> 2;
    x ^= ((a4 << 2) ^ a4) & 0xCCCCCCCCu;
    a4 = a5 & 0x22222222u;
    a4 |= a4 >> 1;
    a4 ^= (a5 << 1) & 0x22222222u;
    x ^= a4;
    y = ((x & 0xFEFEFEFEu) >> 1) | ((x & 0x01010101u) << 7);
    x &= 0x39393939u;
    x ^= y & 0x3F3F3F3Fu;
    y = ((y & 0xFCFCFCFCu) >> 2) | ((y & 0x03030303u) << 6);
    x ^= y & 0x97979797u;
    y = ((y & 0xFEFEFEFEu) >> 1) | ((y & 0x01010101u) << 7);
    x ^= y & 0x9B9B9B9Bu;
    y = ((y & 0xFEFEFEFEu) >> 1) | ((y & 0x01010101u) << 7);
    x ^= y & 0x3C3C3C3Cu;
    y = ((y & 0xFEFEFEFEu) >> 1) | ((y & 0x01010101u) << 7);
    x ^= y & 0xDDDDDDDDu;
    y = ((y & 0xFEFEFEFEu) >> 1) | ((y & 0x01010101u) << 7);
    x ^= y & 0x72727272u;
    x ^= 0x63636363u;
    *w = x;
}


/* ------------------------------------------------------------- */
// Funcoes alteradas para substituir a Caixa-S 

static unsigned char PRNG(u64 seed, int offset) {
    seed ^= seed >> (offset + 3);
    seed *= 6364136223846793005ULL; // Número primo para melhor dispersão
    return (unsigned char)(seed & 0xFF);
}


static void SubLong(u64 *w, const CUSTOM_AES_KEY *key) {
    u64 random = PRNG(key->seed, 0);  // Gera o valor pseudoaleatório uma vez usando a seed derivada da chave
    *w ^= random;                    // XOR diretamente em `u64`
    *w = (*w + random) & 0xFFFFFFFFFFFFFFFF;  // Adição modular em 64 bits
}

static void InvSubLong(u64 *w, const CUSTOM_AES_KEY *key) {
    u64 random = PRNG(key->seed, 0);  // Gera um valor pseudoaleatório uma vez usando a chave derivada da chave
    *w = (*w - random) & 0xFFFFFFFFFFFFFFFF;  // Subtração modular em 64 bits
    *w ^= random; // Reverte XOR
}

/* --------------------------------------------------------------- */


static void ShiftRows(u64 *state)
{
    unsigned char s[4];
    unsigned char *s0;
    int r;

    s0 = (unsigned char *)state;
    for (r = 0; r < 4; r++) {
        s[0] = s0[0*4 + r];
        s[1] = s0[1*4 + r];
        s[2] = s0[2*4 + r];
        s[3] = s0[3*4 + r];
        s0[0*4 + r] = s[(r+0) % 4];
        s0[1*4 + r] = s[(r+1) % 4];
        s0[2*4 + r] = s[(r+2) % 4];
        s0[3*4 + r] = s[(r+3) % 4];
    }
}

static void InvShiftRows(u64 *state)
{
    unsigned char s[4];
    unsigned char *s0;
    int r;

    s0 = (unsigned char *)state;
    for (r = 0; r < 4; r++) {
        s[0] = s0[0*4 + r];
        s[1] = s0[1*4 + r];
        s[2] = s0[2*4 + r];
        s[3] = s0[3*4 + r];
        s0[0*4 + r] = s[(4-r) % 4];
        s0[1*4 + r] = s[(5-r) % 4];
        s0[2*4 + r] = s[(6-r) % 4];
        s0[3*4 + r] = s[(7-r) % 4];
    }
}

static void MixColumns(u64 *state)
{
    uni s1;
    uni s;
    int c;

    for (c = 0; c < 2; c++) {
        s1.d = state[c];
        s.d = s1.d;
        s.d ^= ((s.d & U64(0xFFFF0000FFFF0000)) >> 16)
               | ((s.d & U64(0x0000FFFF0000FFFF)) << 16);
        s.d ^= ((s.d & U64(0xFF00FF00FF00FF00)) >> 8)
               | ((s.d & U64(0x00FF00FF00FF00FF)) << 8);
        s.d ^= s1.d;
        XtimeLong(&s1.d);
        s.d ^= s1.d;
        s.b[0] ^= s1.b[1];
        s.b[1] ^= s1.b[2];
        s.b[2] ^= s1.b[3];
        s.b[3] ^= s1.b[0];
        s.b[4] ^= s1.b[5];
        s.b[5] ^= s1.b[6];
        s.b[6] ^= s1.b[7];
        s.b[7] ^= s1.b[4];
        state[c] = s.d;
    }
}

static void InvMixColumns(u64 *state)
{
    uni s1;
    uni s;
    int c;

    for (c = 0; c < 2; c++) {
        s1.d = state[c];
        s.d = s1.d;
        s.d ^= ((s.d & U64(0xFFFF0000FFFF0000)) >> 16)
               | ((s.d & U64(0x0000FFFF0000FFFF)) << 16);
        s.d ^= ((s.d & U64(0xFF00FF00FF00FF00)) >> 8)
               | ((s.d & U64(0x00FF00FF00FF00FF)) << 8);
        s.d ^= s1.d;
        XtimeLong(&s1.d);
        s.d ^= s1.d;
        s.b[0] ^= s1.b[1];
        s.b[1] ^= s1.b[2];
        s.b[2] ^= s1.b[3];
        s.b[3] ^= s1.b[0];
        s.b[4] ^= s1.b[5];
        s.b[5] ^= s1.b[6];
        s.b[6] ^= s1.b[7];
        s.b[7] ^= s1.b[4];
        XtimeLong(&s1.d);
        s1.d ^= ((s1.d & U64(0xFFFF0000FFFF0000)) >> 16)
                | ((s1.d & U64(0x0000FFFF0000FFFF)) << 16);
        s.d ^= s1.d;
        XtimeLong(&s1.d);
        s1.d ^= ((s1.d & U64(0xFF00FF00FF00FF00)) >> 8)
                | ((s1.d & U64(0x00FF00FF00FF00FF)) << 8);
        s.d ^= s1.d;
        state[c] = s.d;
    }
}

static void AddRoundKey(u64 *state, const u64 *w)
{
    state[0] ^= w[0];
    state[1] ^= w[1];
}

// Funcao utilizada para cifrar, retorna o tempo das operacoes
static OperationTimes Cipher(const unsigned char *in, unsigned char *out, const CUSTOM_AES_KEY *key) {
    u64 state[2];
    int i;
    clock_t start, end;
    double cpu_time_used;
    OperationTimes times = {0};

    memcpy(state, in, 16);

    // Medir tempo de AddRoundKey inicial
    start = clock();
    AddRoundKey(state, key->rd_key);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    times.add_round_key += cpu_time_used;

    for (i = 1; i < key->rounds; i++) {
        // Medir tempo de SubLong para state[0]
        start = clock();
        SubLong(&state[0], key);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        times.sub_long_0 += cpu_time_used;

        // Medir tempo de SubLong para state[1]
        start = clock();
        SubLong(&state[1], key);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        times.sub_long_1 += cpu_time_used;

        // Medir tempo de ShiftRows
        start = clock();
        ShiftRows(state);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        times.shift_rows += cpu_time_used;

        // Medir tempo de MixColumns
        start = clock();
        MixColumns(state);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        times.mix_columns += cpu_time_used;

        // Medir tempo de AddRoundKey
        start = clock();
        AddRoundKey(state, key->rd_key + i * 2);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        times.add_round_key += cpu_time_used;
    }

    // Medir tempo de SubLong para state[0] final
    start = clock();
    SubLong(&state[0], key);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    times.sub_long_0 += cpu_time_used;

    // Medir tempo de SubLong para state[1] final
    start = clock();
    SubLong(&state[1], key);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    times.sub_long_1 += cpu_time_used;

    // Medir tempo de ShiftRows final
    start = clock();
    ShiftRows(state);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    times.shift_rows += cpu_time_used;

    // Medir tempo de AddRoundKey final
    start = clock();
    AddRoundKey(state, key->rd_key + key->rounds * 2);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    times.add_round_key += cpu_time_used;

    memcpy(out, state, 16);
    return times;
}

// Funcao utilizada para decifrar, retorna o tempo das operacoes
static OperationTimes InvCipher(const unsigned char *in, unsigned char *out, const CUSTOM_AES_KEY *key) {
    u64 state[2];
    int i;
    clock_t start, end;
    double cpu_time_used;
    OperationTimes times = {0};

    memcpy(state, in, 16);

    // Medir tempo de AddRoundKey inicial
    start = clock();
    AddRoundKey(state, key->rd_key + key->rounds * 2);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    times.add_round_key += cpu_time_used;

    for (i = key->rounds - 1; i > 0; i--) {
        // Medir tempo de InvShiftRows
        start = clock();
        InvShiftRows(state);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        times.shift_rows += cpu_time_used;

        // Medir tempo de InvSubLong para state[0]
        start = clock();
        InvSubLong(&state[0], key);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        times.sub_long_0 += cpu_time_used;

        // Medir tempo de InvSubLong para state[1]
        start = clock();
        InvSubLong(&state[1], key);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        times.sub_long_1 += cpu_time_used;

        // Medir tempo de AddRoundKey
        start = clock();
        AddRoundKey(state, key->rd_key + i * 2);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        times.add_round_key += cpu_time_used;

        // Medir tempo de InvMixColumns
        start = clock();
        InvMixColumns(state);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        times.mix_columns += cpu_time_used;
    }

    // Medir tempo de InvShiftRows final
    start = clock();
    InvShiftRows(state);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    times.shift_rows += cpu_time_used;

    // Medir tempo de InvSubLong para state[0] final
    start = clock();
    InvSubLong(&state[0], key);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    times.sub_long_0 += cpu_time_used;

    // Medir tempo de InvSubLong para state[1] final
    start = clock();
    InvSubLong(&state[1], key);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    times.sub_long_1 += cpu_time_used;

    // Medir tempo de AddRoundKey final
    start = clock();
    AddRoundKey(state, key->rd_key);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    times.add_round_key += cpu_time_used;

    memcpy(out, state, 16);
    return times;
}


static void RotWord(u32 *x)
{
    unsigned char *w0;
    unsigned char tmp;

    w0 = (unsigned char *)x;
    tmp = w0[0];
    w0[0] = w0[1];
    w0[1] = w0[2];
    w0[2] = w0[3];
    w0[3] = tmp;
}

static void KeyExpansion(const unsigned char *key, u64 *w,
                         int nr, int nk)
{
    u32 rcon;
    uni prev;
    u32 temp;
    int i, n;

    memcpy(w, key, nk*4);
    memcpy(&rcon, "\1\0\0\0", 4);
    n = nk/2;
    prev.d = w[n-1];
    for (i = n; i < (nr+1)*2; i++) {
        temp = prev.w[1];
        if (i % n == 0) {
            RotWord(&temp);
            SubWord(&temp);
            temp ^= rcon;
            XtimeWord(&rcon);
        } else if (nk > 6 && i % n == 2) {
            SubWord(&temp);
        }
        prev.d = w[i-n];
        prev.w[0] ^= temp;
        prev.w[1] ^= prev.w[0];
        w[i] = prev.d;
    }
}


// Função para derivar uma seed a partir da chave do usuário
u64 derive_seed(const unsigned char *userKey, size_t key_len) {
    u64 seed = 0;
    // Combina os bytes da chave para formar a seed
    for (size_t i = 0; i < key_len; i++) {
        seed = (seed << 8) | userKey[i];  // Desloca a seed 8 bits à esquerda e adiciona o byte atual da chave
    }
    return seed;
}

int CUSTOM_AES_set_encrypt_key(const unsigned char *userKey, const int bits, CUSTOM_AES_KEY *key) {
    // Verifica se a chave do usuário ou a estrutura da chave são nulas
    if (!userKey || !key)
        return -1;  

    // Verifica se o tamanho da chave é válido (128, 192 ou 256 bits)
    if (bits != 128 && bits != 192 && bits != 256)
        return -2;  

    // Define o número de rodadas com base no tamanho da chave
    key->rounds = (bits == 128) ? 10 : (bits == 192) ? 12 : 14;

    // Expande a chave para as rodadas de criptografia
    KeyExpansion(userKey, key->rd_key, key->rounds, bits / 32);

    // Deriva a seed a partir da chave do usuário
    key->seed = derive_seed(userKey, bits / 8);  // bits / 8 = comprimento da chave em bytes

    return 0;  
}


int CUSTOM_AES_set_decrypt_key(const unsigned char *userKey, const int bits, CUSTOM_AES_KEY *key) {
    return CUSTOM_AES_set_encrypt_key(userKey, bits, key);
}

OperationTimes CUSTOM_AES_encrypt(const unsigned char *in, unsigned char *out, const CUSTOM_AES_KEY *key) {
   return Cipher(in, out, key);
}

OperationTimes CUSTOM_AES_decrypt(const unsigned char *in, unsigned char *out, const CUSTOM_AES_KEY *key) {
    return InvCipher(in, out, key);
}



// Criptografa um arquivo de entrada usando a openssl
void openssl_encrypt_file(FILE *input_file, FILE *encrypted_file, unsigned char* key) {
    unsigned char buffer_in[BLOCK_SIZE];
    unsigned char buffer_out[2*BLOCK_SIZE];	//espaço para o padding
    size_t bytes_read;
    EVP_CIPHER_CTX *ctx;
    int len = 0;

    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);

    ctx = EVP_CIPHER_CTX_new();

    EVP_EncryptInit(ctx, EVP_aes_128_ecb(), key, NULL);

    while ((bytes_read = fread(buffer_in, 1, BLOCK_SIZE, input_file)) > 0) {
        EVP_EncryptUpdate(ctx, buffer_out, &len, buffer_in, bytes_read);
        fwrite(buffer_out, sizeof(unsigned char), len, encrypted_file);
    }

    EVP_EncryptFinal_ex(ctx, buffer_out, &len);
    fwrite(buffer_out, sizeof(unsigned char), len, encrypted_file);

    // Libera o contexto de cifra
    EVP_CIPHER_CTX_free(ctx);

    EVP_cleanup(); 
    CRYPTO_cleanup_all_ex_data(); 
    ERR_free_strings();
    return;
}

// Descriptografa um arquivo de entrada usando a openssl
void openssl_decrypt_file(FILE *encrypted_file, FILE *output_file, unsigned char* key) {
    unsigned char buffer_in[BLOCK_SIZE];
    unsigned char buffer_out[2*BLOCK_SIZE];	//espaço para o padding
    size_t bytes_read;
    EVP_CIPHER_CTX *ctx;
    int len = 0;

    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);

    ctx = EVP_CIPHER_CTX_new();

    EVP_DecryptInit(ctx, EVP_aes_128_ecb(), key, NULL);

    while ((bytes_read = fread(buffer_in, 1, BLOCK_SIZE, encrypted_file)) > 0) {
        EVP_DecryptUpdate(ctx, buffer_out, &len, buffer_in, bytes_read);
        fwrite(buffer_out, sizeof(unsigned char), len, output_file);
    }

    EVP_DecryptFinal_ex(ctx, buffer_out, &len);
    fwrite(buffer_out, sizeof(unsigned char), len, output_file);

     // Libera o contexto de cifra
    EVP_CIPHER_CTX_free(ctx);

    EVP_cleanup(); 
    CRYPTO_cleanup_all_ex_data(); 
    ERR_free_strings();
    return;
}

// Criptografa um arquivo de entrada usando o método customizado de 16 em 16 bytes
// adicionando um padding no final do arquivo
void encrypt_file(FILE *input_file, FILE *encrypted_file, const void *custom_aesKey) {
    unsigned char buffer_in[BLOCK_SIZE];
    unsigned char buffer_out[BLOCK_SIZE];
    size_t bytes_read;
    size_t resto = 0;
    OperationTimes total_times = {0};

    while ((bytes_read = fread(buffer_in, 1, BLOCK_SIZE, input_file)) > 0) {
	// Verifica se é o último bloco e adiciona o padding
        if (bytes_read < BLOCK_SIZE) {
            resto = BLOCK_SIZE - bytes_read;
            memset(buffer_in+bytes_read, 0, resto);
        }
        
        // Criptografa o bloco e obtém os tempos das operações
        OperationTimes block_times = CUSTOM_AES_encrypt(buffer_in, buffer_out, custom_aesKey);
        
        // Acumula os tempos das operações para todos os blocos
        total_times.sub_long_0 += block_times.sub_long_0;
        total_times.sub_long_1 += block_times.sub_long_1;
        total_times.shift_rows += block_times.shift_rows;
        total_times.mix_columns += block_times.mix_columns;
        total_times.add_round_key += block_times.add_round_key;

        fwrite(buffer_out, 1, BLOCK_SIZE, encrypted_file);
    }

    // Cria um bloco a mais com o tamanho do padding
    memset(buffer_out, resto, BLOCK_SIZE);
    fwrite(buffer_out, 1, BLOCK_SIZE, encrypted_file);

    printf("\n==== Tempo Total das operações para criptografar ====\n");
    printf("AddRoundKey: %f seconds\n", total_times.add_round_key);
    printf("SubLong[0]: %f seconds\n", total_times.sub_long_0);
    printf("SubLong[1]: %f seconds\n", total_times.sub_long_1);
    printf("ShiftRows: %f seconds\n", total_times.shift_rows);
    printf("MixColumns: %f seconds\n", total_times.mix_columns);
    printf("======================================================\n");
    printf("\n");

    return;
}

// Descriptografa um arquivo de entrada usando o método customizado de 16 em 16 bytes
// removendo o padding no final do arquivo
void decrypt_file(FILE *encrypted_file, FILE *output_file, const void *custom_aesKey) {
    unsigned char buffer_in[BLOCK_SIZE];
    unsigned char buffer_out[BLOCK_SIZE];
    size_t bytes_read;
    OperationTimes total_times = {0};

    while ((bytes_read = fread(buffer_in, 1, BLOCK_SIZE, encrypted_file)) > 0) {
        
        // Descriptografa o bloco e obtém os tempos das operações
        OperationTimes block_times = CUSTOM_AES_decrypt(buffer_in, buffer_out, custom_aesKey);

        // Acumula os tempos das operações para todos os blocos
        total_times.sub_long_0 += block_times.sub_long_0;
        total_times.sub_long_1 += block_times.sub_long_1;
        total_times.shift_rows += block_times.shift_rows;
        total_times.mix_columns += block_times.mix_columns;
        total_times.add_round_key += block_times.add_round_key;

        fwrite(buffer_out, 1, BLOCK_SIZE, output_file);
    }

    int padding = (int)(buffer_in[0]);
    fseek(output_file, -(BLOCK_SIZE+padding), SEEK_END);
    ftruncate(fileno(output_file), ftell(output_file));

    printf("\n==== Tempo Total das operações para descriptografar ====\n");
    printf("AddRoundKey: %f seconds\n", total_times.add_round_key);
    printf("SubLong[0]: %f seconds\n", total_times.sub_long_0);
    printf("SubLong[1]: %f seconds\n", total_times.sub_long_1);
    printf("ShiftRows: %f seconds\n", total_times.shift_rows);
    printf("MixColumns: %f seconds\n", total_times.mix_columns);
    printf("==========================================================\n");
    printf("\n");

    return;
}