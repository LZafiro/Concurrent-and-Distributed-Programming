#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

/*
Compilacao:
gcc -o pthreads pthreads_critical.c -lpthread -O3
Execucao:
./pthreads 2048 2000 4
*/

#define SRAND_VALUE 1985

int tam;
int ger;

#define ESQ(i) (i + tam - 1) % tam
#define DIR(i) (i + 1) % tam
#define CIMA(i) (i + tam - 1) % tam
#define BAIXO(i) (i + 1) % tam

int ** old = NULL;
int ** new = NULL;
int num_threads;
pthread_t *thread = NULL;
int * vet = NULL;

typedef struct Tthread
{
	int i;		// Linha
	int def;	// Deslocamento diferencial
	int tr;		// Numero da Thread
} Tthread;


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
	vet = (int*) malloc (sizeof(int) * num_threads);
	thread = (pthread_t*) malloc(sizeof(pthread_t) * num_threads);
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




void * Iterar(void * args)
{
	
	Tthread * arg = (Tthread*)(args);
	int j = 0;

	for (j = arg->def; j < ((tam/num_threads) + arg->def); j++)
	{
		new[arg->i][j] = ViverOuMorrer(old, arg->i, j);
		vet[arg->tr] += new[arg->i][j];
	}
	pthread_exit(NULL);
}

void instanciar(int i)
{
	
	int tr, x;
	Tthread* aux = NULL;

	for(tr = 0; tr < num_threads; tr++)
	{
		aux = malloc(sizeof(Tthread));
		aux->i = i;
		aux->def = ((tam/num_threads) * tr);
		aux->tr = tr;
		vet[tr] = 0;

		x = pthread_create(&(thread[tr]), NULL, Iterar, (void*)(aux) );

		if(x)
		{
			printf("Error: create = %d\n", x);
			exit(-1);
		}
	}
}

int Evoluir()
{
	
	int i, j, c = 0, rc;
	void* status;
		
	for (i = 0; i < tam; i++)
	{
		//printf("/nInstanciar Linha: %d", i + 1);
		instanciar(i);

		for(j = 0; j < num_threads; j++)
		{
			rc = pthread_join(thread[j], &status);
			if(rc)
			{
				printf("Error: join code = %d", rc);
				exit(-1);
			}		
			c += vet[j];
		}
	}

	return c;
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
	int totalVivos;
	
	tam = atoi(argv[1]);
	ger = atoi(argv[2]);
	num_threads = atoi(argv[3]);
	
	inicializacao();

	gettimeofday(&inicio, NULL);

	for (int i = 0; i < ger; i++)
	{
		totalVivos = Evoluir();
		printf("Geracao %d: %d\n", i + 1, totalVivos);

		trocar_matrizes(&old, &new);
	}

	gettimeofday(&final, NULL);
	TempoDecorrido(inicio, final);
	return 0;
}