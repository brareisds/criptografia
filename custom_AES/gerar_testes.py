import os

# Diretório e tamanhos dos arquivos de teste
test_dir = "./test_files"
os.makedirs(test_dir, exist_ok=True)
file_sizes = [1, 10, 100, 500, 1000, 5000]  # Tamanhos em KB

# Função para criar arquivos de entrada de tamanhos variados
def create_test_files(base_path, sizes_in_kb):
    print("Gerando arquivos de teste...")
    for size in sizes_in_kb:
        file_name = os.path.join(base_path, f"test_{size}KB.txt")
        with open(file_name, 'wb') as f:
            f.write(os.urandom(size * 1024))  # Escreve bytes aleatórios
        print(f"Arquivo gerado: {file_name} ({size} KB)")

# Gerar arquivos de teste
create_test_files(test_dir, file_sizes)

print("Todos os arquivos de teste foram gerados com sucesso.")
