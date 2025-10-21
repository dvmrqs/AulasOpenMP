
/*--------------------------------------------
 Exemplos de sincronização de threads.
---------------------------------*/

/*--------------------------------------------
 Pragma omp atomic
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

2. Pragma omp atomic
--------------------*/
/*

A diretiva #pragma omp atomic é uma forma mais leve e eficiente de proteger operações específicas,
como incrementos, decrementos ou somas. 
Ela é mais restrita que a critical, mas oferece melhor desempenho quando a operação é simples
e suportada pelo hardware.

Vamos utilizar novamente o exemplo anterior usando atomic para proteger a atualização da soma_total.

*/

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
    std::vector<double> a(N), b(N), c(N);
    double soma_total = 0.0;

    for (int i = 0; i < N; ++i) {
        a[i] = 1.0;
        b[i] = -5.0;
        c[i] = 6.0;
    }

    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        double soma_local = resolver_bhaskara(a[i], b[i], c[i]);

        // --- OPERAÇÃO ATÔMICA ---
        // A diretiva 'atomic' é mais eficiente que 'critical' para
        // operações simples de atualização (leitura-modificação-escrita).
        // Ela garante que a operação 'soma_total += soma_local'
        // seja executada de forma indivisível, sem interferência de outras threads.
        // É a escolha ideal para contadores ou acumuladores.
        #pragma omp atomic
        soma_total += soma_local;
    }

    std::cout << "Soma total (com #pragma omp atomic): " << soma_total << std::endl;
    std::cout << "Resultado esperado: " << N * 5.0 << std::endl;

    return 0;
}
