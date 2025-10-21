/*-------------------------------------------------------------------------------------------------------------------------
 * File Name : 007_reduction_02.cpp
 * Author    : Prof. Rodrigo Gonçalves Pinto
 * Institution: University of Brasília (UnB) / University of São Paulo (USP) /IESB /SENAC
 * Course    : Parallel and Distributed Programming
 * Objective :  Demonstration of OpenMP reduction to compute population mean and variance (sum and sum of squares)
 * Semester  : 2025/2
 * Version   : 2.0
 * 
 * History:
 *   Creation date : 2023-05-15
 *   Update date   : 2024-04-26
 *   Updated by    : Rodrigo Gonçalves Pinto
 *   Changes made  : Adapted for C++17; general code revision and documentation update.
 --------------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------
 * Apache License, Version 2.0
 *-------------------------------------
 * Copyright (c) 2023–2025 Rodrigo Gonçalves Pinto
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Modifications: Please maintain all change logs in the "History" section above.
 */

/*-------------------------------------
 * NOTICE
 *-------------------------------------
 * Parallel Programming Examples in C++ (OpenMP)
 * Copyright (c) 2023–2025 Rodrigo Gonçalves Pinto
 *
 * This product includes software developed by Rodrigo Gonçalves Pinto.
 *
 * Academic Attribution:
 * If you use this software in academic or scientific work, please cite:
 *   Pinto, R. G. (2025). Parallel Programming Examples in C++ using OpenMP.
 *   ORCID: https://orcid.org/0009-0008-8360-9538
 *   University of São Paulo (USP) or University of Brasília (UnB).
 *
 * Any distribution of this software or derivative works must reproduce
 * this NOTICE file.
 */





/*---------------------------------------------------
 Mais exemplos de uso da cláusula reduction do OpenMP.
-----------------------------------------------------*/

//Média populacional:
//            N
//      μ = ( Σ xᵢ ) / N
//           i=1


//Variância =
//             N
//     σ² =  ( Σ (xᵢ - μ)² ) / N onde,
//             i=1

//     σ² = Variância
//     xᵢ = Cada valor individual no conjunto de dados
//     μ  = Média populacional do conjunto de dados
//     N  = Número total de valores no conjunto de dados



//Observe que para calcular isso, precisamos de duas somas do nosso 
//conjunto de dados:
//A soma de todos os valores (Σx).
//A soma de todos os valores ao quadrado ( Σ (xᵢ - μ)²).
//A cláusula reduction do OpenMP nos permite calcular ambas as somas em um único loop paralelo!
//Podemos simplesmente listar as duas variáveis na cláusula,
// e o OpenMP cuidará de criar cópias privadas para cada uma e combiná-las no final.

/*-----------------------------------------
 Cenário: Temos uma matriz A que representa:
 1. Salários de DEPARTAMENTOS x FUNCIONARIOS. 
 2. Queremos calcular o desvio padrão de todos os salários da empresa.
------------------------------------------------*/


/*-------------------------------------------------
Atenção: é recomendado utilizar um operador
matemático por cláusula reduction.
--------------------------------------------------
- Se precisar de mais de um operador, utilize múltiplas
cláusulas reduction na mesma diretiva.

//Exemplo:
#pragma omp parallel for reduction(+:soma) reduction(*:produto)
for (int i = 0; i < N; ++i) {
    soma += vec[i];
    produto *= vec[i];
}
Isso diz ao OpenMP:
1. Gerenciar uma redução do tipo soma para a variável soma.
2. Gerenciar uma redução do tipo produto para a variável produto.

O openmp fará isso de forma independente e paralela, cuidando
 para que todas as cópias privadas sejam cominadas no final, para
 cada variável, de acordo com a sua operação específica.


*/


// vamos ao exemplo.
#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <iomanip>

int main() {
    // Exemplo: salários empresa fictícia
    const int DEPARTAMENTOS = 100;
    const int FUNCIONARIOS  = 500;
    const int N = DEPARTAMENTOS * FUNCIONARIOS;

    std::vector<double> salarios(N);

    // Inicialização parallel apenas para produzir dados de exemplo
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        // salário base 4000 + variação com padrão simples
        salarios[i] = 4000.0 + (i % 100) * 20.0;
    }

    // ---------------------------
    // 1) MÉDIA POPULACIONAL (μ)
    // ---------------------------
    double soma = 0.0;

    #pragma omp parallel for reduction(+:soma)
    for (int i = 0; i < N; ++i) {
        soma += salarios[i];
    }

    const double mu = soma / static_cast<double>(N);

    // ----------------------------------------------------
    // 2) VARIÂNCIA POPULACIONAL (σ²) PELA FÓRMULA DIRETA
    //     σ² = (1/N) * Σ (xᵢ − μ)²
    // ----------------------------------------------------
    double soma_desvios2 = 0.0;

    #pragma omp parallel for reduction(+:soma_desvios2)
    for (int i = 0; i < N; ++i) {
        const double d = salarios[i] - mu;
        soma_desvios2 += d * d;
    }

    double variancia_pop = soma_desvios2 / static_cast<double>(N);

    // (opcional) blindagem para eventuais imprecisões numéricas negativas muito pequenas
    if (variancia_pop < 0.0 && std::fabs(variancia_pop) < 1e-12) {
        variancia_pop = 0.0;
    }

    const double desvio_padrao_pop = std::sqrt(variancia_pop);

    // Saída
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Analise Salarial (Populacional)\n";
    std::cout << "N = " << N << "\n";
    std::cout << "Media (μ)                  : R$ " << mu << "\n";
    std::cout << "Variancia populacional (σ²): R$ " << variancia_pop << "\n";
    std::cout << "Desvio-padrao populacional : R$ " << desvio_padrao_pop << "\n";

   

    return 0;
}
