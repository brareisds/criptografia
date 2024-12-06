import os
import pandas as pd
import matplotlib.pyplot as plt
import subprocess

# Diretório e arquivos de teste
test_dir = "./test_files"
os.makedirs(test_dir, exist_ok=True)
file_sizes = [1, 10, 100, 500, 1000, 5000]  # Tamanhos em KB
key = "mytestkey1234567"  # Chave de teste

# Função para criar arquivos de entrada de tamanhos variados
def create_test_files(base_path, sizes_in_kb):
    files = []
    for size in sizes_in_kb:
        file_name = os.path.join(base_path, f"test_{size}KB.txt")
        with open(file_name, 'wb') as f:
            f.write(os.urandom(size * 1024))
        files.append(file_name)
    return files

# Rodar os testes com o programa em C
def run_c_tests(files, key):
    results = []
    for file in files:
        # Executar o programa e capturar a saída
        result = subprocess.run(["./aes", file, key], capture_output=True, text=True)
        output = result.stdout.splitlines()
        
        # Verificar se a saída está no formato esperado
        if len(output) < 4:
            print(f"Erro: saída inesperada para o arquivo {file}")
            continue
        
        try:
            custom_encrypt = output[0].split(":")[1].strip().replace(" seconds", "")
            custom_decrypt = output[1].split(":")[1].strip().replace(" seconds", "")
            openssl_encrypt = output[2].split(":")[1].strip().replace(" seconds", "")
            openssl_decrypt = output[3].split(":")[1].strip().replace(" seconds", "")

            # Verificar se a saída é "Yes" e definir o tempo como zero
            custom_encrypt = 0.0 if custom_encrypt == "Yes" else float(custom_encrypt)
            custom_decrypt = 0.0 if custom_decrypt == "Yes" else float(custom_decrypt)
            openssl_encrypt = 0.0 if openssl_encrypt == "Yes" else float(openssl_encrypt)
            openssl_decrypt = 0.0 if openssl_decrypt == "Yes" else float(openssl_decrypt)
        except ValueError as e:
            print(f"Erro ao processar a saída para o arquivo {file}: {e}")
            continue

        file_size = os.path.getsize(file) / 1024  # Converter para KB

        results.append({
            "File Size (KB)": file_size,
            "Custom Encrypt (s)": custom_encrypt,
            "Custom Decrypt (s)": custom_decrypt,
            "OpenSSL Encrypt (s)": openssl_encrypt,
            "OpenSSL Decrypt (s)": openssl_decrypt
        })
    return results

# Criar arquivos de entrada
files = create_test_files(test_dir, file_sizes)

# Executar os testes
results = run_c_tests(files, key)

# Criar um DataFrame para análise
df = pd.DataFrame(results)
print(df)

# Plotar os resultados
plt.figure(figsize=(10, 6))
plt.plot(df["File Size (KB)"], df["Custom Encrypt (s)"], marker='o', label="Custom Encrypt")
plt.plot(df["File Size (KB)"], df["Custom Decrypt (s)"], marker='o', label="Custom Decrypt")
plt.plot(df["File Size (KB)"], df["OpenSSL Encrypt (s)"], marker='o', label="OpenSSL Encrypt")
plt.plot(df["File Size (KB)"], df["OpenSSL Decrypt (s)"], marker='o', label="OpenSSL Decrypt")
plt.xlabel("Tamanho do Arquivo (KB)")
plt.ylabel("Tempo (s)")
plt.title("Comparação de Tempos - Custom AES vs OpenSSL")
plt.legend()
plt.grid()
plt.show()

# Limpar arquivos de teste
# for file in files:
#     os.remove(file)