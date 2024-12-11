/* Autoras: Barbara Reis dos Santos e Luize Cunha Duarte */
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// Função para fatorar parâmetro n, retornando os dois fatores primos.
pair<ll, ll> factorize(ll n){
    vector<ll> factors;
    for (ll i = 2; i * i <= n; i++){
        while (n % i == 0){
            factors.push_back(i);
            n /= i;
        }
    }
    if (n > 1){
        factors.push_back(n);
    }
    if (factors.size() != 2){
        return {-1, -1}; // Fatoração inválida para RSA.
    }
    return {factors[0], factors[1]};
}

// Função para calcular o MDC entre dois números recebidos como parâmetro.
ll gcd(ll a, ll b){
    while (b != 0){
        ll temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Função para calcular o inverso modular usando o algoritmo de Euclides.
// Recebe o E e phi. Retorna o D.
ll modInverse(ll e, ll phi){
    ll t = 0, new_t = 1;    // t e new_t são os coeficientes de e e phi, respectivamente.
    ll r = phi, new_r = e;
    
    while (new_r != 0){
        ll quotient = r / new_r;
        ll temp = new_t;
        new_t = t - quotient * new_t;
        t = temp;

        temp = new_r;
        new_r = r - quotient * new_r;
        r = temp;
    }

    if (r > 1)
        return -1; // Não existe inverso modular

    if (t < 0) t += phi; // Ajusta o resultado para ser positivo
    return t;
}

int main(){
    ll e, n;
    cout << "Digite a chave pública (e, n): ";
    cin >> e >> n;

    // Fatora n para encontrar p e q
    auto [p, q] = factorize(n);
    if (p == -1 || q == -1){
        cerr << "Não foi possível fatorar n em p e q.\n";
        return 1;
    }

    ll phi = (p - 1) * (q - 1);

    // Verifica se E e phi(n) são coprimos
    if (gcd(e, phi) != 1){
        cerr << "E e phi(n) não são coprimos, não é possível calcular o inverso modular.\n";
        return 1;
    }

    // Se forem coprimos, calcula o inverso modular
    ll d = modInverse(e, phi);
    if (d == -1){
        cerr << "Erro: Não foi possível calcular o inverso modular.\n";
        return 1;
    }

    cout << "Os fatores de n são: p = " << p << ", q = " << q << endl;
    cout << "A chave privada d é: " << d << endl;
    return 0;
}
