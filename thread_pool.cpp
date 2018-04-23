#include "thread_pool.h"
#include <stdlib.h>
#include <stdio.h>

ThreadPool::ThreadPool(int num):threadsNum(num)
{
    printf("creating threads pool...\n");
    isRunning = true;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condition, NULL);
    createThreads();
    printf("created threads pool successfully!\n");
}

ThreadPool::~ThreadPool()
{
    clearThreads();  //回收所有线程
    clearQueue(); //清空队列中还没完成的任务
    printf("thread pool is closed!\n");
}

//懒汉模式，线程安全
ThreadPool* ThreadPool::createThreadPool(int num)
{
    static ThreadPool* pThreadPoolInstance = new ThreadPool(num);
    return pThreadPoolInstance;
}

void ThreadPool::createThreads()
{
    pThreadSet = (pthread_t*)malloc(sizeof(pthread_t) * threadsNum);
    for(int i=0;i<threadsNum;i++)
    {
        pthread_create(&pThreadSet[i], NULL, threadFunc, this);
    }
}


void ThreadPool::clearQueue()
{
    while(!taskQueue.empty())
    {
        delete (taskQueue.front());
        taskQueue.pop();
    }
}

void ThreadPool::clearThreads()
{
    if(!isRunning)
        return;

    isRunning = false;
    pthread_cond_broadcast(&condition);
    printf("start closing...\n");

    for (int i = 0; i < threadsNum; i++)
    {
        pthread_join(pThreadSet[i], NULL);
        printf("close thread %lu\n",pThreadSet[i]);
    }

    free(pThreadSet);
    pThreadSet = NULL;
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition);
}

int ThreadPool::getQueueSize()
{
    pthread_mutex_lock(&mutex);
    int size = taskQueue.size();
    pthread_mutex_unlock(&mutex);
    printf("current queue size is %d\n",size);
    return size;
}

int ThreadPool::getThreadlNum()
{
    return threadsNum;
}

void ThreadPool::addTask(Task* pTask)
{
    pthread_mutex_lock(&mutex);
    taskQueue.push(pTask);
    printf("one task is put into queue! Current queue size is %lu\n",taskQueue.size());
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&condition);
}


Task* ThreadPool::takeTask()
{
    Task* pTask = NULL;
    while(!pTask)
    {
        pthread_mutex_lock(&mutex);
        //线程池运行正常但任务队列为空，那就等待任务的到来
        while(taskQueue.empty() && isRunning)
        {
            pthread_cond_wait(&condition, &mutex);
        }

        if(!isRunning)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        else if(taskQueue.empty())
        {
            pthread_mutex_unlock(&mutex);
            continue;
        }

        pTask = taskQueue.front();
        taskQueue.pop();
        pthread_mutex_unlock(&mutex);

    }

    return pTask;
}


void* ThreadPool::threadFunc(void* arg)
{
    ThreadPool* p = (ThreadPool*)arg;
    while(p->isRunning)
    {
        Task* task = p->takeTask();
        //如果取到的任务为空，那么我们结束这个线程
        if(!task)
        {
            //printf("%lu thread will shutdown!\n", pthread_self());
            break;
        }

        printf("take one...\n");

        task->run();
    }
}


