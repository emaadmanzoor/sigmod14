/* Retrofitted for GCC 4.4.x
 * from here: https://github.com/progschj/ThreadPool/
 */

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    ThreadPool(size_t);
    template<class F, class... Args>
    void enqueue(F&& f, Args&&... args);
    ~ThreadPool();
private:
    friend class Worker;
    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;
    // the task queue
    std::queue< std::function<void()> > tasks;
    
    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

class Worker {
  public:
     Worker(ThreadPool &s) : pool(s) { }
     void operator()();
  private:
     ThreadPool &pool;
};

void Worker::operator()()
{
    for (;;)
    {
        std::unique_lock<std::mutex> lock(pool.queue_mutex);
        while(!pool.stop && pool.tasks.empty())
            pool.condition.wait(lock);
        if(pool.stop && pool.tasks.empty())
            return;
        std::function<void()> task(pool.tasks.front());
        pool.tasks.pop();
        lock.unlock();
        task();
    }
}
 
// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
    :   stop(false)
{
    for(size_t i = 0;i<threads;++i)
        workers.push_back(std::thread(Worker(*this)));
}

// add new work item to the pool
template<class F, class... Args>
void ThreadPool::enqueue(F&& f, Args&&... args) 
{
    // don't allow enqueueing after stopping the pool
    if(stop)
        throw std::runtime_error("enqueue on stopped ThreadPool");

    auto task = std::make_shared< std::function<void()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        //tasks.push([task](){ (*task)(); });
        tasks.push(std::function<void()>(*task));
    }
    condition.notify_one();
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(size_t i = 0;i<workers.size();++i)
        workers[i].join();
}

#endif
