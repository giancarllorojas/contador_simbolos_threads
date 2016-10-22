#define MAX 10000 //tamanho do buffer do produtor

char buffer[MAX];
int frente = 0;
int fim = -1;
int buffer_quantidade_simbolos = 0;

bool buffer_esta_vazio() {
   return buffer_quantidade_simbolos == 0;
}

bool buffer_esta_cheio() {
   return buffer_quantidade_simbolos == MAX;
}

int buffer_tamanho() {
   return buffer_quantidade_simbolos;
}  

void buffer_inserir(char simbolo) {

   if(!buffer_esta_cheio()) {
	
      if(fim == MAX-1) {
         fim = -1;            
      }       

      buffer[++fim] = simbolo;
      buffer_quantidade_simbolos++;
   }
}

int buffer_retirar() {
   char simbolo = buffer[frente++];
	
   if(frente == MAX) {
      frente = 0;
   }
	
   buffer_quantidade_simbolos--;
   return simbolo;  
}

