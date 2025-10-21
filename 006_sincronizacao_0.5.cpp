/*--------------------------------------------
 Exemplos de sincronização de threads.
---------------------------------*/

/*--------------------------------------------
utilizando locks
-------------------------------------------*/


/*--------------------------------------------
 Contexto para a fórmula de Bhaskara
-------------------------------------------*/
/*
--> A Fórmula de Bhaskara
--------------------------------
A fórmula de Bhaskara é usada para encontrar as raízes de uma equação de segundo grau
do tipo ax² + bx + c = 0. 
As raízes (x1 e x2) são calculadas da seguinte forma:

1.  **Calcular o discriminante (delta):** Δ = b² - 4ac
2.  **Calcular as raízes:**
    *   x1 = (-b + √Δ) / 2a
    *   x2 = (-b - √Δ) / 2a

Vamos supor o seguinte cenário:
Temos um grande número de equações de segundo grau para resolver e
queremos paralelizar o processo.

Para isso, armazenaremos os coeficientes a, b, e c em arrays e calcularemos as raízes, 
somando-as em uma variável total para verificar o resultado.
É neste ponto que a sincronização se torna crucial.

 vamos calcular as raízes de várias equações e somá-las a uma variável soma_total.
 Sem a sincronização, as threads poderiam tentar atualizar soma_total simultaneamente, 
 fazendo com que o valor final ficasse incorreto.

------------------------------------

5. funções lock

Funções de Lock (omp\_init\_lock, omp\_set\_lock, omp\_unset\_lock, omp\_destroy\_lock)

Locks oferecem um mecanismo de sincronização mais flexível e de baixo nível que a diretiva critical.
Eles são úteis em cenários mais complexos, como proteger o acesso a múltiplas estruturas de dados
ou quando a lógica de bloqueio e desbloqueio não está contida em um único bloco de código.

Como exemplo, vamos criar dois "recursos" (duas variáveis de soma) e usar dois locks separados para protegê-los, 
permitindo que threads diferentes atualizem recursos diferentes ao mesmo tempo.

--------------------*/


#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>

// Função resolver_bhaskara 
double resolver_bhaskara(double a, double b, double c) {
    double delta = (b * b) - (4 * a * c);
    if (delta < 0) return 0.0;
    double x1 = (-b + std::sqrt(delta)) / (2 * a);
    double x2 = (-b - std::sqrt(delta)) / (2 * a);
    return x1 + x2;
}

int main() {
    const int N = 1000;
    std::vector<double> a(N, 1.0), b(N, -5.0), c(N, 6.0);

    // Variáveis que representam dois recursos compartilhados diferentes
    double soma_pares = 0.0;
    double soma_impares = 0.0;

    // Declaração de duas variáveis de lock, uma para cada recurso
    omp_lock_t lock_pares;
    omp_lock_t lock_impares;

    // 1. Inicializa os locks antes de usá-los.
    // Isso prepara a estrutura do lock para ser utilizada.
    omp_init_lock(&lock_pares);
    omp_init_lock(&lock_impares);

    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        double soma_local = resolver_bhaskara(a[i], b[i], c[i]);

        // Lógica para decidir qual recurso/lock usar
        if (i % 2 == 0) {
            // 2. Adquire o lock (trava).
            // A thread irá esperar aqui se o lock_pares já estiver
            // em uso por outra thread. Uma vez que a thread adquire o lock,
            // nenhuma outra pode adquiri-lo até que seja liberado.
            omp_set_lock(&lock_pares);
            soma_pares += soma_local; // Acesso seguro ao recurso
            
            // 3. Libera o lock (destrava).
            // É CRUCIAL liberar o lock para que outras threads
            // que estão esperando possam continuar.
            omp_unset_lock(&lock_pares);
        } else {
            omp_set_lock(&lock_impares);
            soma_impares += soma_local;
            omp_unset_lock(&lock_impares);
        }
    }

    // 4. Destrói os locks após o uso.
    // Isso libera a memória alocada para os locks.
    // Esquecer de fazer isso pode causar vazamentos de memória.
    omp_destroy_lock(&lock_pares);
    omp_destroy_lock(&lock_impares);

    std::cout << "Soma das equações pares: " << soma_pares << std::endl;
    std::cout << "Soma das equações ímpares: " << soma_impares << std::endl;
    std::cout << "Soma total (locks): " << soma_pares + soma_impares << std::endl;

    return 0;
}
