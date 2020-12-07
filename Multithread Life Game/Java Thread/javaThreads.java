package JavaThreads;

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
	
		System.out.println("Condição Inicial: " + gerarMatriz(old) + " vivos.");
		
        long start = System.currentTimeMillis(); // O 11

		for(j = 0; j < gen; j++) {		// O 2000
			th = new Thread[MAX_THREAD];
			rh = new RunTHevolve[MAX_THREAD];
			for(i = 0; i < MAX_THREAD; i++) { // O nt
				rh[i] = new RunTHevolve(MAX_THREAD, tam, i, old, neW); 
				th[i] = new Thread(rh[i]);
				th[i].run();
			}
			resp = 0;
			try {
				for(i = 0; i < MAX_THREAD; i++) {   //O nT
					th[i].join();
					resp += rh[i].resul;
				}
			} catch(InterruptedException e){
				System.out.println("Exception");
			}
			System.out.println("Geração["+(j + 1)+"] = " + resp);
			trocarMatrizes();
		
			rh = null;
			th = null;//149488
		}
		sc.close();
		System.out.println("Tempo de execução:"+(System.currentTimeMillis() - start)/1000 + "." + (System.currentTimeMillis() - start)%1000 + " segundos");
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
