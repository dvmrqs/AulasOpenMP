/*-------------------------------
Neste arquivo, estão somente comentários
sobre sincronização com OpenMP.

Os códigos exemplos estão
distribuídos em outros arquivos.
começando pelo arquivo 006_sincronizacao_0.1.cpp

----------------------------------------------------*/

/*--------------------------------------------------------------
Sincronização com OpenMP - conceitos
1. Pragma omp critical
2. Pragma omp atomic
3. Pragma omp barrier
4. Pragma omp ordered
5. omp_set_lock
6. omp_unset_lock
7. omp_init_lock
8. omp_destroy_lock
--------------------------------------------------------------*/
//a sincronização serve para controlar o acesso simultâneo, garantindo consistência
//e ordem na execução de threads.
/*--------------------------------------------------------------
pragma omp critical
------------------------------------------------*/
// A diretiva `#pragma omp critical` é usada para definir uma seção crítica,
// garantindo que apenas uma thread possa executar o bloco de código associado
// Isto é, permite que apenas uma thread por vez exetue o trecho de código dentro da região critical.
//Isso é útil para proteger seções onde há escrita em uma variável compartilhada.

//Exemplo:
#pragma omp parallel
{

    #pragma omp critical
    {
        // só uma thread por vez entra aqui.
        total += valor; // variável compartilhada
    }
}

// sem o critical, duas threads poderiam atualizar o total ao mesmo tempo e sobrescrever resultados.
// com critical, cada uma espera a outra terminar antes de entrar na seção crítica.

/*--------------------------------------------------------------
pragma omp atomic
----------------------------------------------------------------*/
//protege operações simples de leitura/moidificação/escrita em uma variável.
//Ela é recomentada quando queremos garantir segurança em operações simples, como +=, ++, --, etc.
// sem o custo de um bloco critical completo.
//Exemplo:
#pragma omp parallel for    
for (int i = 0; i < N; i++){
    #pragma omp atomic
    soma += 1; // operação atômica segura
}
// atomic é mais rápido que critical, mas só serve para expressões simples.


/*-----------------------------------------------------------------
Pragma omp barrier
------------------------------------------------------------------*/
// força todas as threads esperarem umas pelas outras antes de seguir para o próximo trecho de código.
//exemplo
#pragma omp parallel
{
    faz_etapa1();

    #pragma omp barrier // todas as threads esperam aqui    
    // código que depende do trabalho anterior
    faz_etapa2();

}
// útil quando uma etapa depende do resultado da etapa anterior.



/*---------------------------------------------------------------
pragma omp ordered
-----------------------------------------------------------------*/
//garante que uma parte do código dentro de um laço paralelo seja executada na ordem original
//das iterações.
//exemplo:
#pragma omp for ordered
for (int i = 0; i < N; i++) {
    #pragma omp ordered
    {
        // código que deve ser executado na ordem
        std::cout << "i = " << i << "\n";  // imprime na ordem correta
    }
}
// serve para manter a sequência de saída, logs ou resultados.

/*---------------------------------------------------------------
omp_set_lock, omp_unset_lock, omp_init_lock, omp_destroy_lock
-----------------------------------------------------------------*/
// Essas funções são usadas para criar e gerenciar locks (bloqueios) em OpenMP.
// Elas oferecem um controle mais fino sobre a sincronização em comparação com as diretivas pragma.
// Exemplo de uso:
omp_lock_t lock;
omp_init_lock(&lock);

#pragma omp parallel
{
    omp_set_lock(&lock);
    // seção crítica
    omp_unset_lock(&lock);
}

omp_destroy_lock(&lock);    
// útil para cenários complexos onde critical ou atomic não são suficientes.
// locks permitem controle explícito sobre quando uma thread pode entrar ou sair de uma seção crítica.
// mas exigem mais cuidado para evitar deadlocks (quando duas ou mais threads ficam esperando indefinidamente uma pela outra).
// diferente de critial, locks não são automáticos, o programador deve garantir que cada set_lock tenha um unset_lock correspondente.
// sempre inicialize e destrua locks para evitar vazamentos de memória.


/*---------------------------
vamos aos exemplos práticos
-----------------------------------*/