#define MAX 1000000 //tamanho do buffer do produtor

char buffer[MAX];
int contador = 0;

bool buffer_esta_vazio() {
   if(topo == -1) 
		return 1;
   else 
		return 0;
}

bool buffer_esta_cheio() {
	if(topo == MAX)
		return 1;
	else
		return 0;
}

void buffer_inserir(char simbolo) {
   	static int in = 0;
	pthread_mutex_lock(&mutex);
	while(count == N) {
		pthread_cond_wait(&cond, &mutex);
	}
	buffer[in] = simbolo;
	contador++;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
	in = (in + 1)%N;
}

int buffer_retirar() {
	char simbolo;
	if(!buffer_esta_vazio()) {
	  	simbolo = buffer[topo];
	  	topo--;
		printf("r: %d\n", topo);   
	  	return simbolo;
	} else {
	  	printf("Could not retrieve data, Stack is empty.\n");
	}
}

