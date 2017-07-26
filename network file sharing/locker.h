/*************************************************************************
	> File Name: locker.h
	> Author:   刘强
	> Mail: 849886839@qq.com 
	> Created Time: Thu 15 Jun 2017 07:12:49 PM PDT
 ************************************************************************/

#ifndef _LOCKER_H
#define _LOCKER_H

/* 同步机制包装类 */

#include <exception>
#include <pthread.h>
#include <semaphore.h>

/* 封装信号量 */
class sem 
{
public:
    sem()
    {
        if (sem_init(&m_sem, 0, 0) != 0)
        {
            throw std::exception();
        }
    }

    ~sem()
    {
        sem_destroy(&m_sem);
    }

    bool wait()
    {
        return sem_wait(&m_sem) == 0;
    }

    bool post()
    {
        return sem_post(&m_sem) == 0;
    }

private:
    sem_t m_sem;
};

/* 封装互斥锁类 */
class locker 
{
public:
    locker()
    {
        if( pthread_mutex_init( &m_mutex, NULL) != 0)
        {
            throw std::exception();
        }
    }

    ~locker()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;
    }

    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }

private:
    pthread_mutex_t m_mutex;
};

#endif
