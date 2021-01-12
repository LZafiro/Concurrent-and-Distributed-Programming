import java.util.concurrent.Semaphore;

public class TrafficController {
    public Semaphore sem = new Semaphore(1, true);

//red car entrando na ponte
//bloqueia a ponte para o carro azul
    public void enterLeft(){
         try { 
            sem.acquire();
		} catch (InterruptedException e){}
    
    }

    // blue car entrando na ponte
    // bloqueia a ponte para carro vermelho
    public void enterRight()  {
    try {
		sem.acquire();
	} catch (InterruptedException e){}
  
    }

//blue car saindo da ponte
//libera a ponte 
    public void leaveLeft() {
        sem.release();
    }
    
//red car saindo da ponte
//libera a ponte
    public void leaveRight() {
        sem.release();
    }
}