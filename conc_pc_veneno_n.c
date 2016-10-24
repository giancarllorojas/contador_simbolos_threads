#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "timer.h"

#define QT_SIMBOLOS 90 //quantidade de simbolos ASC que se enquadram nos critérios do problema { !, ?, @.... A, B ,C... , a , b c ...0,1,2... }
#define N_BUFFER 1000 //tamanho do buffer
#define N_CADEIA 1000

typedef struct simbolo{
	char valor;
	int ocorrencias;
}simbolo;

/** Variaveis globais **/
pthread_mutex_t mutex;
pthread_cond_t cond_leitor, cond_contador;
FILE *arq_entrada;
simbolo *vetor;
char *buffer[N_BUFFER];
int count=0, nthreads;

void insereBuffer (char *cadeia) {
	static int in = 0;
	char *cad = (char*)malloc(sizeof(char)*N_CADEIA);
	strcpy(cad, cadeia);
	pthread_mutex_lock(&mutex);
	while(count == N_BUFFER) {
		pthread_cond_wait(&cond_leitor, &mutex);
	}
	//printf("%c", item);
	buffer[in] = cad;
	count++;
	in = (in + 1)%N_BUFFER;
	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond_contador);
}

char *retiraBuffer () {
	static int out = 0; char *cadeia;
	pthread_mutex_lock(&mutex);
	while(count == 0) {
		pthread_cond_wait(&cond_contador, &mutex);
	}
	cadeia = buffer[out];
	//printf("%s", cadeia);
	count--;
	out = (out + 1)%N_BUFFER;
	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond_leitor);
	return cadeia;
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
* Contabiliza a ocorrência de um simbolo de uma cadeia de chars do texto, se ele tiver dentro dos critérios
**/
void contabiliza_cadeia_simbolos(char cadeia[], simbolo* vetor){
	int i;
	char simbolo;
	int asc_s;
	while((simbolo = cadeia[i]) != '\0'){
		asc_s = (int) simbolo;
		printf("%c", simbolo);
		if(asc_s > 33 && asc_s < 33+QT_SIMBOLOS){
			vetor[asc_s - 33].ocorrencias++;
		}
		i++;
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
	char cadeia[N_CADEIA];
	while(fgets (cadeia, 60, arq_entrada)!=NULL) {
		insereBuffer(cadeia);
		//printf("%s", cadeia);
	}
	printf("\nENDOFFILE\n");
	pthread_exit(NULL);
}

/**
* Função que lê os simbolos do buffer e contabiliza no vetor
**/
void *contabiliza_arquivo(void *arg){
	int c;
	char *cadeia;
	simbolo *vetor = instancia_vetor_simbolos();
	while(1) {
		cadeia = retiraBuffer();
		contabiliza_cadeia_simbolos(cadeia, vetor);
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
