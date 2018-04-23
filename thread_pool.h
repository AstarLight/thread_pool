//
// Created by lijunshi on 18-4-18.
//

#include <queue>
#include <unistd.h>
#include <pthread.h>

#ifndef THREAD_POOL_THREAD_POOL_H
#define THREAD_POOL_THREAD_POOL_H

class Task
{
public:
    Task(void* a = NULL): arg(a)
    {

    }

    void SetArg(void* a)
    {
        arg = a;
    }

    virtual int run()=0;

protected:
    void* arg;

};

class ThreadPool
{
private:
    std::queue<Task*> taskQueue;
    bool isRunning;
    pthread_t* pThreadSet;
    int threadsNum;
    pthread_mutex_t mutex;
    pthread_cond_t condition;

    //单例模式，保证全局线程池只有一个
    ThreadPool(int num=10);
    void createThreads();  //创建内存池
    void clearThreads();
    void clearQueue();
    static void* threadFunc(void* arg);
    Task* takeTask();  //工作线程获取任务

public:
    void addTask(Task* pTask);   //任务入队
    static ThreadPool* createThreadPool(int num=10);
    ~ThreadPool();
    int getQueueSize();
    int getThreadlNum();

};


#endif //THREAD_POOL_THREAD_POOL_H
