#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 2
#define CYCLES 10000000

int request = 0, respond = 0;
long local_cycles;
long SOMA = 0;

// Thread cliente
void *client(void *arg){
	long tid = (long) arg;
	long i, local;	

	for(i = 0; i < local_cycles; i++) {
		//while(respond != tid)
        //    request = tid;

        // Seção crítica
        SOMA = SOMA + 1;
        // local = SOMA;
        // sleep(rand()%2);
        // SOMA = local + 1;
		
		printf("Thread %ld - Soma %ld\n", tid, SOMA);

		respond = 0;
	}
	pthread_exit(NULL);
}

// Thread servidor
void *server(void *arg){
	while(1){
		while(request == 0){}
		respond = request;		
		while(respond != 0){}
		request = 0;
	}
	pthread_exit(NULL);
}

// Main 
int main(){

    pthread_t thread[NUM_THREADS];
	pthread_t thread_server;
    long tid; 

	local_cycles = CYCLES / NUM_THREADS; // Iterações de cada thread

    // Executa a thread do servidor
    pthread_create(&thread_server, NULL, server, NULL);

    // Executa a thread dos clientes
	for(tid = 0; tid < NUM_THREADS; tid++) 
		pthread_create(&thread[tid], NULL, client, (void*) tid+1);		
	
    // Linka as threads
	for(tid = 0; tid < NUM_THREADS; tid++)
		pthread_join(thread[tid], NULL);
	
	printf("Soma total = %ld\n", SOMA);
	return 0;
}