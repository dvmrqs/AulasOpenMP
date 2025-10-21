/*-------------------------------------------------------------------------------------------------------------------------
 * File Name : 007_reduction_0.0.cpp
 * Author    : Prof. Rodrigo Gonçalves Pinto
 * Institution: University of São Paulo (USP) / University of Brasília (UnB) /IESB /SENAC
 * Course    : Parallel and Distributed Programming
 * Objective : Demonstration of OpenMP reduction basics
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





/*---------------------------------
reduction
---------------------------------*/
//vamos supor que precisamos calcular a soma de milhões de números.
//Uma forma paralela de fazer isso seria dividir o trabalho,
//por exemplo,cada thread soma uma parte dos números em uma
//variável local. No final, precisariamos efetuar as somas de cada thread
//em uma variável global. Isso pode ser feito com a cláusula reduction
//do OpenMP. A cláusula reduction cria uma variável privada para cada
//thread, que é inicializada com o valor da variável global. Quando a
//thread termina, o valor da variável privada é adicionado à variável
//global.


// já vimos anteriormente que proteger a soma final com critical
// funciona, no entanto, isso pode se tornar um gargalo de desempenho
// se muitas threads tentarem acessar a seção crítica ao mesmo tempo.
// pois threads podem ficar esperando na seção crítica para poder
// atualizar o total.


/*--------------------------------------------------------------
  A cláusula reduction automatiza e otimiza extamente esse processo.
----------------------------------------------------------------*/
//Ela instrui o OpenMP para.
// 1. Criar uma cópia privada da variável de redução para cada thread,
// ou seja , uma variável local para cada thread.
//
// 2. Inicializa essa cópia com o valor neutro para operação: no caso da soma,
// 0 para soma, 1 para multiplicação, etc.)
//
// 3. No final da região paralela, combina os valores das variáveis privadas,
// na variável global original, de forma segura e eficiente.

/*--------------------------------------------------
 reduction pode ser utilizada para diversas operações e associativas
 como:
 - soma,
 - multiplicação,
 - máximo,
 - mínimo,
    - operações lógicas (AND, OR, XOR).
    - concatenação de strings, etc.

É a forma mais preferida e que oferece mais desempenho para agregar
resultados em programas paralelos.
--------------------------------------------------*/


/*
  vamos utilizar Bhaskara como exemplo, o mesmo que já vimos
  com critical e atomic.
*/
#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>

// Função para retornar a SOMA das raízes de Bhaskara
double soma_raizes_bhaskara(double a, double b, double c) {
    double delta = (b * b) - (4 * a * c);
    if (delta < 0) return 0.0;
    double x1 = (-b + std::sqrt(delta)) / (2 * a);
    double x2 = (-b - std::sqrt(delta)) / (2 * a);
    return x1 + x2;
}

int main() {
    const int N = 10000;
    std::vector<double> a(N, 1.0), b(N, -7.0), c(N, 10.0); // Raízes 5 e 2, soma 7
    
    double soma_total = 0.0;

    // A mágica acontece aqui: reduction(+:soma_total)
    // 1. O OpenMP cria uma soma_total privada para cada thread, inicializada com 0.
    // 2. Cada thread acumula suas somas locais nesta variável privada. Não há conflito!
    // 3. No final do loop, o OpenMP soma os resultados de todas as variáveis
    //    privadas na soma_total global de forma segura e otimizada.
    //
    // Este código é mais limpo, mais legível e geralmente mais rápido que as
    // versões com critical ou atomic para este tipo de problema.
    #pragma omp parallel for reduction(+:soma_total)
    for (int i = 0; i < N; ++i) {
        // O resultado da função é somado diretamente à soma_total.
        // Internamente, a soma é feita na cópia privada da thread.
        soma_total += soma_raizes_bhaskara(a[i], b[i], c[i]);
    }

    std::cout << "Soma total das raízes com reduction: " << soma_total << std::endl;
    std::cout << "Resultado esperado: " << N * 7.0 << std::endl;

    return 0;
}