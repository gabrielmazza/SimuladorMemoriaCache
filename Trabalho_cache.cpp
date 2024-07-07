#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct {
    unsigned int tag;
    int valido; //
    int dirty; //dados na linha da cache foram modificados apos terem sido carregados da MP
    int ultima_vez_usado; //para implementar substituicao como LRU
    int frequencia; //para implementar substituicao como LFU
} CacheLinha;

typedef struct {
    CacheLinha *linhas;
} CacheConjunto;

typedef struct {
    CacheConjunto *conjuntos;
    int numConjuntos;
    int linhas_por_conjunto;
    int tamanhoLinha;
    int hit_time;
    int politica_escrita; // 0 - write-through e 1 - write-back
    int politica_substituicao; // 0 - LFU, 1 - LRU, 2 - Random
} Cache;

void inicializa_cache(Cache *cache, int num_linhas, int associatividade, int tamanhoLinha, int hit_time, int politica_escrita, int politica_substituicao){
    cache->numConjuntos = num_linhas / associatividade;
    cache->linhas_por_conjunto = associatividade;
    cache->tamanhoLinha = tamanhoLinha;
    cache->hit_time = hit_time;
    cache->politica_escrita = politica_escrita;
    cache->politica_substituicao = politica_substituicao;
    cache->conjuntos = (CacheConjunto *)malloc(sizeof(CacheConjunto) * cache->numConjuntos);

     for (int i = 0; i < cache->numConjuntos; i++) {
        cache->conjuntos[i].linhas = (CacheLinha *)malloc(sizeof(CacheLinha) * cache->linhas_por_conjunto);
        for (int j = 0; j < cache->linhas_por_conjunto; j++) {
            cache->conjuntos[i].linhas[j].valido = 0;
            cache->conjuntos[i].linhas[j].dirty = 0;
            cache->conjuntos[i].linhas[j].ultima_vez_usado = 0;
            cache->conjuntos[i].linhas[j].frequencia = 0;
        }
    }
}

void liberar_cache(Cache *cache) {
    for (int i = 0; i < cache->numConjuntos; i++) {
        free(cache->conjuntos[i].linhas);
    }
    free(cache->conjuntos);
}

