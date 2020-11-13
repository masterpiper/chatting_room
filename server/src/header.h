#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>

#include <pthread.h>

#define PORT      4321
#define BUFFER_SIZE   1024
#define MAX_QUE_CONN_NM 5
#define MYFIFO "./record_for_server"

struct sockaddr_in server_sockaddr, client_sockaddr;
int sin_size, recvbytes, sendbytes;
int sockfd, client_fd;
int fd,fd_w;

char buf_recv[BUFFER_SIZE];
char buf_send[BUFFER_SIZE];
char buf[BUFFER_SIZE];

pthread_t p_recv,p_send,p_display;//线程变量

void init_socket();
void bind_socket();
void listen_socket();
void accept_socket();
void write_to_fifo(char buf[]);

//线程程序
void create_process();
void *info_send();
void *info_recv();
void *display();
