/*client.c*/
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

#define PORT	4321
#define BUFFER_SIZE 1024
#define MYFIFO "./record_for_client"

struct sockaddr_in serv_addr;
struct hostent *host;
int sockfd;
int sendbytes, recvbytes, sin_size;
int fd,fd_w;
char buf_recv[BUFFER_SIZE];
char buf_send[BUFFER_SIZE];
char buf[BUFFER_SIZE];

pthread_t p_send,p_recv,p_display;

void init_socket(int argc, char *argv[])//��ʼ���ͻ���
{
	if(argc < 3)
	{
		fprintf(stderr,"USAGE: ./client Hostname(or ip address) Text\n");
		exit(1);
	}
	
	/*��ַ��������*/
	if ((host = gethostbyname(argv[1])) == NULL)
	{
		perror("gethostbyname");
		exit(1);
	}
	
	memset(buf_recv, 0, sizeof(buf_recv));
	memset(buf_send, 0, sizeof(buf_send));
	sprintf(buf_send, "%s", argv[2]);
	
	/*����socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	
	/*����sockaddr_in �ṹ������ز���*/
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(serv_addr.sin_zero), 8);
}

void connect_socket()//���ӷ�����
{
	/*����connect������������Է������˵�����*/
	if(connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(struct sockaddr))== -1)
	{
		perror("connect");
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

void *info_recv()//�ͻ��˽����߳�
{
  while(1)
  {
    memset(buf,0,BUFFER_SIZE);
    if((recvbytes = recv(sockfd,buf_recv,BUFFER_SIZE,0))==-1)
    {
      perror("recv");
      exit(1);
    }
    strcpy(buf,"Server:");
    strcat(buf,buf_recv);
    write_to_fifo(buf);
    memset(buf_recv,0,sizeof(buf_recv));
  }
}
void *info_send()//������������߳�
{
  while(1)
  {
    /*������Ϣ����������*/
    if ((sendbytes = send(sockfd, buf_send, strlen(buf_send), 0)) == -1)
    {
      perror("send");
      exit(1);
    }
    memset(buf_send,0,sizeof(buf_send));
    memset(buf,0,BUFFER_SIZE);
    fgets(buf_send,BUFFER_SIZE,stdin);
    strcpy(buf,"Client:");
    strcat(buf,buf_send);
    write_to_fifo(buf);
  }
}
void *display()//��ʾ�����¼�߳�
{
  system("alacritty -e ./display_record");
}
void create_process()//�����߳�
{
  pthread_create(&p_display,NULL,display,NULL);
  pthread_create(&p_send,NULL,info_send,NULL);
  pthread_create(&p_recv,NULL,info_recv,NULL);
  pthread_join(p_send,NULL);
  pthread_join(p_recv,NULL);
  pthread_join(p_display,NULL);
}

int main(int argc, char *argv[])
{
  init_socket(argc,argv);
	connect_socket();
  create_process();
	close(sockfd);
	exit(0);
}
