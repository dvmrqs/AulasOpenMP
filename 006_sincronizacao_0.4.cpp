/*--------------------------------------------
 Exemplos de sincronização de threads.
---------------------------------*/

/*--------------------------------------------
 Pragma omp ordered
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

4. Pragma omp ordered

A diretiva #pragma omp ordered é usada dentro de um loop com a cláusula ordered para garantir que um bloco de código 
seja executado na mesma ordem que seria em uma execução sequencial.
Isso é útil para tarefas como impressão de resultados,
ou escrita em arquivos, onde a ordem das iterações importa.

nesse exemplo, vamos calcular as raízes e imprimir o resultado de cada equação na ordem correta, de i = 0 a N-1.

*/

#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>

// Função resolver_bhaskara (a mesma dos exemplos anteriores)
double resolver_bhaskara(double a, double b, double c) {
    double delta = (b * b) - (4 * a * c);
    if (delta < 0) return 0.0;
    double x1 = (-b + std::sqrt(delta)) / (2 * a);
    double x2 = (-b - std::sqrt(delta)) / (2 * a);
    return x1 + x2;
}

int main() {
    const int N = 10;
    std::vector<double> a(N, 1.0), b(N, -5.0), c(N, 6.0);

    // A cláusula 'ordered' no loop 'for' é necessária para usar
    // a diretiva '#pragma omp ordered' dentro do loop.
    #pragma omp parallel for ordered
    for (int i = 0; i < N; ++i) {
        double soma_local = resolver_bhaskara(a[i], b[i], c[i]);

        // --- BLOCO ORDENADO ---
        // Embora as iterações do loop (cálculo de soma_local) possam ser
        // executadas fora de ordem pelas threads, o bloco de código
        // dentro de '#pragma omp ordered' será executado na ordem sequencial
        // das iterações (i=0, i=1, i=2, ...).
        #pragma omp ordered
        {
            std::cout << "Resultado para a equação " << i << ": Soma das raízes = " << soma_local << std::endl;
        }
    }

    return 0;
}