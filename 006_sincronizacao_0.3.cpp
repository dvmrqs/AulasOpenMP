/*--------------------------------------------
 Exemplos de sincronização de threads.
---------------------------------*/

/*--------------------------------------------
 Pragma omp barrier
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

3. Pragma omp barrier
--------------------*/
/*

A diretiva #pragma omp barrier cria um ponto de sincronização. 
Nenhuma thread pode passar da barreira até que *todas* as outras threads do time de threads tenham chegado a esse ponto.
Isso é muito útil quando o processamento é dividido em fases e a fase seguinte depende da conclusão da anterior por todas as threads.

No nosso exemplo, vamos simular um processo de duas fases:
1.  Fase 1: Todas as threads calculam as raízes e as armazenam em um vetor compartilhado.
2.  Fase 2: Após *todas* as raízes terem sido calculadas, as threads fazem uma verificação nos resultados.
*/

#include <iostream> 
#include <vector>
#include <cmath>
#include <omp.h>

// Função para calcular apenas a primeira raiz de Bhaskara
double primeira_raiz_bhaskara(double a, double b, double c) {
    double delta = (b * b) - (4 * a * c);
    if (delta < 0) return 0.0; // Retorna 0 se não houver raiz real
    return (-b + std::sqrt(delta)) / (2 * a);
}

int main() {
    const int N = 8; // Usando um número menor para facilitar a visualização
    std::vector<double> a(N, 1.0), b(N, -5.0), c(N, 6.0);
    std::vector<double> raizes_x1(N); // Vetor para armazenar os resultados

    #pragma omp parallel
    {
        int id = omp_get_thread_num();

        // --- FASE 1: CÁLCULO DAS RAÍZES ---
        // A diretiva 'for' distribui as iterações entre as threads.
        // Cada thread calcula uma parte das raízes e armazena no vetor 'raizes_x1'.
        #pragma omp for
        for (int i = 0; i < N; ++i) {
            raizes_x1[i] = primeira_raiz_bhaskara(a[i], b[i], c[i]);
            printf("Thread %d calculou a raiz para i = %d.\n", id, i);
        }

        // --- PONTO DE SINCRONIZAÇÃO (BARREIRA) ---
        // A diretiva 'barrier' força todas as threads a esperarem aqui.
        // Nenhuma thread continuará para a Fase 2 até que TODAS as threads
        // tenham terminado a Fase 1 (o loop 'for' acima).
        // Isso garante que o vetor 'raizes_x1' esteja completamente preenchido
        // antes de prosseguir.
        #pragma omp barrier

        // --- FASE 2: VERIFICAÇÃO DOS RESULTADOS ---
        // Agora que sabemos que todos os cálculos da Fase 1 terminaram,
        // podemos executar uma tarefa que dependa desses resultados.
        #pragma omp for
        for (int i = 0; i < N; ++i) {
            printf("Thread %d verificando o resultado de i = %d, que é %.2f.\n", id, i, raizes_x1[i]);
        }
    }

    return 0;
}
