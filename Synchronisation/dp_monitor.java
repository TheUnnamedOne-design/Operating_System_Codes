import java.util.concurrent.Semaphore;

class dp_monitor
{
    static Semaphore plock[], mutex; // Array of semaphores for philosophers and a mutex for mutual exclusion
    static int states[]; // Array to track the state of each philosopher
    static int eating, hungry, thinking; // Constants representing philosopher states
    static int n; // Number of philosophers
    
    // Initialize variables and semaphores
    static void init()
    {
        n = 5; // Number of philosophers
        eating = 0;
        hungry = 1;
        thinking = 2;
        states = new int[n]; // Array to track philosopher states
        plock = new Semaphore[n]; // Semaphores for each philosopher
        for (int i = 0; i < n; i++) plock[i] = new Semaphore(0); // Initializing philosopher semaphores
        mutex = new Semaphore(1); // Binary semaphore for mutual exclusion
    }
    
    // Get left philosopher index
    static int left(int i)
    {
        return (i + n - 1) % n;
    }
    
    // Get right philosopher index
    static int right(int i)
    {
        return (i + 1) % n;
    }
    
    // Attempt to pick up forks
    static void take_fork(int i)
    {
        try
        {
            mutex.acquire(); // Enter critical section
            states[i] = hungry; // Philosopher is hungry
            System.out.printf("Philosopher %d is hungry\n", i);
            test(i); // Check if the philosopher can eat
            mutex.release(); // Exit critical section
            plock[i].acquire(); // Wait if forks are not available
        }
        catch(Exception e) 
        {
            e.printStackTrace();
        }
    }
    
    // Check if a philosopher can eat
    static void test(int i)
    {
        try
        {
            if (states[i] == hungry && states[left(i)] != eating && states[right(i)] != eating)
            {
                states[i] = eating; // Philosopher starts eating
                System.out.printf("Philosopher %d is eating\n", i);
                System.out.printf("Philosopher %d is picking up forks %d and %d\n", i, i, right(i));
                Thread.sleep(1000); // Simulate eating time
                plock[i].release(); // Allow the philosopher to proceed
            }
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }
    
    // Put down forks after eating
    static void put_fork(int i)
    {
        try
        {
            mutex.acquire(); // Enter critical section
            states[i] = thinking; // Philosopher starts thinking
            System.out.printf("Philosopher %d is putting down forks %d and %d\n", i, i, right(i));
            System.out.printf("Philosopher %d is thinking\n", i);
            test(left(i)); // Check if left neighbor can eat
            test(right(i)); // Check if right neighbor can eat
            mutex.release(); // Exit critical section
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }
    
    // Runnable class for philosopher threads
    static class phil implements Runnable
    {
        public void run()
        {
            while (true)
            {
                try
                {
                    String s = Thread.currentThread().getName(); // Get thread name (philosopher number)
                    int hold = Integer.parseInt(s); // Convert thread name to integer
                    take_fork(hold); // Attempt to pick up forks
                    Thread.sleep(1000); // Simulate eating
                    put_fork(hold); // Put down forks after eating
                }
                catch(Exception e)
                {
                    e.printStackTrace();
                }
            }
        }
    }
    
    public static void main(String args[])
    {
        init(); // Initialize semaphores and variables
        Thread pl[] = new Thread[n]; // Array of philosopher threads
        phil obj = new phil(); // Create a runnable object
        
        // Create philosopher threads
        for (int i = 0; i < n; i++)
        {
            pl[i] = new Thread(obj);
            String s = Integer.valueOf(i).toString();
            pl[i].setName(s); // Assign thread name as philosopher index
        }
        
        // Start philosopher threads
        for (int i = 0; i < n; i++) pl[i].start();
    }
}
