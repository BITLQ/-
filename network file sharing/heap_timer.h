/*************************************************************************
	> File Name: heap_timer.h
	> Author:   刘强
	> Mail: 849886839@qq.com 
	> Created Time: Sun 02 Jul 2017 02:10:16 AM PDT
 ************************************************************************/

#ifndef _HEAP_TIMER_H
#define _HEAP_TIMER_H

#include<iostream>
#include<netinet/in.h>
#include<time.h>
#define BUFFER_SIZE 64

using namespace std;

class heap_timer; /* 向前声明 */
/* 绑定socket和定时器 */
struct client_data
{
    sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    heap_timer* timer;
};

/* 定时器类 */
class heap_timer
{
public:
    heap_timer(int delay)
    {
        expire = time(NULL) + delay;
    }

public:
    time_t expire; /* 定时器生效的绝对时间 */
    void (*cb_func)(client_data*); /* 定时器的回调函数 */
    client_data* user_data; /* 用户数据 */
};

/* 时间堆类 */
class time_heap
{
public:
    /* 构造函数之一，初始化一个大小为cap的空堆 */
    time_heap(int cap) throw(std::exception):capacity(cap),cur_size(0)
    {
        array = new heap_timer* [capacity]; /* 创建数组 */
        if(!array)
        {
            throw std::exception();
        }
        for(int i = 0; i < capacity; ++i)
        {
            array[i] = NULL;
        }
    }

    /* 构造函数之2，用已有数组初始化堆 */
    time_heap(heap_timer** init_array, int size, int capacity)throw (std::exception):cur_size(size),capacity(capacity)
    {
        if(capacity < size)
        {
            throw std::exception();
        }
        
        array = new heap_timer* [capacity];
        if(!array)
        {
            throw std::exception();
        }

        for(int i = 0; i < capacity; ++i)
        {
            array[i] = NULL;
        }

        if(size != 0)
        {
            /* 初始化堆数组 */
            for(int i = 0; i < size; ++i)
            {
                array[i] = init_array[i];
            }
            for(int i = (cur_size - 1)/2; i >= 0; --i)
            {
                /*向下调整*/
                percolate_down(i);
            }
        }
    }

    /* 销毁时间堆*/
~time_heap()
{
    for(int i = 0; i < cur_size; ++i)
    {
        delete array[i];
    }
    delete[] array;
}

public:
    /* 添加目标定时器 */
    void add_timer(heap_timer* timer) throw(std::exception)
    {
        if(!timer)
        {
            return;
        }
        if(cur_size >= capacity)
        {
            resize();
        }

        /* 新插入一个元素，当前堆大小增加1，hole是新建空穴的位置*/
        int hole = cur_size++;
        int parent = 0;

        for(; hole > 0; hole = parent)
        {
            parent = (hole - 1)/2;
            if(array[parent]->expire <= timer->expire)
            {
                break;
            }
            array[hole] = array[parent];
        }
        array[hole] = timer;
    }

    /* 删除目标定时器 */
    void del_timer(heap_timer* timer)
    {
        if(!timer)
        {
            return;
        }
        /* 仅仅将目标定时器的回调函数置为NULL，即所谓的延迟销毁，这将节省真正销毁定时器的开销，但这样做容易使数组膨胀 */
        timer->cb_func = NULL;
    }

    /* 获得堆顶的定时器 */
    heap_timer* top()const
    {
        if(empty())
        {
            return NULL;
        }
        return array[0];
    }

    /* 删除堆顶部的计时器 */
    void pop_timer()
    {
        if(empty())
        {
            return;
        }
        if(array[0])
        {
            delete array[0];
            array[0] = array[--cur_size];
            percolate_down(0);
        }
    }

    /* 心博函数 */
    void tick()
    {
        heap_timer* tmp = array[0];
        time_t cur = time(NULL);
        while(!empty())
        {
            if(!tmp)
            {
                break;
            }
            /* 如果堆顶计时器没到期则退出循环 */
            if(tmp->expire > cur)
            {
                break;
            }
            /* 否则就执行堆顶计时器的任务 */
            if(array[0]->cb_func)
            {
                array[0]->cb_func(array[0]->user_data);
            }

            /* 将堆顶元素删除， 同时生成新的堆顶计时器 */
            pop_timer();
            tmp = array[0];
        }
    }

    bool empty() const{return cur_size;}

private:
    /* 最小堆的下调操作 */
    void percolate_down(int hole)
    {
        heap_timer* temp = array[hole];
        int child = 0;
        for(; (hole*2 +1) <= (cur_size - 1); hole=child)
        {
            child = hole*2 + 1;
            if((child < (cur_size - 1)) && (array[child+1]->expire < array[child]->expire))
            {
                ++child;
            }
            if(array[child]->expire < temp->expire)
            {
                array[hole] = array[child];
            }
            else
            {
                break;
            }
        }
        array[hole] = temp;
    }

    /* 将堆数组容量扩大一倍*/
    void resize()throw(std::exception)
    {
        heap_timer** temp = new heap_timer* [2*capacity];
        for(int i = 0; i < 2*capacity; ++i)
        {
            temp[i] = NULL;
        }
        if(!temp)
        {
            throw std::exception();
        }
        capacity = 2*capacity;
        for(int i = 0; i < cur_size; ++i)
        {
            temp[i] = array[i];
        }
        delete[] array;
        array = temp;
    }

private:
    heap_timer** array;
    int capacity;
    int cur_size;
};

#endif

