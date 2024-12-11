### README: AES Modificado com PRNG e Seed Personalizada

## Autoras

- Barbara Reis dos Santos
- Luize Cunha Duarte

## Como Compilar e Executar

### Compilação

Use o **Makefile** incluso para compilar o projeto:

```bash
make
```

### Geração de Dados de Teste

Antes de executar a criptografia e descriptografia, é necessário criar os arquivos de teste. Utilize o script Python incluído para gerar dados de diferentes tamanhos:

```bash
python3 gerar_testes.py
```

O script criará arquivos de teste na pasta `test_files`. Estes arquivos contêm bytes aleatórios para validar a funcionalidade e o desempenho do algoritmo. Outros arquivos podem ser adicionados manualmente.

### Execução do Programa

Após gerar os arquivos de teste, execute o programa principal para realizar operações de criptografia e descriptografia.

#### Sintaxe:

```bash
./aes <arquivo_entrada> <tamanho_chave_em_bits> [operacao] [chave_hex (apenas para decrypt)]
```

- **Parâmetros**:
  - `arquivo_entrada`: Caminho para o arquivo a ser processado.
  - `tamanho_chave_em_bits`: Tamanho da chave (128, 192 ou 256).
  - `operacao`: Tipo de operação: `encrypt`, `decrypt` ou `both`.
  - `chave_hex`: Apenas para descriptografia. Chave fornecida em formato hexadecimal.

Aqui está a versão revisada e melhorada dessa seção para tornar as informações mais claras e organizadas:

---

#### Exemplos de Uso:

- **Criptografia Simples:**

  ```bash
  ./aes test_files/test_1000KB.txt 128 encrypt
  ```

  Neste exemplo, o programa criptografa o arquivo fornecido utilizando uma chave gerada automaticamente. Essa chave será exibida no terminal e deve ser armazenada pelo usuário caso a descriptografia seja feita posteriormente de forma separada. Essa operação gera o arquivo encrypted_file_custom.aes que deverá ser usado na operação de descriptografia junto com a chave.

- **Criptografia e Descriptografia na Mesma Execução:**

  ```bash
  ./aes test_files/test_1000KB.txt 128 both
  ```

  Essa operação realiza tanto a criptografia quanto a descriptografia no mesmo processo. Após a execução, o programa gera arquivos intermediários (criptografados e descriptografados) e realiza automaticamente a comparação entre o texto original e o texto descriptografado para garantir a integridade dos dados.

- **Descriptografia com Chave Específica:**

  ```bash
  ./aes encrypted_file_custom.aes 128 decrypt 00112233445566778899aabbccddeeff
  ```

  Nesse caso, o programa descriptografa o arquivo utilizando uma chave hexadecimal fornecida pelo usuário. O arquivo resultante é comparado automaticamente com o arquivo original (indicado dentro do código), e o programa exibe o resultado da comparação.

---

### Saída dos Arquivos

- A cada operação, são gerados arquivos para validação:
  - **Arquivo Criptografado:** Contém os dados encriptados do arquivo original.
  - **Arquivo Descriptografado:** Resultado da descriptografia para verificar se corresponde ao texto original.

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
  ```

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
