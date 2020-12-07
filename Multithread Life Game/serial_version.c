#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define SRAND_VALUE 1985

int tam;
int ger;

#define ESQ(i) (i + tam - 1) % tam
#define DIR(i) (i + 1) % tam
#define CIMA(i) (i + tam - 1) % tam
#define BAIXO(i) (i + 1) % tam


int ** old = NULL;
int ** new = NULL;



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


int Evoluir(int** old, int** new)
{
	int i, j, c = 0;

	for (i = 0; i < tam; i++)
	{
		for (j = 0; j < tam; j++)
		{
			new[i][j] = ViverOuMorrer(old, i, j);
			c += new[i][j];
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
	
	inicializacao();
	
    gettimeofday(&inicio, NULL);

	for (int i = 0; i < ger; i++)
	{

		totalVivos = Evoluir(old, new);

		printf("Geracao %d: %d\n", i + 1, totalVivos);

		trocar_matrizes(&old, &new);
	}

    gettimeofday(&final, NULL);
    TempoDecorrido(inicio, final);
	return 0;
}