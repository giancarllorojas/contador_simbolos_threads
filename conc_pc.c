#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "timer.h"

#define QT_SIMBOLOS 90 //quantidade de simbolos ASC que se enquadram nos critérios do problema { !, ?, @.... A, B ,C... , a , b c ...0,1,2... }
#define N_BUFFER 1000 //tamanho do buffer

typedef struct simbolo{
	char valor;
	int ocorrencias;
}simbolo;

/** Variaveis globais **/
pthread_mutex_t mutex, mutex_contador;
pthread_cond_t cond_leitor, cond_contador;
FILE *arq_entrada;
simbolo *vetor;
char buffer[N_BUFFER];
int count=0, sinal_fim_arquivo = 0, sinal_fim_contados = 0, nthreads;

void insereBuffer (char item) {
	static int in = 0;
	pthread_mutex_lock(&mutex);
	while(count == N_BUFFER) {
		pthread_cond_wait(&cond_leitor, &mutex);
	}
	printf("%c", item);
	buffer[in] = item;
	count++;
	in = (in + 1)%N_BUFFER;
	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond_contador);
}

char retiraBuffer () {
	static int out = 0; char item;
	pthread_mutex_lock(&mutex);

	if(sinal_fim_arquivo && !count) {
		sinal_fim_contados = 1;
		printf("cheguei no fim: %i\n", sinal_fim_contados);
		return -1;
	}
	while(count == 0) {
		pthread_cond_wait(&cond_contador, &mutex);
	}
	item = buffer[out];
	count--;
	out = (out + 1)%N_BUFFER;
	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond_leitor);
	//printf("%c", item);
	return item;
}


/**
* Cria um vetor de simbolo possíveis para o problema
**/
simbolo* instancia_vetor_simbolos(){
	
	int i;
	simbolo *vetor = (simbolo*) malloc(QT_SIMBOLOS*sizeof(simbolo));

	for(i = 0; i < QT_SIMBOLOS; i++){
		vetor[i].valor = (char) i + 33; //pois os caracteres asc que nos interessam começam a partir do 33
		vetor[i].ocorrencias = 0;
		//printf(" simbolo asc %c  codigo asc %d indice: %d %d\n", vetor[i].asc_code,vetor[i].asc_code, i, vetor[i].ocorrencias);
	}
	
	return vetor; //retorna um vetor cujos elementos sao os caracteres asc que se enquadram no problema
}

/**
* Contabiliza a ocorrência de um simbolo no texto, se ele tiver dentro dos critérios
**/
void contabiliza_simbolo(char simb){
    int asc_s = (int) simb;
	if(asc_s > 33 && asc_s < 33+QT_SIMBOLOS){
		pthread_mutex_lock(&mutex_contador);
		vetor[asc_s - 33].ocorrencias++;
		pthread_mutex_unlock(&mutex_contador);
	}
}

/**
* Imprime a saída no arquivo de saída
**/
void imprime_simbolos(simbolo *vetor_simbolos, FILE* arq){
	int i;
	fprintf(arq, "Simbolo, Quantidade\n");
	for( i = 0; i < QT_SIMBOLOS;i++)
		if(vetor_simbolos[i]. ocorrencias != 0)
			fprintf(arq, " %c, %d\n", vetor_simbolos[i].valor, vetor_simbolos[i].ocorrencias);
	fclose(arq);
}

/**
* Função que lê os simbolos do arquivo e joga pro buffer
**/
void *le_arquivo(void *arg){
	int item, c, i;
	while((c = fgetc(arq_entrada)) != EOF) {
		insereBuffer(c);
	}
	sinal_fim_arquivo = 1;
	printf("\nENDOFFILE: %i\n", sinal_fim_arquivo);
	pthread_exit(NULL);
}

/**
* Função que lê os simbolos do buffer e contabiliza no vetor
**/
void *contabiliza_arquivo(void *arg){
	int c;
	int item;
	while((item = retiraBuffer())) {
		//printf("count: %i\n", count);
		//printf("item: %i\n", item);
		if(sinal_fim_contados) {
			//printf("cheguei mesmo\n");
			break;
		}
		contabiliza_simbolo(item);
	}
	
}

int main(int argc, char *argv[]){
	int c, t, *tid;
	double inicio, fim, tempo;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond_leitor, NULL);
	pthread_cond_init(&cond_contador, NULL);

	if(argc < 4 ){
		printf("Execute %s <arquivo entrada> <arquivo saida> <número threads>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if(atoi(argv[3]) < 2){
		printf("Número minimo de threads é 2\n");
		exit(EXIT_FAILURE);
	}

	nthreads = atoi(argv[3]);
	pthread_t thread[nthreads];

	vetor = instancia_vetor_simbolos();
	arq_entrada = fopen(argv[1], "r");
	FILE* arq_saida   = fopen( argv[2], "w");

	GET_TIME(inicio);
	pthread_create(&thread[0], NULL, le_arquivo, NULL);
	for(t = 1; t < nthreads; t++){
		tid = (int*) malloc(sizeof(int));
		*tid = t;
		pthread_create(&thread[t], NULL, contabiliza_arquivo, (void*)tid); 
	}

	for (t = 0; t < nthreads; t++) {
		 pthread_join(thread[t], NULL);
  	}
	GET_TIME(fim);

	fclose(arq_entrada);

	tempo = fim - inicio;
	printf("tempo: %f\n", tempo);
	imprime_simbolos(vetor, arq_saida);
}
