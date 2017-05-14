//��ͬ�����첽���̳�
#ifndef PROCESSPOOL_H
#define PROCESSPOOL_H

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<stdio.h>
#include<unistd.h>
#include<error.h>
#include<string.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/epoll.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/stat.h>

/* ����һ���ӽ��̵��࣬ m_pid��Ŀ���ӽ��̵�PID �� m_pipefd �Ǹ����̺��ӽ���ͨ���õĹܵ�  */
class process
{
public:
	process(): m_pid(-1){}
	
public:
	pid_t m_pid;
	int m_pipefd[2];
};

/* ���̳��࣬ ��������Ϊģ������Ϊ�˴��븴�á� ��ģ������Ǵ����߼�������� */
template <typename T>
class processpool
{
private:
	/* �����캯������Ϊ˽�У� �������ֻ��ͨ�������create��̬����������processpoolʵ�� */
	processpool(int listenfd, int process_number = 8);

public:
	/*  ����ģʽ�� �Ա�֤�������ʹ���һ��ʵ���� ���ǳ�����ȷ�����źŵı�Ҫ���� */
	static processpool<T>* create(int listenfd, int process_number = 8)
	{
		if (m_instance)
		{
			m_instance = new processpool<T> (listend, process_number);
		}
		
		return m_instance;
	}
	
	~processpool()
	{
		delete m_sub_process;
	}
	
	/* �������̳�  */
	void run();
	
private:
	void setup_sig_pipe();
	void run_parent();
	void run_child();
	
private:
	static const int MAX_PROCESS_NUMBER = 16;
	static const int USER_PER_PROCESS = 65535;
	static const int MAX_EVENT_NUMBER = 10000;
	int m_process_number;
	int m_idx;   /*  �ӽ����ڳ��еı��  */
	int m_epollfd;
	int m_listenfd;
	int m_stop;
	/* ���������ӽ��̵�������Ϣ  */
	process* m_sub_process;
	
	static processpool<T>* m_instance;
};

template<typename T>
processpool<T>* processpool<T>::m_instance = NULL;

/* ���ڴ����źŵĹܵ��� ��ʵ��ͳһ�¼�Դ�� �����֮Ϊ�źŹܵ� */
static int sig_pipefd[2];

static int setnonblocking(int fd)
{
	int old_option = fcntl(fd, F_GETFL);
	int new_option =  old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}

static void addfd(int epollfd, int fd)
{
	epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
	setnoblocking(fd);
}

/* ��epollfd��ʶ��epoll�ں��¼�����ɾ��fd�ϵ�����ע���¼� */
static void removefd(int epollfd, int fd)
{
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
	close(fd);
}

static void sig_handler(int sig)
{
	int save_error = errno;
	int msg = sig;
	send(sig_pipefd[1], (char*)&msg, 1, 0);
	error = save_errno;
}

static void addsig(int sig, void(handler)(int), bool restart = true)
{
	struct sigaction sa;
	memset(&sa, '\0', sizeof(sa));
	sa.sa_handler = handler;
	if(restart)
	{
		sa.sa_flags |= SA_RESTART;
	}
	
	sigfillset(&sa.sa_mask);
	assert(sigaction(sig, &sa, NULL) != -1);
}

/* ���̳صĹ��캯���� ����listenfd�Ǽ���socket���������ڴ������̳�֮ǰ�������������ӽ����޷�ֱ���������� ����process_numberָ�����̳��е��ӽ��̵����� */
template <typename T>
processpool<T>::processpool(int listenfd, int process_number)
	:m_listenfd(litenfd)
	,m_process_number(process_number)
	,m_idx(-1)
	,m_stop(false)
{
	assert((process_number > 0) && (process_number <= MAX_PROCESS_NUMBER));
	
	m_sub_process = new process[process_number];
	assert(m_sub_process);
	
	/* ����process_number ���ӽ��̣� ���������Ǻ͸�����֮��Ĺܵ�  */
	for(int i = 1; i < process_number; ++i)
	{
		int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, m_sub_process[i].m_pipefd);
		assert(ret == 0)
		
		m_sub_process[i].m_pid = fork();
		assert(m_sub_process[i].m_pid >=0);
		if(m_sub_process[i].m_pid > 0)
		{
			//parent
			close(m_sub_process[i].m_pipefd[1])
			continue;
		}
		else
		{
			close(m_sub_process[i].m_pipefd[0]);
			m_idx = i;
			break;
		}
	}
}


