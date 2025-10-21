/*---------------------------------
Exercícios
---------------------------------*/

/*
 Utilize o código base abaixo para a resolução dos exercícios.
 Ao final de cada código, escreva um parágrafo com suas palavras

 a) Qual é o objetivo do código em detalhes.
 b) O que o seu código faz, etapa por etapa em detalhes.
 c) Ao final de cada exercício, ouvirei uma explicação sua sobre o que está sendo feito.
 
 */

 /*
  Explicações, datas:

  exercício 1 e 2 hoje: 07/10/2025.
  exercício 3 e 4 14/10/2025.
  exercício 5     21/10/2025.
 */


#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>

// Função para calcular as raízes de uma equação de segundo grau
// Retorna um std::pair contendo as raízes x1 e x2.
// Se não houver raízes reais, ambas serão 0.
std::pair<double, double> resolver_bhaskara(double a, double b, double c) {
    double delta = (b * b) - (4 * a * c);

    if (delta < 0) {
        return {0.0, 0.0};
    }

    double x1 = (-b + std::sqrt(delta)) / (2 * a);
    double x2 = (-b - std::sqrt(delta)) / (2 * a);

    return {x1, x2};
}

int main() {
    // Número de equações a serem resolvidas
    const int N = 1000;

    // Vetores para armazenar os coeficientes
    std::vector<double> a(N), b(N), c(N);

    // Preenche os vetores com valores de exemplo.
    // Metade terá raízes reais, a outra metade não.
    for (int i = 0; i < N; ++i) {
        a[i] = 1.0;
        if (i % 2 == 0) {
            // Equação com raízes reais: x^2 - 7x + 10 = 0 (raízes 5 e 2)
            b[i] = -7.0;
            c[i] = 10.0;
        } else {
            // Equação sem raízes reais: x^2 + 2x + 5 = 0 (delta < 0)
            b[i] = 2.0;
            c[i] = 5.0;
        }
    }

    // ===============================================================
    //               ÁREA PARA RESOLVER OS EXERCÍCIOS
    // ===============================================================

    // Adicione aqui o seu código OpenMP para cada exercício.


    // ===============================================================

    return 0;
}

/*------------------------------------------------------
 Instruções dos exercícios:
------------------------------------------------------*/
/* 1. Faça um programa que utilize OpenMP para paralelizar a resolução das equações do segundo
grau com a formula de Bhaskara.

2. crie uma entrada de dados onde o usuário decicirá a quantidade de equações a serem resolvidas.
Ao final, mostre o tempo gasto para resolver as equações, e o número de equações que possuem raízes reais e quantas não possuem.

3.Utilizam todos os conceitos do exercios das aulas 006, na mesma ordem em qual foram apresentados.
*/