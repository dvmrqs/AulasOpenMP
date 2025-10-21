/*-------------------------------------------------------------------------------------------------------------------------
 * File Name : 007_reduction_02.cpp
 * Author    : Prof. Rodrigo Gonçalves Pinto
 * Institution: University of Brasília (UnB) / University of São Paulo (USP) /IESB /SENAC
 * Course    : Parallel and Distributed Programming
 * Objective :  Demonstration of OpenMP reduction with logical operators (&&, ||)
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


/*-------------------------------------------------
Reduction com Operadores Lógicos (&&, ||)
---------------------------------------------------
- A cláusula reduction não se limita a operações aritméticas. 
  Ela também funciona com operadores lógicos, como &&  (E) e  || (OU).
  Isso é útil para cenários onde precisamos combinar resultados booleanos 
  de múltiplas threads.

  Sendo assim, temos dois casos principais:

- reduction(&& : flag): O resultado final será true somente se o flag for true em todas
 as threads. 
Logo, começa com true, e se uma única thread o tornar false, o resultado final será false.


- reduction(|| : flag): O resultado final será true se o flag for true em pelo menos uma thread. 
   Logo, começa com false, e se uma única thread o tornar true, o resultado final será true.

   --------------------------------------------------
   Objetivo do código exemplo:
   
   Vamos auditar nosso vetor de salários para verificar
   três regras de negócio da empresa simultaneamente:
   ------------------------------------------------
1. Regra 1: Existe algum salário abaixo do piso de R$ 1500,00? (`||`)
2. Regra 2: Existe algum salário acima do teto de R$ 20000,00? (`||`)
3. Regra 3: Todos os salários registrados são valores positivos? (`&&`)
    --------------------------------------------------
    Cada regra será representada por uma variável booleana.
    Usaremos a cláusula reduction para combinar os resultados
    de todas as threads e obter o resultado final de cada regra.
*/

#include <iostream>
#include <vector>
#include <omp.h>

int main() {
    const int N = 100000;
    std::vector<double> salarios(N);

    // Preenche com dados "bons"
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        salarios[i] = 5000.0;
    }

    // Injeta alguns dados "ruins" para a auditoria encontrar
    salarios[1000] = 1400.0;      // Viola a Regra 1 (abaixo do piso)
    salarios[20000] = -50.0;     // Viola a Regra 3 (salário negativo)
    salarios[50000] = 25000.0;  // Viola a Regra 2 (acima do teto)

    /*---------------------------------------------
        Variáveis chamdas de  flag para a auditoria.
        Vamos declarar três variáveis booleanas.
        1. piso_violado.
        2. teto_violado.
        3. dados_validos.
    ---------------------------------------------*/
     
    /*-------------------------------------------------------
     Os valores iniciais para as variáveis booleanas, assumem:
     -------------------------------------------------
     1. piso_violado: para essa variável, o inicio é false. Pois vamos assumir que 
     nenhum piso foi violado ainda.

     2. teto_violado: para essa variável, o inicial é false. Pois vamos assumir que
     nenhum teto foi violado ainda.

     3. dados_validos: para essa variável, o inicial é true. Pois vamos assumir que
     nenhum dado é inválido ainda.
    */
   
    bool piso_violado = false;   //Regra 1: Existe algum salário abaixo do piso de R$ 1500,00? 
    bool teto_violado = false;  //Regra 2: Existe algum salário acima do teto de R$ 20000,00? 
    bool dados_validos = true; //Regra 3: Todos os salários registrados são valores positivos?

    // Aqui, vamos definir algumas constantes para o piso e teto salarial.
    const double PISO_SALARIAL = 1500.0;
    const double TETO_SALARIAL = 20000.0;

   /*-------------------------------------------------
    reduction com 3 variáveis e operadores diferentes.
    ---------------------------------------------------
    - Podemos ter múltiplas cláusulas reduction na mesma diretiva,
      uma para cada tipo de operador.
    */
    #pragma omp parallel for \
        reduction(||:piso_violado, teto_violado) \
        reduction(&&:dados_validos)
    for (int i = 0; i < N; ++i) {
        double s = salarios[i];

        // Regra 1: Se encontrarmos UM salário abaixo, o piso foi violado.
        if (s < PISO_SALARIAL) {
            piso_violado = true; //a thread grita encontrei!
        }

        // Regra 2: Se encontrarmos UM salário acima, o teto foi violado.
        if (s > TETO_SALARIAL) {
            teto_violado = true; //a thread grita encontrei!
        }

        // Regra 3: Se encontrarmos UM salário inválido, nem todos os dados são válidos.
        if (s <= 0) {
            dados_validos = false;
        }
    }

    /*------------------------------------------------- 
     Apresentação dos Resultados da Auditoria
    --------------------------------------------------*/
    std::cout << "Relatório de auditoria salarial" << std::endl;
    if (piso_violado) {
        std::cout << "[ALERTA] Regra 1: Encontrado pelo menos um salário abaixo do piso de R$ " << PISO_SALARIAL << std::endl;
    } else {
        std::cout << "[OK] Regra 1: Nenhum salário abaixo do piso foi encontrado." << std::endl;
    }

    if (teto_violado) {
        std::cout << "[ALERTA] Regra 2: Encontrado pelo menos um salário acima do teto de R$ " << TETO_SALARIAL << std::endl;
    } else {
        std::cout << "[OK] Regra 2: Nenhum salário acima do teto foi encontrado." << std::endl;
    }

    if (!dados_validos) {
        std::cout << "[ALERTA] Regra 3: Encontrados dados inválidos (salários negativos ou zero)." << std::endl;
    } else {
        std::cout << "[OK] Regra 3: Todos os registros de salário são válidos (positivos)." << std::endl;
    }

    return 0;
}
