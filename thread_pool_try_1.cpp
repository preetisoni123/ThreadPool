#include <thread>
#include <iostream>
#include <atomic>
#include <queue>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>

class thread_pool
{
    std::atomic<bool> done;
    std::mutex mu;
    
    std::queue<std::function<void()>> work_queue;
    std::vector<std::thread> threads;

    // joiner

    void worker_thread()
    {
        if(debug)
            std::cout << "\tWorker Thread " << std::this_thread::get_id() << " Wait for task pushed to work queue" << std::endl;
        while(!done)
        {
            std::function<void()> task;
            std::unique_lock<std::mutex> m(mu);
            if(!work_queue.empty())
            {
                if(debug)
                    std::cout << "\tWorker thread " << std::this_thread::get_id() << " : Adding a task" << std::endl;
                task = work_queue.front();
                work_queue.pop();
                m.unlock();
                task();
            }
        }
    }
public:
    bool debug;
    thread_pool():
    done(false),
    debug(false)
    {
        unsigned const thread_count = std::thread::hardware_concurrency();

        try
        {
            for(unsigned i = 0; i < thread_count; ++i)
            {
                if(debug)
                    std::cout << "Adding thread: " << i << std::endl;
                threads.push_back(
                    std::thread(&thread_pool::worker_thread, this)
                );   
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            done = true;
            throw;
        }
        
    }

    ~thread_pool()
    {
        done = true;
        for(auto &t: threads)
        {
            if(t.joinable())
                t.join();
        }
    }

    template<typename FunctionType>
    void submit(FunctionType f)
    {
        // Submit task
        if(debug)
            std::cout << "Main thread: submiting a function to the work queue" << std::endl;
        work_queue.push(std::function<void()>(f));
    }

};

void func1()
{
    std::cout << "\t\tWorker Thread: " << std::this_thread::get_id() << "*** I am func1 ***" << std::endl;
}

void func2()
{
    std::cout << "\t\tWorker Thread: " << std::this_thread::get_id() << "*** I am func2 ***" << std::endl;
    // Lets do some work
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main()
{
    thread_pool th;
    th.debug = true;
    if(th.debug)
        std::cout << "Main thread: Started the thread pool" << std::endl;

    th.submit(func1);
    th.submit(func2);
    th.submit(func1);
    th.submit(func2);

    std::this_thread::sleep_for(std::chrono::seconds(5));
    if(th.debug)
        std::cout << "\nMain thread: Exiting ..." << std::endl;
    return 0;
}
 