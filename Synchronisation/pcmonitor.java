import java.util.concurrent.Semaphore;

class pcmonitor
{
	static int nb, f, r, pc, cc, mi;
	static int buff[];
	static Semaphore mutex, full, empty;

	// Initialize variables and semaphores
	static void init()
	{
		cc = 0;
		pc = 0;
		f = 0;
		r = 0;
		mi = 100; // Maximum items to produce/consume
		nb = 5; // Buffer size
		buff = new int[nb];
		mutex = new Semaphore(1); // Mutex for critical section
		full = new Semaphore(0); // Count of filled slots
		empty = new Semaphore(nb); // Count of empty slots
	}

	// Producer class
	static class Prod implements Runnable
	{
		public void run()
		{
			try
			{
				int item = 1;
				while (pc < mi)
				{
					empty.acquire(); // Wait if buffer is full
					mutex.acquire(); // Enter critical section

					buff[r] = item;
					System.out.printf("Item produced : %d\n", item);
					r = (r + 1) % nb; // Update buffer index
					item++;
					pc++;

					mutex.release(); // Exit critical section
					full.release(); // Signal consumer
				}
			}
			catch (Exception e) { System.out.println(e); }
		}
	}

	// Consumer class
	static class Cons implements Runnable
	{
		public void run()
		{
			try
			{
				while (cc < mi)
				{
					full.acquire(); // Wait if buffer is empty
					mutex.acquire(); // Enter critical section

					System.out.printf("Item consumed : %d\n", buff[f]);
					f = (f + 1) % nb; // Update buffer index
					cc++;

					mutex.release(); // Exit critical section
					empty.release(); // Signal producer
				}
			}
			catch (Exception e) { System.out.println(e); }
		}
	}

	public static void main(String args[])
	{
		Thread p, c;
		Prod pr = new Prod();
		Cons co = new Cons();
		p = new Thread(pr);
		c = new Thread(co);

		p.start(); // Start producer thread
		c.start(); // Start consumer thread
	}
}