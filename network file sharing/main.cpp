#include "threadpool.h"
#include "heap_timer.h"
#define MAX_FD 65535
#include<sys/epoll.h>
#include<errno.h>
#define FD_LIMIT 65535
#define MAX_EVENT_NUMBER 1024
#define TIMESLOT 100 

static int pipefd[2];
static int epollfd = 0;
static time_heap timer_heap(10);
extern "C"
{
    #include "httpd.h"
}

extern "C"
{
    int deal_accept_request(int );
}

class conn
{
public:
    conn(int fd = -1)
        :connfd(fd)
    {}

    ~conn()
    {}

    void process()
    {
        deal_accept_request(connfd);
    }
private:
    int connfd;
};


int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

static void time_addfd(int epollfd, int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD,fd, &event);
    //setnonblocking(fd);
}

void sig_handler(int sig)
{
    int save_errno = errno;
    printf("sig_handler %d is runnig\n", sig);
    int msg = sig;
    send(pipefd[1], (char*)&msg, 1, 0);
    printf("sig_handler is runned\n");
    errno = save_errno;
}

void addsig(int sig)
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = sig_handler;
    sa.sa_flags|= SA_RESTART;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}



static void Useage(const char* str)
{
	printf("Usage: %s [local_ip] [local_port]\n",str);
}


void timer_handler()
{
    timer_heap.tick();
    alarm(TIMESLOT);
}

void cb_func(client_data* user_data)
{
    printf("cb_func is running!\n");
    epoll_ctl(epollfd, EPOLL_CTL_DEL,user_data->sockfd, 0);
    assert(user_data);
    close(user_data->sockfd);
    printf("close fd %d\n",user_data->sockfd);
}

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		Useage(argv[0]);
		return 0;
	}

//	daemon(1,0);
	
	int listensock = startup(argv[1],atoi(argv[2]));
    
    threadpool< conn >* pool = NULL;
    try
    {
        pool = new threadpool< conn >;
    }
    catch( ... )
    {
        return 1;
    }
    
    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    time_addfd(epollfd, listensock);

    int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, pipefd);
    assert(ret != -1);
    //setnonblocking(pipefd[1]);
    time_addfd(epollfd, pipefd[0]);


    /* 设置信号处理函数 */
    addsig(SIGALRM);
    addsig(SIGTERM);

    bool stop_server = false;

    client_data* users = new client_data[FD_LIMIT];
    bool timeout = false;
    alarm(TIMESLOT);  /* 定时 */

    while(!stop_server)
    {
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if((number < 0))
        {
            printf("epoll failed\n");
            break;
        }

        for (int i = 0; i < number; ++i)
        {
            int sockfd = events[i].data.fd;
            /* 处理新的客户链接 */
            if (sockfd == listensock)
            {
                struct sockaddr_in client_address;
                socklen_t client_addlenth = sizeof(client_address);
                int connfd = accept(listensock, (struct sockaddr*)&client_address,&client_addlenth);
                time_addfd(epollfd, connfd);
                users[connfd].address = client_address;
                users[connfd].sockfd = connfd;

                /* 创建定时器，设置其回调函数与超时时间，然后绑定定时器与用户数据，最后将定时器添加到时间堆中； */
                heap_timer* timer = new heap_timer(120);
                timer->user_data = &users[connfd];
                timer->cb_func = cb_func;
                time_t cur = time(NULL);
                timer->expire = cur + TIMESLOT;
                timer_heap.add_timer(timer);
                printf("new link add!\n");
                pool->append(&conn(connfd));
            }

            /* 处理信号 */
            else if((sockfd == pipefd[0]) && (events[i].events & EPOLLIN))
            {

                printf("deal signal");
                int sig;
                char signals[1024];
                ret = recv(pipefd[0], signals, sizeof(signals), 0);
                if(ret == -1)
                {
                    //handler the error
                    continue;
                }
                else if(ret == 0)
                {
                    continue;
                }
                else
                {
                    printf("signal ret is %d\n", ret);
                    for(int i = 0; i < ret; i++)
                    {
                        printf("signal is %d\n",signals[i]);
                        switch(signals[i])
                        {
                            case SIGALRM:
                            {
                                timeout = true;
                                break;
                            }
                            case SIGTERM:
                            {
                                stop_server = false;
                                break;
                            }
                        }
                    }
                }
            }
            else if(events[i].events & EPOLLIN)
            {
                
                char c;
			    int ret = recv(sockfd,&c,1,MSG_PEEK);
                if(ret <= 0)
                {
                    //关闭链接 
                    timer_heap.del_timer(users[sockfd].timer);
                    cb_func(users[sockfd]);
                    users[sockfd].timer = NULL;
                }
                else
                {
                     heap_timer* timer = users[sockfd].timer;

                     if(timer)
                     {
                         heap_timer* tmp = NULL;
                         tmp->expire = time(NULL) + TIMESLOT;
                         tmp->cb_func = timer->cb_func;
                         tmp->user_data = timer->user_data;

                         timer_heap.del_timer(timer);
                         printf("adjust timer once\n");
                         users[sockfd].timer = tmp;
                         timer_heap.add_timer(tmp);
                     }
                }
            }
        }

        if(timeout)
        {
            printf("timer_handler is running");
            timer_handler();
            timeout = false;
        }

    }

    close(listensock);
    close(pipefd[0]);
    close(pipefd[1]);
    delete pool;
	return 0;
}
