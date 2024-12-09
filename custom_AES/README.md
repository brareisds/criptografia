### README: AES Modificado com PRNG e Seed Personalizada

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

  Neste exemplo, o programa criptografa o arquivo fornecido utilizando uma chave gerada automaticamente. Essa chave será exibida no terminal e deve ser armazenada pelo usuário caso a descriptografia seja feita posteriormente de forma separada.

- **Criptografia e Descriptografia na Mesma Execução:**

  ```bash
  ./aes test_files/test_1000KB.txt 128 both
  ```

  Essa operação realiza tanto a criptografia quanto a descriptografia no mesmo processo. Após a execução, o programa gera arquivos intermediários (criptografados e descriptografados) e realiza automaticamente a comparação entre o texto original e o texto descriptografado para garantir a integridade dos dados.

- **Descriptografia com Chave Específica:**

  ```bash
  ./aes test_files/test_1000KB.txt 128 decrypt 00112233445566778899aabbccddeeff
  ```

  Nesse caso, o programa descriptografa o arquivo utilizando uma chave hexadecimal fornecida pelo usuário. O arquivo resultante é comparado automaticamente com o arquivo original (passado por parâmetro), e o programa exibe o resultado da comparação.

---

### Saída dos Arquivos

- A cada operação, são gerados arquivos para validação:
  - **Arquivo Criptografado:** Contém os dados encriptados do arquivo original.
  - **Arquivo Descriptografado:** Resultado da descriptografia para verificar se corresponde ao texto original.

### Comparação de Desempenho

O programa compara o desempenho do AES customizado com a implementação do OpenSSL, exibindo tempos de execução para ambas as versões. Os resultados incluem:

- Tempo de criptografia e descriptografia.
- Verificação de integridade entre arquivos originais e descriptografados.

---

## Estrutura do Projeto

- **`main.c`**: Código principal para criptografia/descriptografia e comparação com OpenSSL.
- **`aes.c`**: Implementação customizada do algoritmo AES, com PRNG e substituição da S-Box.
- **`gerar_testes.py`**: Script Python para criar arquivos de teste.

---
