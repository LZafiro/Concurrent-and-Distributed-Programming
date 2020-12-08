
import java.util.*;

/*
Compilacao:
javac javaThreads.java
Execucao:
java javaThreads <TAMANHO> <GERACOES> <THREADS>
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
		
		tam = Integer.parseInt(args[0]);
		gen = Integer.parseInt(args[1]);
		MAX_THREAD = Integer.parseInt(args[2]);
	
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
					resp += rh[i].getResult();
				} catch(InterruptedException e){
					System.out.println("Exception");
				}
			}
			
			System.out.println("Geracao["+(j + 1)+"] = " + resp);
			trocarMatrizes();
		
			rh = null;
			th = null;//149488
		}
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

	private int THREAD_NUM, tam, thAtual;
	private int resul;
	int[][] old, neW;
	private int begin, end;
	
	public RunTHevolve(int THREAD_NUM, int tam, int thAtual, int[][] old, int[][] neW) {
		this.THREAD_NUM = THREAD_NUM;
		this.tam = tam;
		this.thAtual = thAtual;
		this.old = old;
		this.neW = neW;
		this.begin = (tam / THREAD_NUM)* thAtual;
		this.end = this.begin + (tam / THREAD_NUM);
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
		int i, j;
		this.resul = 0;
		
		for(i = this.begin; i < this.end; i++) {
			for(j = 0; j < tam; j++) {
				neW[i][j] = this.viverOuMorrer(old, i, j);
				this.resul += neW[i][j];
			}
		}
		
	}

	public int getResult(){
		return this.resul;
	}
}
