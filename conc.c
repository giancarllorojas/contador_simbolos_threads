#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include<pthread.h>
#include "timer.h"
#include "buffer.c"

#define QT_SIMBOLOS 90 //quantidade de simbolos ASC que se enquadram nos critérios do problema { !, ?, @.... A, B ,C... , a , b c ...0,1,2... }

typedef struct simbolo{
	char valor;
	int ocorrencias;
}simbolo;

/** Variaveis globais **/
pthread_mutex_t mutex;
FILE *arq_entrada;
simbolo *vetor;
bool sinal = 1;


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
	int c;
	while ((c = fgetc(arq_entrada)) != EOF) {
		pthread_mutex_lock(&mutex);
		buffer_inserir(c);
		pthread_mutex_unlock(&mutex);
		//printf("%c", c);
    }
	sinal = 0; //sinaliza que terminou de ler o arquivo
}

/**
* Função que lê os simbolos do buffer e contabiliza no vetor
**/
void *contabiliza_arquivo(void *arg){
	int c;
	while (sinal) {
		if(!buffer_esta_vazio()){
			pthread_mutex_lock(&mutex);
			c = buffer_retirar(c);
			pthread_mutex_unlock(&mutex);
			contabiliza_simbolo(c);
			//printf("%c", c);
		}
    }
}

int main(int argc, char *argv[]){
	int c, t, *tid, nthreads;
	double inicio, fim, tempo;

	pthread_mutex_init(&mutex, NULL);

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
	pthread_create(&thread[0], NULL, contabiliza_arquivo, NULL);
	for(t = 1; t < nthreads; t++){
		tid = (int*) malloc(sizeof(int));
		*tid = t;
		pthread_create(&thread[t], NULL, le_arquivo, (void*)tid); 
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
