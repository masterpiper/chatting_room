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

void init_socket()//socket��ʼ��
{
	/*����socket����*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0))== -1)
	{
		perror("socket");
		exit(1);
	}
	printf("Socket id = %d\n",sockfd);
	
	/*����sockaddr_in �ṹ������ز���*/
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(PORT);
	server_sockaddr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(server_sockaddr.sin_zero), 8);
  //���ܺʹ��仺������ʼ��
	memset(buf_recv , 0, sizeof(buf_recv));
	memset(buf_send , 0, sizeof(buf_send));
}
void bind_socket()//��socket
{
	int i = 1;/* ʹ���ظ�ʹ�ñ��ص�ַ���׽��ֽ��а� */
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));	
	/*�󶨺���bind*/
	if (bind(sockfd, (struct sockaddr *)&server_sockaddr, sizeof(struct sockaddr))== -1)
	{
		perror("bind");
		exit(1);
	}
	printf("Bind success!\n");
}
void listen_socket()//�����ͻ���
{
	/*����listen����*/
	if (listen(sockfd, MAX_QUE_CONN_NM) == -1)
	{
		perror("listen");
		exit(1);
	}
	printf("Listening....\n");
}
void accept_socket()
{
	/*����accept�������ȴ��ͻ��˵�����*/
	if ((client_fd = accept(sockfd, (struct sockaddr *)&client_sockaddr, &sin_size)) == -1)
	{
		perror("accept");
		exit(1);
	}
}

void write_to_fifo(char buf[])//д��ͨ�Źܵ�
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
void *info_send()//��ͻ��˷�����Ϣ�߳�
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
void *info_recv()//���ܿͻ�����Ϣ�߳�
{
  while(1)
  {
    /*����recv�������տͻ��˵�����*/
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
void *display()//�����¼��ʾ�߳�
{
  system("alacritty -e ./display_record");
}

void create_process()//�����߳�
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
