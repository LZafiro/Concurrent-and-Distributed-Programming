package JavaThreads;

public class RunTHevolve implements Runnable{

	private static int THREAD_NUM, tam, thAtual;
	public int resul;
	int[][] old, neW;
	
	public RunTHevolve(int aTHREAD_NUM, int atam, int athAtual, int[][] aold, int[][] aneW) {
		THREAD_NUM = aTHREAD_NUM;
		tam = atam;
		thAtual = athAtual;
		old = aold;
		neW = aneW;		
	}
	
	private int CIMA(int i) {
		return(i + tam - 1) % tam;
	}
	
	private int BAIXO(int i) {
		return(i + 1) % tam; 
	}
	
	private int ESQ(int i) {
		return(i + tam - 1) % tam;
	}
	
	private int DIR(int i) {
		return(i + 1) % tam;
	}
	
	private int getNeighbors(int[][] grid, int i, int j) {
		return (
			grid[CIMA(i)][ESQ(j)] + grid[CIMA(i)][j] + grid[CIMA(i)][DIR(j)]
			) + (
			grid[i][ESQ(j)] + grid[i][DIR(j)]
			) + (
			grid[BAIXO(i)][ESQ(j)] + grid[BAIXO(i)][j] + grid[BAIXO(i)][DIR(j)]
			);
	}
	
	private int viverOuMorrer(int[][] grid, int i, int j) {
		
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
	
	public void run() { 
		int begin = tam / THREAD_NUM * thAtual;
		int reps = begin + tam / THREAD_NUM;
		int i, j;
		int soma_parcial;
		
		for(i = begin; i < reps; i++) {
			soma_parcial = 0;
			for(j = 0; j < tam; j++) {
				neW[i][j] = this.viverOuMorrer(old, i, j);
				soma_parcial += neW[i][j];
			}
			resul += soma_parcial;
		}
		
	}
}