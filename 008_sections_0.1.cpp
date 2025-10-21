/*-------------------------------------------------------------------------------------------------------------------------
 * File Name : 008_sections_0.0.cpp
 * Author    : Prof. Rodrigo Gonçalves Pinto
 * Institution: University of Brasília (UnB) / University of São Paulo (USP) /IESB /SENAC
 * Course    : Parallel and Distributed Programming
 * Objective :  Demonstration of OpenMP sections for parallel execution of independent tasks
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
Exemplo: Simulação de inicialização de sistema
-----------------------------------------------------
Imaginem que estamos desenvolvendo um software grande, 
como um jogo ou um sistema de análise de dados.
Quando o programa inicia, ele precisa realizar várias tarefas de preparação que demoram um certo tempo:

1.  Carregar dados de um banco de dados.
2.  Conectar-se a um serviço externo na rede uma API, por exemplo.
3.  Inicializar um sistema de cache em memória.
4.  Ler e processar arquivos de configuração do disco.

Essas quatro tarefas são independentes umas das outras. 
Não precisamos esperar o banco de dados carregar para começar a ler os arquivos de configuração.
Portanto, este é um caso de uso  para o paralelismo por tarefas com sections.

-------
O objetivo é reduzir o tempo total de "boot" do nosso programa.
Se cada tarefa levasse 2 segundos, a execução serial levaria 2 + 2 + 2 + 2 = 8 segundos.
Em paralelo, com 4 threads, o tempo total seria de aproximadamente 2 segundos
o que corresponde ao tempo da tarefa mais longa.
------
Vamos criar quatro funções, cada uma simulando uma dessas tarefas.
---



Neste script, cada função simulará uma tarefa de longa duração usando sleep_for e retornará true
se a "inicialização" for bem-sucedida.
O main usará sections para chamar as quatro funções em paralelo e, no final, 
verificará se todos os subsistemas estão prontos.

*/

#include <iostream>
#include <string>
#include <vector>
#include <chrono> // Para simular tempo
#include <thread> // Para a função sleep_for
#include <omp.h>

// Nossas 4 Funções de Inicialização
// Cada uma simula uma tarefa demorada e independente.

/**
 * Simula o carregamento de dados de um banco de dados.
 * return true se o carregamento foi bem-sucedido.
 */
bool carregarBancoDeDados() {
    int thread_id = omp_get_thread_num();
    std::cout << "[Thread " << thread_id << "] Iniciando carregamento do Banco de Dados... (demora 2s)\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "[Thread " << thread_id << "] >>> Banco de Dados CARREGADO.\n";
    return true;
}

/**
 *  Simula a conexão com um serviço de rede externo.
 * rturn true se a conexão foi estabelecida.
 */
bool conectarServicoExterno() {
    int thread_id = omp_get_thread_num();
    std::cout << "[Thread " << thread_id << "] Iniciando conexão com API externa... (demora 1.5s)\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    std::cout << "[Thread " << thread_id << "] >>> Conectado à API externa.\n";
    return true;
}

/**
 *  Simula a criação e o preenchimento de um cache em memória.
 *  true se o cache foi inicializado.
 */
bool inicializarCache() {
    int thread_id = omp_get_thread_num();
    std::cout << "[Thread " << thread_id << "] Iniciando inicialização do cache em memória... (demora 1s)\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "[Thread " << thread_id << "] >>> Cache PRONTO.\n";
    return true;
}

/**
 * Simula a leitura de múltiplos arquivos de configuração do disco.
 * true se os arquivos foram lidos com sucesso.
 */
bool lerArquivosDeConfiguracao() {
    int thread_id = omp_get_thread_num();
    std::cout << "[Thread " << thread_id << "] Iniciando leitura dos arquivos de configuração... (demora 0.5s)\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "[Thread " << thread_id << "] >>> Configurações LIDAS.\n";
    return true;
}


int main() {
    // Variáveis para guardar o status de cada subsistema
    bool db_ok = false;
    bool api_ok = false;
    bool cache_ok = false;
    bool config_ok = false;

    std::cout << "--- Iniciando a simulação de boot do sistema ---\n";
    double start_time = omp_get_wtime();

    // Cada 'section' é uma tarefa independente que o OpenMP atribuirá a uma thread.
    // Todas as seções dentro deste bloco podem ser executadas em paralelo.
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            db_ok = carregarBancoDeDados();
        }

        #pragma omp section
        {
            api_ok = conectarServicoExterno();
        }

        #pragma omp section
        {
            cache_ok = inicializarCache();
        }

        #pragma omp section
        {
            config_ok = lerArquivosDeConfiguracao();
        }
    } // Barreira implícita: o programa espera aqui até TODAS as 4 seções terminarem.

    double end_time = omp_get_wtime();
    double duration = end_time - start_time;

    std::cout << "\n--- Processo de boot finalizado --- \n";
    std::cout << "Tempo total de inicialização: " << duration << " segundos.\n";
    
    double tempo_serial_esperado = 2.0 + 1.5 + 1.0 + 0.5;
    std::cout << "Tempo serial estimado seria: " << tempo_serial_esperado << " segundos.\n";

    // Verificação final do status do sistema
    if (db_ok && api_ok && cache_ok && config_ok) {
        std::cout << "\n[SUCESSO] Todos os subsistemas foram inicializados corretamente. O programa pode continuar.\n";
    } else {
        std::cout << "\n[FALHA] Um ou mais subsistemas falharam na inicialização. O programa será encerrado.\n";
    }

    return 0;
}


/*
-----------------------------------------------
Observações sobre a execução do código:
-----------------------------------------------

1.  Ordem de Início e Fim: As mensagens "Iniciando..." 
aparecerão quase todas ao mesmo tempo. 
As mensagens ">>> CONCLUÍDO" aparecerão em uma ordem que depende de qual tarefa termina primeiro 
a de 0.5s provavelmente será a primeira, e a de 2s a última.
Cada uma será executada por uma Thread diferente.

2.  Tempo Total de Execução: O ponto mais importante. 
O tempo total será um pouco maior que o da tarefa mais demorada (2 segundos),
e não a soma de todos os tempos (5 segundos). 
Isso prova o ganho de desempenho do paralelismo de tarefas.

3.  Estrutura do Código: A separação das tarefas em funções torna o código dentro do bloco sections
 muito limpo e legível. 
 Cada section tem uma responsabilidade clara.
*/