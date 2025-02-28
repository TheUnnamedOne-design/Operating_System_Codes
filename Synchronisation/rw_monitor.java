import java.util.concurrent.Semaphore;

class rw_monitor {
    static Semaphore rl, wl; // Semaphores for readers and writers
    static int reading; // Count of active readers
    static int n;

    static void init() {
        n = 5; // Number of readers and writers
        rl = new Semaphore(n); // Controls reader access
        wl = new Semaphore(1); // Ensures mutual exclusion for writers
        reading = 0; // Initially, no readers are active
    }

    static class reader implements Runnable {
        public void run() {
            try {
                rl.acquire(); // Acquire access to reading section
                reading++;
                if (reading == 1) wl.acquire(); // First reader locks the writer
                rl.release(); // Release reader lock

                String s = Thread.currentThread().getName();
                System.out.println(s + " is reading");
                Thread.sleep(1500);
                System.out.println(s + " has finished reading");

                rl.acquire();
                reading--;
                if (reading == 0) wl.release(); // Last reader unlocks the writer
                rl.release();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    static class writer implements Runnable {
        public void run() {
            try {
                wl.acquire(); // Ensure exclusive access for the writer
                String s = Thread.currentThread().getName();
                System.out.println(s + " is writing");
                Thread.sleep(1500);
                System.out.println(s + " has written");
                wl.release(); // Release the lock after writing
            } catch (Exception e) {
                System.out.println(e);
            }
        }
    }

    public static void main(String args[]) {
        init();
        Thread rds[] = new Thread[n];
        Thread wrs[] = new Thread[n];
        int i;
        reader rr = new reader();
        for (i = 0; i < n; i++) {
            String s = "Reader " + i;
            rds[i] = new Thread(rr);
            rds[i].setName(s);
        }
        writer wr = new writer();
        for (i = 0; i < n; i++) {
            String s = "Writer " + i;
            wrs[i] = new Thread(wr);
            wrs[i].setName(s);
        }
        for (i = 0; i < n; i++) {
            rds[i].start(); // Start all reader threads
            wrs[i].start(); // Start all writer threads
        }
    }
}