/* ͬһ�¼�Դ  */
template <typename T>
void processpool<T>::setup_sig_pipe()
{
	/* ����epoll�¼�������͹ܵ�  */
	m_epollfd = epoll_create(5);
	assert(m_epollfd != -1);
	
	int ret = sockpair(PF_UNIX, SOCK_STREAM, 0, sig_pipefd);
	assert(ret != -1);
	
	setnonblocking(sig_pipefd[1]);
	addfd(m_epollfd, sig_pipefd[0]);
	
	/* �����źŴ�����  */
	addsig(SIGCHLD, sig_handler);
	addsig(SIGTERM, sig_handler);
	addsig(SIGINT, sig_handler);
	addsig(SIGPIPE, SIG_IGN);      //ʲôʱ��ᷢ��SIGPIPE�ź�
}

/* ������m_idx ֵΪ-1���ӽ�����m_idx  ֵ���ڵ���0�����Ǿݴ��жϽ�����Ҫ���е��Ǹ����̵Ĵ��뻹���ӽ��̵Ĵ���  */
template <typename T>
void processpool<T>::run()
{
	if(m_idx != -1)
	{
		run_child();
		return;
	}
	run_parent();
}

template <typename T>
void processpool<T>::run_child()
{
	setup_sig_pipe();
	
	/* ÿ���ӽ��̶�ͨ�����ڽ��̳��е����ֵm_idx�ҵ��븸����ͨ�ŵĹܵ�  */
	int pipefd = m_sub_process[m_idx].m_pipefd[1];
	/* �ӽ�����Ҫ�����ܵ��ļ�������pipefd����Ϊ�����̽�ͨ������֪ͨ�ӽ��̽����µ�����  */
	addfd(m_epollfd, pipefd);
	
	epoll_event events[MAX_EVENT_NUMBER];
	T* users = new T[USER_PER_PROCESS];
	
	assert(users);
	int number = 0;
	int ret = -1;
	
	while( !m_stop )
	{
		number = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
		if(number < 0 && (errno != EINTR))
		{
			printf("epoll  failed\n");
			break;
		}
		
		for(int i = 0; i < number; ++i)
		{
			int sockfd = events[i].data.fd;
			if((sockfd == pipefd) && (events[i].events & EPOLLIN))
			{
				//�����̴����µĿͻ�����
				int client = 0;
				/* �Ӹ��ӽ���֮��Ĺܵ���ȡ���ݣ�������������ڱ���client�У� �����ȡ�ɹ���ʾ���µĿͻ�����  */
				
				ret = recv(sockfd, (char*)&client, sizeof(client), 0);
				if( ((ret < 0)  && (errno != EAGAIN) )  || ret == 0)
				{
					continue;
				}
				else
				{
					struct sockaddr_in client_address;
					socklen_t client_addrlength = sizeof(client_address);
					int connfd = accept(m_listenfd, (struct sockaddr*)&client_address, &client_addrlength);
					if(connfd < 0)
					{
						printf("errno is: %d\n", errno);
						continue;
					}
					
					addfd(m_epollfd, connfd);
					/* ģ����T����ʵ��init������ �Գ�ʼ��һ���ͻ����ӣ� ����ֱ��ʹ��connfd�������߼��������T���͵Ķ��󣩣� ����߳����Ч�� */
					users[connfd].init(m_epollfd, connfd, client_address);   //��������������ҵ�client_data   
				}
			}
			else if(socked == sig_pipefd[0]  &&  (events[i].events & EPOLLIN))
			{
				int sig;
				char signals[1024];
				ret = recv(sig_pipefd[0], signals, sizeof(signals), 0);
				if (ret < 0)
				{
					continue;
				}
				else
				{
					for(int i = 0; i < ret; ++i)
					{
						switch( signals[i])
						{
							case SIGCHLD:
							{
								pid_t pid;
								int stat;
								while ((pid = waitpid(-1, &stat, WNOHANG)) > 0 )
								{
									continue;
								}
								break;
							}
							case SIGTREM:
							case SIGINT:
							{
								m_stop = true
								break;
							}
							default:
							{
								break;
							}
						}
					}
				}
			}
			else if(events[i].events & EPOLLIN)
			{
				users[sockfd].process();
			}
			else
			{
				continue;
			}
		}
	}
	delete []users;
	users = NULL;
	close(pipefd);
	close(m_epollfd);
}

