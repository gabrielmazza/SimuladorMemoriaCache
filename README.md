Simulador de Cache - README
Descrição
Este programa implementa uma simulação de cache, que permite avaliar o desempenho de diferentes políticas de substituição e escrita em um sistema de cache. Ele lê um arquivo de entrada contendo uma sequência de endereços de memória e operações (leitura ou escrita), e simula o comportamento da cache ao acessar esses endereços.

Estrutura do Código
O código é organizado em duas estruturas principais:

CacheLinha: Representa uma linha da cache, armazenando informações como tag, validade, dirty bit (indica se os dados foram modificados), a última vez que a linha foi usada (para LRU) e a frequência de uso (para LFU).
CacheConjunto: Representa um conjunto de linhas da cache (cache associativa), ou seja, um conjunto de linhas dentro de uma cache associativa por conjunto.
Cache: A cache em si, composta por múltiplos conjuntos e responsável por armazenar parâmetros como política de substituição, política de escrita, tamanho de linha, número de conjuntos, e tempo de hit.
Funcionalidades
Inicialização da Cache: A função inicializa_cache configura a cache de acordo com os parâmetros fornecidos, como número de linhas, associatividade, tamanho da linha, tempo de hit, política de escrita e política de substituição.

Simulação de Acesso à Cache: O código lê um arquivo de entrada que contém endereços e operações (leitura ou escrita) e simula o comportamento da cache ao acessar esses endereços. Para cada endereço, ele verifica se o dado já está na cache (hit) ou se precisa ser carregado da memória principal (miss).

Políticas de Escrita:

Write-through: Escreve na cache e imediatamente na memória principal.
Write-back: Escreve na cache e adia a escrita na memória principal até que a linha da cache precise ser substituída.
Políticas de Substituição:

LFU (Least Frequently Used): Substitui a linha da cache que foi menos utilizada.
LRU (Least Recently Used): Substitui a linha da cache que foi usada há mais tempo.
Random: Substitui uma linha da cache escolhida aleatoriamente.
Cálculo de Estatísticas: O programa calcula várias métricas, como:

Taxa de acerto de leitura e escrita (hit rate).
Número de leituras e escritas na memória principal.
Tempo médio de acesso à cache.
Arquivo de Entrada
O arquivo de entrada (ex: oficial.cache) deve conter endereços de memória em formato hexadecimal, seguidos por uma operação ('R' para leitura e 'W' para escrita), separados por espaços. Exemplo:

0x12345 R
0x54321 W

Parâmetros de Entrada
Os parâmetros configuráveis da cache são:

Tamanho total da cache: O tamanho total da cache em bytes.
Tamanho da linha: Tamanho de cada linha da cache (em bytes).
Associatividade: Número de linhas por conjunto (para cache associativa por conjunto).
Hit time: Tempo de acesso à cache em caso de acerto (em nanosegundos).
Política de escrita: Pode ser:
0: Write-through.
1: Write-back.
Política de substituição: Pode ser:
0: LFU.
1: LRU.
2: Substituição aleatória (Random).
Esses parâmetros são definidos no início da função main e podem ser ajustados para simulações diferentes.

Saída
O programa gera um arquivo de saída que contém:

Os parâmetros de configuração da cache.
O total de endereços processados.
O total de leituras e escritas.
As taxas de acerto (hit rate) de leitura, escrita e global.
O tempo médio de acesso à cache.
Compilação e Execução
Para compilar e executar o código:

gcc simulador_cache.c -o simulador_cache -lm ./simulador_cache

O nome do arquivo de saída é gerado automaticamente com base nos parâmetros da cache, como o tamanho, associatividade e política de escrita/substituição, e salvo no diretório de execução.

Exemplo de Execução
Ao rodar o programa, será gerado um arquivo de saída com um nome semelhante a este:

5_cache_tt16384_nl256_ass2_tl64_w-t_LRU.txt

Esse arquivo conterá as estatísticas da simulação, permitindo que você analise o desempenho da cache sob diferentes configurações.

Limpeza
Após a execução, a função liberar_cache libera a memória alocada para a cache, garantindo que não haja vazamentos de memória.

Este simulador pode ser ajustado e expandido para incluir novas políticas ou diferentes tamanhos e arquiteturas de cache para testes de desempenho em ambientes específicos.