int main() {

    // Parâmetros de entrada
    int tamanho_total_cache =  16 * 1024;
    int tamanhoLinha = 64;
    int num_linhas = tamanho_total_cache / tamanhoLinha;    
    // int num_linhas = 16;
    // int tamanho_total_cache = num_linhas * tamanhoLinha;
    int associatividade = 2;
    int hit_time = 10;
    int politica_escrita = 0; // 0 - write-through, 1 - write-back
    int politica_substituicao = 1; // 0 - LFU, 1 - LRU, 2 - Random

    // Nome do arquivo de saída
    int numTeste = 5;
    char NomeArquivoSaida[100];
    sprintf(NomeArquivoSaida, "%d_cache_tt%d_nl%d_ass%d_tl%d_%s_%s.txt", numTeste, tamanho_total_cache, num_linhas, associatividade, tamanhoLinha,(politica_escrita == 0 ? "w-t" : "w-b"), (politica_substituicao == 0 ? "LFU" : politica_substituicao == 1 ? "LRU" : "R"));

    // Inicializar a cache
    Cache cache;
    inicializa_cache(&cache, num_linhas, associatividade, tamanhoLinha, hit_time, politica_escrita, politica_substituicao);

    //parametros para calculo de saída
    // Calcular o total de endereços
    int total_enderecos = 0;
    int total_leituras = 0;
    int total_escritas = 0;

    // Calcular o total de leituras e escritas da memória principal
    int total_leituras_mp = 0;
    int total_escritas_mp = 0;

    // Calcular o número de hits
    int num_hits_leitura = 0;
    int num_hits_escrita = 0;

    // Calcular o tempo médio de acesso da cache
    double tempo_medio_acesso_cache = 0.0;


    // Abrir o arquivo para leitura
    FILE *arquivo = fopen("../oficial.cache", "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    // Ler o arquivo linha por linha
    char linha[100];

    while (fgets(linha, sizeof(linha), arquivo)) {
        // Extrair o endereço e a operação da linha
        unsigned int endereco;
        char operacao;
        sscanf(linha, "%x %c", &endereco, &operacao);

        // Atualizar o total de endereços
        total_enderecos++;

        // Atualizar o total de leituras e escritas
        if (operacao == 'R') {
            total_leituras++;
        } else if (operacao == 'W') {
            total_escritas++;
        }

        // Calcular o conjunto e a tag do endereço
        unsigned int conjunto = (endereco / cache.tamanhoLinha) % cache.numConjuntos;
        unsigned int tag = endereco / (cache.tamanhoLinha * cache.numConjuntos);

        // Procurar o endereço na cache
        int hit = 0;
        for (int i = 0; i < cache.linhas_por_conjunto; i++) {
            CacheLinha *linha_cache = &cache.conjuntos[conjunto].linhas[i];
            if (linha_cache->valido && linha_cache->tag == tag) {
                hit = 1;
                linha_cache->ultima_vez_usado++;
                linha_cache->frequencia++;

                if (operacao == 'W') {
                    if (cache.politica_escrita == 1) { // Write-back
                        linha_cache->dirty = 1;
                    }
                }
                break;
            }
        }

        //Se a politica foi write-through e operação é de escrita
        //incrementa memória principal

        //HIT
        if (hit) {
                // Atualizar o número de hits
            if (operacao == 'R') {
                    num_hits_leitura++;

            } else if (operacao == 'W') {
                    num_hits_escrita++;
                    if(cache.politica_escrita == 0) {
                        total_escritas_mp++;
                    }
            }
            continue;
        }

        int maior_ultima_vez_usado = 0;

        // MISS
        // Encontrar a linha para substituição
        int linha_substituir = 0;
        if (cache.politica_substituicao == 0) { // LFU
            int menor_frequencia = cache.conjuntos[conjunto].linhas[0].frequencia;

            for (int i = 1; i < cache.linhas_por_conjunto; i++) {
                if (cache.conjuntos[conjunto].linhas[i].frequencia < menor_frequencia) {
                    menor_frequencia = cache.conjuntos[conjunto].linhas[i].frequencia;
                    linha_substituir = i;
                }
            }
            
        } else if (cache.politica_substituicao == 1) { // LRU
            int menor_ultima_vez_usado = cache.conjuntos[conjunto].linhas[0].ultima_vez_usado;
            maior_ultima_vez_usado = cache.conjuntos[conjunto].linhas[0].ultima_vez_usado;

            for (int i = 1; i < cache.linhas_por_conjunto; i++) {
                if (cache.conjuntos[conjunto].linhas[i].ultima_vez_usado < menor_ultima_vez_usado) {
                    menor_ultima_vez_usado = cache.conjuntos[conjunto].linhas[i].ultima_vez_usado;
                    linha_substituir = i;
                }
                if(cache.conjuntos[conjunto].linhas[i].ultima_vez_usado > maior_ultima_vez_usado){
                    maior_ultima_vez_usado = cache.conjuntos[conjunto].linhas[i].ultima_vez_usado;
                }
            }

        } else if (cache.politica_substituicao == 2) { // Random
            linha_substituir = rand() % cache.linhas_por_conjunto;
        }

        // Substituir a linha na cache        
        CacheLinha *linha_cache = &cache.conjuntos[conjunto].linhas[linha_substituir];

        //escrita na memória principal caso a linha da cache esteja suja e a politica de escrita seja write-back
        if (linha_cache->dirty == 1){ 
            total_escritas_mp++;
        }

        //  ler da memória principal para salvar na cache, indepebdente da operação
        total_leituras_mp++;

        //atualiza para válido (simula trazer da memória principal para a cache)
        linha_cache->valido = 1;
        linha_cache->tag = tag;
        linha_cache->dirty = 0;
        linha_cache->ultima_vez_usado = ++maior_ultima_vez_usado ;
        linha_cache->frequencia++;

        // Realizar a operação de leitura ou escrita
        if (operacao == 'W') {
            // Escrever na memória principal
            if (cache.politica_escrita == 1) { // Write-back
                linha_cache->dirty = 1;
            } else if(cache.politica_escrita == 0) {
                //escrita na memória principal caso a politica de escrita seja write-through
                total_escritas_mp++;
            }
        }

    }

    //dump final, caso a politica de escrita seja write-back
    if(cache.politica_escrita == 1) {
        for (int i = 0; i < cache.numConjuntos; i++) {
            for (int j = 0; j < cache.linhas_por_conjunto; j++) {
                if(cache.conjuntos[i].linhas[j].dirty == 1){
                    total_escritas_mp++;
                }                
            }
        }
    }

    // Abrir o arquivo de saída
    // FILE *saida = fopen("saida2_16.txt", "w");
    FILE *saida = fopen(NomeArquivoSaida, "w");

    if (saida == NULL) {
        printf("Erro ao abrir o arquivo de saída.\n");
        return 1;
    }

    // Escrever os parâmetros de entrada
    fprintf(saida, "Parâmetros de entrada:\n");
    fprintf(saida, "Número de linhas: %d\n", num_linhas);
    fprintf(saida, "Associatividade: %d\n", associatividade);
    fprintf(saida, "Tamanho da linha: %d\n", tamanhoLinha);
    fprintf(saida, "Hit time: %d\n", hit_time);
    fprintf(saida, "Política de escrita: %s\n", politica_escrita == 0 ? "write-through" : "write-back");
    fprintf(saida, "Política de substituição: %s\n", politica_substituicao == 0 ? "LFU" : politica_substituicao == 1 ? "LRU" : "Random");

    // Calcular a taxa de acerto
    double taxa_acerto_leitura = (double)num_hits_leitura / total_leituras * 100.0;
    double taxa_acerto_escrita = (double)num_hits_escrita / total_escritas * 100.0;
    double taxa_acerto_global = (double)(num_hits_leitura + num_hits_escrita) / total_enderecos * 100.0;

    // Escrever as informações no arquivo de saída
    fprintf(saida, "\nInformações da simulação:\n");
    fprintf(saida, "Total de endereços no arquivo de entrada: %d\n", total_enderecos);
    fprintf(saida, "Total de escritas: %d\n", total_escritas);
    fprintf(saida, "Total de leituras: %d\n", total_leituras);
    fprintf(saida, "Total de escritas da memória principal: %d\n", total_escritas_mp);
    fprintf(saida, "Total de leituras da memória principal: %d\n", total_leituras_mp);
    fprintf(saida, "Taxa de acerto (leitura): %.4f%% (%d)\n", taxa_acerto_leitura, num_hits_leitura);
    fprintf(saida, "Taxa de acerto (escrita): %.4f%% (%d)\n", taxa_acerto_escrita, num_hits_escrita);
    fprintf(saida, "Taxa de acerto (global): %.4f%% (%d)\n", taxa_acerto_global, num_hits_leitura + num_hits_escrita);
    fprintf(saida, "Tempo médio de acesso da cache (formula): %.4f ns\n",  (10 + ((1 - (taxa_acerto_global/100)) * 60)));

    // Fechar o arquivo de saída
    fclose(saida);

    // Fechar o arquivo entrada
    fclose(arquivo);

    liberar_cache(&cache);

    return 0;


}
