### README: AES Modificado com PRNG e Seed Personalizada

## Como Compilar e Executar

### Compilação

Use o **Makefile** incluso para compilar o projeto:

```bash
make
```

### Execução

1. **Gerar Arquivos de Teste:**

   - Antes de testar a criptografia e descriptografia, é necessário gerar os arquivos de teste com diferentes tamanhos. Para isso, execute o script Python incluso:
     ```bash
     python3 testes.py
     ```
   - Esse script cria os arquivos de teste dentro da pasta `test_files` e gera um gráfico.

2. **Executar a Criptografia e Descriptografia Individualmente:**

   - Após gerar os arquivos de teste, é possível executar a criptografia e descriptografia em qualquer arquivo individualmente:

     ```bash
     ./aes <caminho_do_arquivo_de_teste> <chave_de_teste>
     ```

   - Exemplo:
     ```bash
     ./aes test_files/test_1000KB.txt mytestkey1234567
     ```

3. **Sobre os Arquivos de Teste:**
   - A pasta `test_files` conterá arquivos de tamanhos variados para diferentes testes de desempenho e validação.

### Testes de Desempenho Comparativo

Execute novamente o script **Python** para testar o desempenho da implementação customizada em comparação com a versão do OpenSSL:

```bash
python3 testes.py
```
---

## Descrição do Projeto

Este projeto implementa uma variante do algoritmo **AES (Advanced Encryption Standard)**, com as seguintes modificações principais:

1. **Geração de Números Pseudoaleatórios (PRNG):**

   - Baseado em uma **seed** derivada da chave do usuário, utilizada para transformar os blocos de dados.

2. **Transformação Personalizada no Lugar da S-Box:**
   - Substituição da tradicional substituição de bytes (S-Box) do AES por operações baseadas em valores pseudoaleatórios (XOR e adição modular).

Essas alterações são projetadas para explorar novos conceitos de criptografia e avaliar os impactos em segurança e desempenho.

---

### Principais Diferenças em Relação ao AES Tradicional

#### 1. **Substituição da S-Box**

- **No AES Original:**

  - A substituição de bytes utiliza uma **S-Box** que aplica inversões no campo finito \(GF(2^8)\) e uma transformação afim.
  - Essa operação garante alta difusão e não-linearidade.

- **No AES Modificado:**
  - Substituímos a S-Box por uma função **SubLong**, que utiliza:
    - **XOR** com um valor pseudoaleatório gerado pela **seed**.
    - **Adição modular** com o mesmo valor pseudoaleatório.

#### 2. **Geração de Valores Pseudoaleatórios**

- O **PRNG** gera valores pseudoaleatórios para cada bloco de 64 bits com base na **seed** da chave do usuário:
  ```c
  seed ^= seed >> (offset + 3);
  seed *= 6364136223846793005ULL;


#### 3. **Transformações Personalizadas**

- **`SubLong`:** Aplica XOR e adição modular com valores pseudoaleatórios.
- **`InvSubLong`:** Reverte a operação de `SubLong` usando subtração modular e XOR.

#### 4. **Seed Derivada da Chave**

- A mesma chave sempre gera a mesma sequência pseudoaleatória, enquanto diferentes chaves produzem sequências únicas.

---

### Estrutura do Código

#### Funções Principais

1. **`CUSTOM_AES_set_encrypt_key`:**

   - Expande a chave do usuário e deriva uma **seed** personalizada:
     ```c
     key->seed = derive_seed(userKey, bits / 8);
     ```

2. **`SubLong`:**

   - Transforma o estado do AES usando o PRNG:
     ```c
     u64 random = PRNG(key->seed, 0);
     *w ^= random;
     *w = (*w + random) & 0xFFFFFFFFFFFFFFFF;
     ```

3. **`InvSubLong`:**

   - Reverte a transformação de `SubLong`:
     ```c
     *w = (*w - random) & 0xFFFFFFFFFFFFFFFF;
     *w ^= random;
     ```

4. **Cifra e Decifra:**
   - Implementam rodadas de transformação utilizando as novas operações.

---

### Comparação com OpenSSL

O projeto inclui a implementação do AES tradicional usando a biblioteca OpenSSL. Através do script Python `testes.py`, você pode comparar:

1. **Tempos de execução:**
   - Criptografia e descriptografia.
2. **Verificação de integridade:**
   - Compara os arquivos originais com os descriptografados.

---

### Conclusão

Este projeto oferece uma abordagem alternativa ao AES tradicional, incorporando PRNG e transformações personalizadas. É voltado para estudos acadêmicos e experimentais, permitindo explorar o impacto das alterações no desempenho e na segurança.
