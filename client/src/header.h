#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>

#include <pthread.h>

#define PORT  4321
#define BUFFER_SIZE 1024
#define MYFIFO "./record_for_client"

struct sockaddr_in serv_addr;
struct hostent *host;
int sockfd;
int sendbytes, recvbytes, sin_size;
int fd,fd_w;

char buf_recv[BUFFER_SIZE];//接受缓冲
char buf_send[BUFFER_SIZE];//发送缓冲
char buf[BUFFER_SIZE];//记录显示缓冲

pthread_t p_send,p_recv,p_display;//线程变量

void init_socket(int argc,char *argv[]);
void connect_socket();
void write_to_fifo(char buf[]);

//线程程序
void create_process();//创建线程
void *info_recv();//接受函数
void *info_send();//发送函数
void *display();//聊天记录打印