template< typename T>
void processpool<T>::run_parent()
{
	setup_sig_pipe();

	/* �����̼���listenfd */
	addfd(m_epollfd, m_listenfd);

	epoll_event events[MAX_EVENT_NUMBER];
	int sub_process_counter = 0;
	int new_conn = 1;
	int number = 0;
	int ret = -1;

	while( ! m_stop )
	{
		number = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
		if(number < 0  && (errno != EINTR))
		{
			printf("epoll failure\n");
			break;
		}
		for( int i = 0; i < number; ++i)
		{
			int sockfd = events[i].data.fd;
			if(sockfd == m_listenfd)
			{
				/* ����������ӵ����Ͳ���Round Robin(��ѯ)��ʽ��������һ���ӽ��� */
				int i = sub_process_counter;
				do
				{
					if( m_sub_process[i].m_pipefd != -1)
					{
						break;
					}
					i = (i+1)%m_process_number;
				}while(i != sub_process_counter);

				if(m_sub_process[i].m_pid == -1)
				{
					m_stop = true;
					break;
				}

				sub_process_counter = (i+1)%m_process_number;
				send( m_sub_process[i].m_pipefd[0], (char*)&new_conn, sizeof(new_conn), 0);
				printf("send request to child %d\n", i);
			}
			else if((sockfd == sig_pipefd[0]) && (events[i].events & EPOLLIN))
			{
				int sig;
				char signals[1024];
				ret = recv(sig_pipefd[0], signals, sizeof(signals));
				if(ret <= 0)
				{
					continue;
				}
				else
				{
					for(int i = 0; i < ret; ++i)
					{
						switch(signals[i])
						{
						case SIGCHLD:
							{
								pid_t pud;
								int stat;
								while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
								{
									for(int i = 0; i < m_process_number; ++i)
									{
										/* ������̳��еĵ�i���̳��˳��ˣ��������̹ر���Ӧ��
										ͨ�Źܵ�����������Ӧ��m_pid = -1�� �Ա�Ǹ��ӽ����Ѿ��˳� */
										if(m_sub_process[i].m_pid == pid)
										{
											printf("child %d join\n", i);
											close(m_sub_process[i].m_pipefd[0]);
											m_sub_process[i].m_pid = -1;
										}
									}
								}
								/* ��������ӽ��̶��˳��ˣ���ô������Ҳ�˳� */
								m_stop = true;
								for(int i = 0; i < m_process_number; ++i)
								{
									if(m_sub_process[i].m_pid != -1)
									{
										m_stop = false;
									}
								}
								break;
							}
						case SIGTREM:
						case SIGINT:
							{
								/* ��������̽��ܵ���ֹ�źţ���ô��ɱ�����е��ӽ��̣����ȴ��ӽ��̽�����
								��Ȼ��֪ͨ�ӽ��̽������õİ취�����ӽ���֮���ͨ�Źܵ������������ݣ� */
								printf("kill all the child now\n");
								for(int i = 0; i < m_process_number; ++i)
								{
									int pid = m_sub_process[i].m_pid;
									if(pid != -1)
									{
										kill(pid, SIGTERM);
									}
								}
								break;
							}
						default:
							break;
						}
					}
				}
			}
			else
			{
				continue;
			}
		}
	}
	close(m_epollfd);
}

#endif