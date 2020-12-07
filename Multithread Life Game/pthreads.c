#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

/*
Compilacao:
gcc -o ex pthreads.c -pthread -O3
Execucao:
./ex 2048 2000 4
*/


#define SRAND_VALUE 1985


int tam;
int ger;
int num_threads;


#define ESQ(i) (i + tam - 1) % tam
#define DIR(i) (i + 1) % tam
#define CIMA(i) (i + tam - 1) % tam
#define BAIXO(i) (i + 1) % tam


int ** old = NULL;
int ** new = NULL;


void print(int** m){
	for(int i = 0; i < tam; i++){
		for(int j = 0; j < tam; j++){
			printf("%d ", m[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}


int GerarMatriz(int ** grid)
{
	int i, j, c = 0;

	srand(SRAND_VALUE);
	for (i = 0; i < tam; i++) 
	{ 
		//laço sobre as células do tabuleiro sem contar com um eventual halo
		for (j = 0; j < tam; j++) 
		{
			grid[i][j] = rand() % 2;
			c += grid[i][j];
		}
	}
	return c;
}


void inicializacao ()
{
	// Cria matrizes dinamicamente
	old = (int**) malloc(tam * sizeof(int*));
	new = (int**) malloc(tam * sizeof(int*));
	int i;
	for (i = 0; i < tam; i++)
        {
		old[i] = (int*)malloc(tam * sizeof(int));
		new[i] = (int*)malloc(tam * sizeof(int));
	}

	// Gera matriz e demonstra condicao inicial
	printf("Condicao inicial: %d\n", GerarMatriz(old));
}


void trocar_matrizes(int*** A, int*** B)
{
	int** aux = *A;
	*A = *B;
	*B = aux;
}


int getNeighbors(int** grid, int i, int j)
{
	// Soma todos os vizinhos
	return (
		grid[CIMA(i)][ESQ(j)] + grid[CIMA(i)][j] + grid[CIMA(i)][DIR(j)]
		) + (
		grid[i][ESQ(j)] + grid[i][DIR(j)]
		) + (
		grid[BAIXO(i)][ESQ(j)] + grid[BAIXO(i)][j] + grid[BAIXO(i)][DIR(j)]
		);
}


int ViverOuMorrer(int** grid, int i, int j)
{
	//Captura o número de vizinhos vivos
	int vizinhosVivos = getNeighbors(grid, i, j);

	if (grid[i][j] == 1) // Se vivo
	{
		// Morte por poucos ou muitos vizinhos
		if (vizinhosVivos < 2 || 4 <= vizinhosVivos)
			return 0;
		else
			return 1;

	}
	else // Se morto
	{
		// Vive, se tiver a quantidade correta de vizinhos
		if (vizinhosVivos == 3)
			return 1;
		else
			return 0;
	}
}


void* Evoluir(void* param)
{ 
	int c = 0, soma_parcial;
	int tid = (int) param;


	for (int i = tid * tam / num_threads; i < (tid * tam / num_threads) + tam / num_threads; i++){
		soma_parcial = 0;
		for (int j = 0; j < tam; j++)
		{
			new[i][j] = ViverOuMorrer(old, i, j);
			soma_parcial += new[i][j];
		}
		c += soma_parcial;
	}
	pthread_exit((void*)c);
}


void TempoDecorrido(struct timeval inicio, struct timeval fim){
	int tmili = (int) (1000*(fim.tv_sec - inicio.tv_sec)+
                    (fim.tv_usec - inicio.tv_usec)/1000);
	int segundos = tmili/1000;
	int milisegundos = tmili-segundos*1000;
	printf("\nTempo: %d segundos %d milisegundos\n", segundos, milisegundos);
}


int main(int argc, char **argv) 
{
	struct timeval inicio, final;
	tam = atoi(argv[1]);
	ger = atoi(argv[2]);
	num_threads = atoi(argv[3]);
	
	/* Instantiate thread array, parameter and return status*/
	pthread_t thread[num_threads];
  
	inicializacao();

	void* status;
	int curentAlive;
	int rc;
	
	gettimeofday(&inicio, NULL);

	for(int i = 0; i < ger; i++){
		curentAlive = 0;
		/* Threads creation*/
		for(int j = 0; j < num_threads; j++){
			rc = pthread_create(&thread[j], NULL, Evoluir, (void *)j);
			if(rc){
				printf("Error: create = %d\n", rc);
				exit(-1);
			}
		}
		
		for(int j = 0; j < num_threads; j++){
			rc = pthread_join(thread[j], &status);
			if(rc){
				printf("Error: join code = %d", rc);
				exit(-1);
			}
			curentAlive += (int)status;
		}		

		printf("Geracao %d: %d\n", i + 1, curentAlive);

		trocar_matrizes(&old, &new);
	}

	gettimeofday(&final, NULL);
	TempoDecorrido(inicio, final);

	//printf("Geracao %d: %d\n", ger, curentAlive);

	return 0;
}
