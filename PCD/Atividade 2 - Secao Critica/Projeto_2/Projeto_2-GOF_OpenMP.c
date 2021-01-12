#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>
#include <assert.h>

#define NUM_THREADS 1
#define SRAND_VALUE 1985
#define DIM 2048
#define LIFE_CYCLES 2001

int **grid, **newgrid;
int numCell;
double inicio, fim, acumulado;

int soma_vivos;

void lastCountReduction(){
    inicio = omp_get_wtime();
    soma_vivos = 0;

    #pragma omp for reduction(+:soma_vivos)
    for(int i = 0; i < DIM; i++)
        for(int j = 0; j < DIM; j++)
            if (grid[i][j] == 1)
                soma_vivos++;
    
    fim = omp_get_wtime();
    printf("\nTempo Reducao: %.5f\n", fim - inicio);
}

void lastCountCriticalSession(){
    inicio = omp_get_wtime();
    soma_vivos = 0;

    #pragma omp parallel for
            for(int i = 0; i < DIM; i++)
                for(int j = 0; j < DIM; j++)
                    #pragma omp critical
                    { 
                    if (grid[i][j] == 1)
                        soma_vivos++;
                    }
    
    fim = omp_get_wtime();
    printf("Tempo Sessao Critica: %.5f\n", fim - inicio);
}

void lastCountNormal(){
    inicio = omp_get_wtime();
    soma_vivos = 0;

    for(int i = 0; i < DIM; i++)
        for(int j = 0; j < DIM; j++)
            if (grid[i][j] == 1)
                soma_vivos++;

    fim = omp_get_wtime();
    printf("Tempo Normal: %.5f\n", fim - inicio);
}


int getNeighbors(int i, int j){
    int neighborNum = 0, z, w, aux;

    aux = grid[i][j];
    grid[i][j] = 0;

    for(int a = i-1; a <= i+1; a++){
        for(int b = j-1; b <= j+1; b++){
            if(a < 0){
                z = DIM-1;
            }else if(a >= DIM){
                z = 0;
            }else{
                z = a;
            }
            if(b < 0){
                w = DIM-1;
            }else if(b >= DIM){
                w = 0;
            }else{
                w = b;
            }

            // printf(" ->%d, %d<-", z, w);
            neighborNum += grid[z][w];
        }
    }
    grid[i][j] = aux;
    return neighborNum;
}

void setNextGen(){
    inicio = omp_get_wtime();
    
    #pragma omp parallel
    {
        #pragma omp for reduction(+:numCell)
            for(int i = 0; i < DIM; i++){
                for(int j = 0; j < DIM; j++){
                    if(grid[i][j] == 1){
                        if((getNeighbors(i, j) == 2) || (getNeighbors(i, j) == 3)){
                            newgrid[i][j] = 1;
                            numCell++;
                        }else{
                            newgrid[i][j] = 0;
                        }
                    }else{
                        if(getNeighbors(i, j) == 3){
                            newgrid[i][j] = 1;
                            numCell++;
                        }
                    }
                }
            }
        #pragma omp barrier
            #pragma omp parallel for
                for(int i = 0; i < DIM; i++)
                    for(int j = 0; j < DIM; j++)
                        grid[i][j] = newgrid[i][j];    
                
    }

    fim = omp_get_wtime();
    acumulado += (fim-inicio);
}

void runLife(){
    for(int i = 0; i < LIFE_CYCLES; i++){
        printf("Geracao %d: %d\n", i, numCell);
        numCell = 0;
        setNextGen();
    }

    lastCountReduction();
    lastCountCriticalSession();
    lastCountNormal();
}

int main(){

    srand(SRAND_VALUE);
    acumulado = 0;
    numCell = 0;

    omp_set_num_threads(NUM_THREADS);

    grid = malloc(DIM*sizeof(int *));
    newgrid = malloc(DIM*sizeof(int *));
    
    for(int i = 0; i < DIM; i++){
        grid[i] = malloc(DIM*sizeof(int));
        newgrid[i] = malloc(DIM*sizeof(int));
    }

    for(int i = 0; i < DIM; i++){
        for(int j = 0; j < DIM; j++){
            grid[i][j] = rand() % 2;
            numCell += grid[i][j];
        }
    }
    
    runLife();
    
    printf("\nOpenMP");
    printf("\nNumero de Threads: %d",NUM_THREADS);
    printf("\nTempo: %.2f\n", acumulado);

    return 0;
}