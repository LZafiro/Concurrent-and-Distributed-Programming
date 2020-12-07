
import java.util.*;

/*
  1 - 110.196
  2 - 89.383
  4 - 117.97
  8 - 159.192
 */
public class javaThreads {
	
	private static int[][] neW, old;
	private static int MAX_THREAD;
	private static int tam, gen; 
	
	public static void main(String args[]) {

		//int[][] old, neW;
		Thread[] th;
		RunTHevolve[] rh;
		int i, j, resp = 0;
		
		Scanner sc = new Scanner(System.in);
		
		// Gets the thread number and array size
		tam = sc.nextInt();
		gen = sc.nextInt();
		MAX_THREAD = sc.nextInt();
	
		old = new int[tam][tam];
		neW = new int [tam][tam];
	
		System.out.println("Condicao Inicial: " + gerarMatriz(old) + " vivos.");
		
        long start = System.currentTimeMillis();

		for(j = 0; j < gen; j++){
			th = new Thread[MAX_THREAD];
			rh = new RunTHevolve[MAX_THREAD];
			for(i = 0; i < MAX_THREAD; i++) {
				rh[i] = new RunTHevolve(MAX_THREAD, tam, i, old, neW); 
				th[i] = new Thread(rh[i]);
				th[i].start();
			}
			resp = 0;
			for(i = 0; i < MAX_THREAD; i++) {
				try {
					th[i].join();
					resp += rh[i].resul;
				} catch(InterruptedException e){
					System.out.println("Exception");
				}
			}
			
			System.out.println("Geracao["+(j + 1)+"] = " + resp);
			trocarMatrizes();
		
			rh = null;
			th = null;//149488
		}
		sc.close();
		System.out.println("Tempo de execucao:"+(System.currentTimeMillis() - start)/1000 + "." + (System.currentTimeMillis() - start)%1000 + " segundos");
	}
	
	private static int gerarMatriz(int[][] grid) {

		int i, j, c = 0;
		Random gerador = new Random(1985);
		
		for(i = 0; i < tam; i++) {
			for(j = 0; j < tam; j++) {
				grid[i][j] = gerador.nextInt(2147483647) % 2; 
				c += grid[i][j];
			}
		}
		
		return c;
	}
	
	private static void trocarMatrizes() {
		int[][] aux;
		aux = old;
		old = neW;
		neW = aux;
	}	
}

class RunTHevolve implements Runnable{

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
		
		//Captura o numero de vizinhos vivos
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
