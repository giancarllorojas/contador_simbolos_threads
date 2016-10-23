#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include<pthread.h>
#include "timer.h"
//#include "buffer.c"

#define QT_SIMBOLOS 90 //quantidade de simbolos ASC que se enquadram nos critérios do problema { !, ?, @.... A, B ,C... , a , b c ...0,1,2... }
#define N_BUFFER 200 //tamanho do buffer

typedef struct simbolo{
	char valor;
	int ocorrencias;
}simbolo;

/** Variaveis globais **/
pthread_mutex_t mutex;
pthread_cond_t cond;
FILE *arq_entrada;
simbolo *vetor;
char buffer[N_BUFFER];
bool sinal = 0;
int count=0;
int nthreads;

void insereBuffer (char item) {
	static int in = 0;
	pthread_mutex_lock(&mutex);
	while(count == N_BUFFER) {
		//printf("i");
		pthread_cond_wait(&cond, &mutex);
	}
	//printf("%c", item);
	buffer[in] = item;
	count++;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
	in = (in + 1)%N_BUFFER;
}

char retiraBuffer () {
	static int out = 0; char item;
	pthread_mutex_lock(&mutex);
	while(count == 0) {
		//printf("r");
		pthread_cond_wait(&cond, &mutex);
	}
	item = buffer[out];
	count--;
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mutex);
	out = (out + 1)%N_BUFFER;
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
		vetor[asc_s - 33].ocorrencias++;
		//printf("c");
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

	int item, cont=0, c, i;
	while((c = fgetc(arq_entrada)) != EOF) {
		insereBuffer(c);
		cont++;
	}
	printf("h");
	for(i=0; i <= nthreads; i++) insereBuffer(EOF);
	//printf("terminei de ler");
	pthread_exit(NULL);
}

/**
* Função que lê os simbolos do buffer e contabiliza no vetor
**/
void *contabiliza_arquivo(void *arg){
	int c;

	int item, cont=0;
	while((item = retiraBuffer()) != EOF) {
		//printf("%c", item);
		cont++;
		contabiliza_simbolo(item);
	}
	//printf("exit thread");
	pthread_exit(NULL);
}

int main(int argc, char *argv[]){
	int c, t, *tid;
	double inicio, fim, tempo;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

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
