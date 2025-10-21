/*---------------------------------------
Paralelizar o for básico com OpenMP
-----------------------------------------*/
/*
 Compilar (GCC/MinGW):
   g++ -O2 -fopenmp basic_parallel.cpp -o basic_parallel.exe
 */

#include <iostream>
#include <vector>
#include <omp.h>

int main() {
    const int N = 10;                 // tamanho dos vetores
    std::vector<int> x(N), y(N), z(N), a(N);

    // Inicializa valores simples (sequencial)
    for (int i = 0; i < N; i++) {
        x[i] = i;
        y[i] = i + 1;
        z[i] = i + 2;
    }

    std::cout << "Calculando a = x*x + y*y + z*z para vetores de tamanho " << N << "\n";

    /*--------------------------------------------------
     Parallelizar o loop for com OpenMP
    ----------------------------------------------------*/
    // a siretiva omp parallel for paraleliza o loop for mais próximo.
    // essa diretiva diz ao compilador: crie um time de threads e divida as iterações do for entre elas.
    // cada thread recebe um pedaço do intervalo[0,M) para calcular.

    /*--------------------------------------------------
     A cláusula schedule(static)
     --------------------------------------------------
     define como as iterações do loop são distribuídas entre as threads.
     - static: o compilador divide as iterações em blocos de tamanho igual e atribui fixamente a 
       cada thread.
       Exemplo: N = 10, 2 Threads
       -->  Thread 0: i = 0,1,2,3,4
            Thread 1: i = 5,6,7,8,9 
     -  isso é rápido, ótimo quando todas as iterações tem o mesmo custo.

     --------------------------------------------------
     O corpo a[i] = x[i]*x[i] + y[i]*y[i] + z[i]*z[i];
    - cada thread calcula o valor de a[i] para o subconjunto de índices que recebeu.
    - Não há risco de conflito porque cada a[i] é escrito apenas por uma thread.
     --------------------------------------------------*/

    
    
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < N; i++) {
        a[i] = x[i]*x[i] + y[i]*y[i] + z[i]*z[i];
        // (evite imprimir aqui para não embaralhar a saída)
    }

    // --- IMPRESSÃO EM ORDEM (SEQUENCIAL) ---
    // Mantemos a impressão fora da região paralela para garantir ordem por i.
    for (int i = 0; i < N; i++) {
        std::cout << "i = " << i
                  << " | x[i] = " << x[i]
                  << " | y[i] = " << y[i]
                  << " | z[i] = " << z[i]
                  << " | a[i] = " << a[i]
                  << std::endl;
    }

    // Resumo tabular
    std::cout << "\nResumo dos resultados:\n";
    std::cout << "----------------------------------------\n";
    std::cout << "  i |   x[i] |   y[i] |   z[i] |   a[i]\n";
    std::cout << "----------------------------------------\n";
    for (int i = 0; i < N; i++) {
        std::cout << " " << i
                  << "  | " << x[i]
                  << "     | " << y[i]
                  << "     | " << z[i]
                  << "     | " << a[i]
                  << std::endl;
    }
    std::cout << "----------------------------------------\n";
}

/*--------------------------------------------------
 Analisando a saída
 --------------------------------------------------*/
/*
  A saída mostra que o cálculo foi feito corretamente.
  Cada a[i] corresponde a x[i]^2 + y[i]^2 + z[i]^2.

  A paralelização do loop for com OpenMP é simples e eficaz para acelerar cálculos independentes.
  Cada thread trabalha em seu próprio subconjunto de dados, evitando conflitos de escrita.

  Observação:
  - Evite imprimir dentro do loop paralelo, pois isso pode embaralhar a saída.
  - Mantenha a impressão fora da região paralela para garantir ordem e clareza.
  - o OpenMP gerencia a criação e sincronização das threads automaticamente.
  - o OpenMP escolhe o número de threads baseado no número de núcleos lógicos da CPU por padrão.
  - para controlar o número de threads, use a variável de ambiente OMP_NUM_THREADS.
  */