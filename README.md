# ThreadPool
Simple thread pool examples

The main points are:
1. A simple thread pool to add void functions(task).
2. thread_pool constructor will add threads. Number of threads is same as hardware_concurrency. 
3. Destructor will perform join on threads.
4. Vector\<thread\> is used to manage threads 
5. Queue\<std::function()\> for task to be performed. Main thread will add tasks to the queue. Child threads will pick up the task and execute them.
