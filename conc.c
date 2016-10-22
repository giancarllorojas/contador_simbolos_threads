#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include<pthread.h>
#include "timer.h"

#define QT_SIMBOLOS 90 //quantidade de simbolos ASC que se enquadram nos critérios do problema { !, ?, @.... A, B ,C... , a , b c ...0,1,2... }

typedef struct simbolo{
	char valor;
	int ocorrencias;
}simbolo;


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
void contabiliza_simbolo(char simb, simbolo *vetor_simbolos){
    int asc_s = (int) simb;
	if(asc_s > 33 && asc_s < 33+QT_SIMBOLOS){
		vetor_simbolos[asc_s - 33].ocorrencias++;
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

int main(int argc, char *argv[]){
	simbolo *vetor;
	int c;
	double inicio, fim, tempo;
	if(argc < 3 ){
		printf("Execute %s <arquivo entrada> <arquivo saida>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	vetor = instancia_vetor_simbolos();

	FILE* arq_entrada = fopen(argv[1], "r");
	FILE* arq_saida   = fopen( argv[2], "w");

	GET_TIME(inicio);
	while ((c = fgetc(arq_entrada)) != EOF) {
        contabiliza_simbolo(c, vetor);
		//printf("%c", c);
    }
	GET_TIME(fim);

	tempo = fim - inicio;
	printf("tempo: %f\n", tempo);
	imprime_simbolos(vetor, arq_saida);
}
