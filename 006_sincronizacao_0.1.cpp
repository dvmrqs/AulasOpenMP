/*--------------------------------------------
 Exemplos de sincronização de threads.
---------------------------------*/

/*--------------------------------------------
 Pragma omp critical
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

1.  Calcular o discriminante (delta): Δ = b² - 4ac
2.  Calcular as raízes:
       x1 = (-b + √Δ) / 2a
      x2 = (-b - √Δ) / 2a

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

1. Pragma omp critical
--------------------*/


#include <iostream> // Para entrada e saída
#include <vector> // vetor  
#include <cmath> // para operações matemátias
#include <omp.h> // openMp API.

// Função para resolver uma equação de segundo grau usando Bhaskara
// e retornar a soma das raízes (ou 0 se não houver raízes reais).
double resolver_bhaskara(double a, double b, double c) {
    // Calcula delta
    double delta = (b * b) - (4 * a * c);

    // Se o delta for negativo, não existem raízes reais.
    if (delta < 0) {
        return 0.0;
    }

    // Calcula as duas raízes
    double x1 = (-b + std::sqrt(delta)) / (2 * a);
    double x2 = (-b - std::sqrt(delta)) / (2 * a);

    // Retorna a soma das raízes
    return x1 + x2;
}

int main() {
    // Número de equações a serem resolvidas em paralelo
    const int N = 1000;

    // Vetores para armazenar os coeficientes de N equações
    std::vector<double> a(N), b(N), c(N);

    double soma_total = 0.0; 

    // Preenche os vetores com valores de exemplo
    for (int i = 0; i < N; ++i) {
        a[i] = 1.0;
        b[i] = -5.0;
        c[i] = 6.0; // Raízes são 2 e 3, a soma é 5
    }

    // Inicia a região paralela. O loop for será dividido entre as threads.
    // A variável 'soma_total' é compartilhada por todas as threads.
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        // Cada thread calcula a soma das raízes para uma equação.
        double soma_local = resolver_bhaskara(a[i], b[i], c[i]);

        // --- INÍCIO DA SEÇÃO CRÍTICA ---
        // A diretiva 'critical' garante que apenas UMA thread por vez
        // possa executar o bloco de código a seguir.
        // Isso evita que duas threads tentem atualizar 'soma_total'
        // ao mesmo tempo, o que poderia causar a perda de uma das atualizações.
        #pragma omp critical
        {
            soma_total += soma_local;
        }
        // --- FIM DA SEÇÃO CRÍTICA ---
    }

    // O resultado esperado é N * 5, pois a soma das raízes de cada equação é 5.
    std::cout << "Soma total (com #pragma omp critical): " << soma_total << std::endl;
    std::cout << "Resultado esperado: " << N * 5.0 << std::endl;

    return 0;
}
