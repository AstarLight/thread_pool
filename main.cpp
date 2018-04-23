#include <stdio.h>
#include "thread_pool.h"
#include <string>
#include <stdlib.h>

typedef struct
{
    int task_id;
    std::string task_name;
}msg_t;

class MyTask: public Task
{
public:
    int run()
    {
        msg_t* msg = (msg_t*)arg;   //参数解析
        printf("working thread[%lu] : task_id:%d  task_name:%s\n", pthread_self(),
               msg->task_id, msg->task_name.c_str());
        sleep(10);
        return 0;
    }
};

int main()
{
    ThreadPool* pMyPool = ThreadPool::createThreadPool(5); //建立大小为5的线程池
    char buf[32] = {0};

    msg_t msg[10];
    MyTask task_A[10];

    //模拟生产者生产任务
    for(int i=0;i<10;i++)
    {
        msg[i].task_id = i;
        sprintf(buf,"qq_task_%d",i);
        msg[i].task_name = buf; 
        task_A[i].SetArg(&msg[i]);      //设置函数参数
        pMyPool->addTask(&task_A[i]);    //任务入队
        sleep(1);
    }

    while(1)
    {
        //printf("there are still %d tasks need to process\n", pMyPool->getQueueSize());
        if (pMyPool->getQueueSize() == 0)
        {
            printf("Now I will exit from main\n");
            break;
        }

        sleep(1);
    }

    delete pMyPool;
    return 0;
}
