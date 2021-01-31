#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

/*
Compilacao:
mpicc -o mpi jogo_vida_mpi.c -O3
Execucao:
mpirun -n 4 ./mpi 2048 500
*/

#define SRAND_VALUE 1985

#define ESQ(i) (i + tam - 1) % tam
#define DIR(i) (i + 1) % tam
#define CIMA(i) (i + tam - 1) % tam
#define BAIXO(i) (i + 1) % tam

int ** old = NULL;
int ** new = NULL;

int tam;
int num_processos;

int remainderC(int a, int b){
	int mode = a%b;
	while(mode<0){
		mode = (b+mode)%b;
	}
	return mode;
}

void inicializacao ()
{
	// Cria matrizes dinamicamente
    old = (int**) malloc(tam * sizeof(int*));
	new = (int**) malloc(tam * sizeof(int*));
	int i;
    for (i = 0; i < tam; i++){
		old[i] = (int*)malloc(tam * sizeof(int));
		new[i] = (int*)malloc(tam * sizeof(int));
	}	
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

int Evoluir(int** old, int** new, int rank)
{
    //Alterar a funcao de evolucao para um range do processo
	int i, j, c = 0;

    for (i = (tam/num_processos)*rank; i < (tam/num_processos)*(rank+1); i++){
        for (j = 0; j < tam; j++){
            new[i][j] = ViverOuMorrer(old, i, j);
            c += new[i][j];
        }
    }

	return c;
}

void trocar_matrizes(int*** A, int*** B)
{
	int** aux = *A;
	*A = *B;
	*B = aux;
}

void enviar_receber(int **grid, int rank, int mode, int ger){
	MPI_Request send_request, recv_request;
	MPI_Status send_status, recv_status;
	int recv_position, send_position, send_rank, recv_rank;
	if(mode==1){//Enviar para o proximo e receber do anterior
		recv_position = remainderC((tam/num_processos)*rank-1, tam);//Antes da sua primeira posicao
		send_position = (tam/num_processos)*(rank+1)-1;//Sua ultima posicao
		recv_rank = remainderC(rank-1,num_processos);
		send_rank = remainderC(rank+1,num_processos);
	}
	else{//Enviar para o anterior e receber do proximo
		recv_position = remainderC((tam/num_processos)*(rank+1), tam);//Depois da ultima posicao
		send_position = (tam/num_processos)*rank;//Sua primeira posicao
		recv_rank = remainderC(rank+1,num_processos);
		send_rank = remainderC(rank-1,num_processos);
	}
	MPI_Isend(
		grid[send_position],
		tam,
		MPI_INT,
		send_rank,
		ger*2+1+mode,
		MPI_COMM_WORLD,
		&send_request
	);
	MPI_Irecv(
		grid[recv_position],
		tam,
		MPI_INT,
		recv_rank,
		ger*2+1+mode,
		MPI_COMM_WORLD,
		&recv_request
	);
	MPI_Wait(
		&send_request,
		&send_status
	);
	MPI_Wait(
		&recv_request,
		&recv_status
	);
}

void TempoDecorrido(struct timeval inicio, struct timeval fim){
	int tmili = (int) (1000*(fim.tv_sec - inicio.tv_sec)+
                    (fim.tv_usec - inicio.tv_usec)/1000);
	int segundos = tmili/1000;
	int milisegundos = tmili-segundos*1000;
	printf("\nTempo: %d segundos %d milisegundos\n", segundos, milisegundos);
}

int main(int argc, char **argv){
	struct timeval inicio, final;
    MPI_Init(NULL, NULL);

    int world_rank;
    tam = atoi(argv[1]);
    int ger = atoi(argv[2]);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processos);
    int i, j, z;
    int local_sum, global_sum;

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    inicializacao();

    global_sum = GerarMatriz(old);

    if(world_rank==0){
        printf("Condicao inicial: %d\n", global_sum);
		gettimeofday(&inicio, NULL);
	}

    for(i=0;i<ger;i++){
        local_sum = Evoluir(old, new, world_rank);
        MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        if(world_rank==0){
            printf("Geracao %d: %d\n", i + 1, global_sum);
        }
        trocar_matrizes(&old, &new);
        if(num_processos>1){
			enviar_receber(old, world_rank, 1, i);
			enviar_receber(old, world_rank, -1, i);
		}
    }
	MPI_Barrier(MPI_COMM_WORLD);
	if(world_rank==0){
		gettimeofday(&final, NULL);
		TempoDecorrido(inicio, final);
	}
    return 0;
}