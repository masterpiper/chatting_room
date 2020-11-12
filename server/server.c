/*server.c*/
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

#define PORT			4321
#define BUFFER_SIZE		1024
#define MAX_QUE_CONN_NM	5
#define MYFIFO "./record_for_server"

struct sockaddr_in server_sockaddr, client_sockaddr;
int sin_size, recvbytes, sendbytes;
int sockfd, client_fd;
int fd,fd_w;
char buf_recv[BUFFER_SIZE];
char buf_send[BUFFER_SIZE];
char buf[BUFFER_SIZE];

pthread_t p_recv,p_send,p_display;

void init_socket()//socket初始化
{
	/*建立socket连接*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0))== -1)
	{
		perror("socket");
		exit(1);
	}
	printf("Socket id = %d\n",sockfd);
	
	/*设置sockaddr_in 结构体中相关参数*/
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(PORT);
	server_sockaddr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(server_sockaddr.sin_zero), 8);
  //接受和传输缓冲区初始化
	memset(buf_recv , 0, sizeof(buf_recv));
	memset(buf_send , 0, sizeof(buf_send));
}
void bind_socket()//绑定socket
{
	int i = 1;/* 使得重复使用本地地址与套接字进行绑定 */
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));	
	/*绑定函数bind*/
	if (bind(sockfd, (struct sockaddr *)&server_sockaddr, sizeof(struct sockaddr))== -1)
	{
		perror("bind");
		exit(1);
	}
	printf("Bind success!\n");
}
void listen_socket()//监听客户端
{
	/*调用listen函数*/
	if (listen(sockfd, MAX_QUE_CONN_NM) == -1)
	{
		perror("listen");
		exit(1);
	}
	printf("Listening....\n");
}
void accept_socket()
{
	/*调用accept函数，等待客户端的连接*/
	if ((client_fd = accept(sockfd, (struct sockaddr *)&client_sockaddr, &sin_size)) == -1)
	{
		perror("accept");
		exit(1);
	}
}

void write_to_fifo(char buf[])//写入通信管道
{
  if((fd=open(MYFIFO,O_WRONLY))==-1)
  {
    printf("open fifo file error!\n");
    exit(1);
  }
  if((fd_w=write(fd,buf,BUFFER_SIZE))<0)
  {
    printf("write error!\n");
    exit(1);
  }
  close(fd);
}
void *info_send()//向客户端发送信息线程
{
  while(1)
  {
    if((sendbytes = send(client_fd, buf_send,strlen(buf_send),0))==-1)
    {
      perror("send");
      exit(1);
    }
    memset(buf_send,0,sizeof(buf_send));
    memset(buf,0,sizeof(buf_send));
    fgets(buf_send,BUFFER_SIZE,stdin);
    strcpy(buf,"Server:");
    strcat(buf,buf_send);
    write_to_fifo(buf);
  }
}
void *info_recv()//接受客户端信息线程
{
  while(1)
  {
    /*调用recv函数接收客户端的请求*/
    if ((recvbytes = recv(client_fd, buf_recv, BUFFER_SIZE, 0)) == -1)
    {
      perror("recv");
      exit(1);
    }
    memset(buf,0,BUFFER_SIZE);
    strcpy(buf,"Client:");
    strcat(buf,buf_recv);
    write_to_fifo(buf);
    memset(buf_recv,0,sizeof(buf_recv));
  }
}
void *display()//聊天记录显示线程
{
  system("alacritty -e ./display_record");
}

void create_process()//创建线程
{
  pthread_create(&p_display,NULL,display,NULL);
  pthread_create(&p_recv,NULL,info_recv,NULL);
  pthread_create(&p_send,NULL,info_send,NULL);
  pthread_join(p_recv,NULL);
  pthread_join(p_send,NULL);
  pthread_join(p_display,NULL);
}

int main()
{
	init_socket();
	bind_socket();
  listen_socket();
  accept_socket();
  create_process();
	close(sockfd);
	exit(0);
}
