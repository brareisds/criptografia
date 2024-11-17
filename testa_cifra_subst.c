/**
 * Teste de Cifra de Substituição
 * ==============================
 *
 * Este programa implementa um teste para verificar o funcionamento
 * de uma cifra de substituição personalizada antes de integrá-la 
 * ao algoritmo AES.
 *
 */


#include <stdio.h>
#include <stdint.h>

typedef uint64_t u64;


void SubLong(u64 *state) {
    for (int i = 0; i < 2; i++) {
        state[i] = (state[i] << 13) | (state[i] >> (64 - 13)); // Rotação de 13 bits
    }
}

void InvSubLong(u64 *state) {
    for (int i = 0; i < 2; i++) {
        state[i] = (state[i] >> 13) | (state[i] << (64 - 13)); // Rotação reversa
    }
}

void print_state(const char *label, u64 *state) {
    printf("%s: [0x%016lx, 0x%016lx]\n", label, state[0], state[1]);
}

int main() {

    /* Estado inicial de teste (128 bits divididos em dois blocos de 64 bits) 
    * para ser compatível com o modelo usado no AES.*/
    u64 state[2] = {0x0123456789abcdef, 0xfedcba9876543210};
    u64 original_state[2];

    // Salva estado original para comparação
    original_state[0] = state[0];
    original_state[1] = state[1];
    print_state("Original", state);

    // Aplica SubLong personalizada (cifragem)
    SubLong(state);
    print_state("After SubLong", state);

    // Aplica InvSubLong personalizada (decifragem)
    InvSubLong(state);
    print_state("After InvSubLong", state);

    // Verifica se o estado foi restaurado
    if (state[0] == original_state[0] && state[1] == original_state[1]) {
        printf("Teste bem-sucedido: o estado original foi restaurado!\n");
    } else {
        printf("Erro: o estado original NÃO foi restaurado.\n");
    }

    return 0;
}
