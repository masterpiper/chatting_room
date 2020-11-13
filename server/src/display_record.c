#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MYFIFO "./record_for_server"
#define BUFFER_SIZE 1024

int main()
{
  char buf[BUFFER_SIZE];
  int fd,nread;
  if(access(MYFIFO, F_OK)==-1)
  {
    if((mkfifo(MYFIFO,0666)<0)&&(errno != EEXIST))
    {
      printf("Cannot create fifo file\n");
      exit(1);
    }
  }
  if((fd=open(MYFIFO,O_RDONLY))==-1)
  {
    printf("Open fifo file error\n");
    exit(1);
  }
  while(1)
  {
    memset(buf,0,BUFFER_SIZE);
    if((nread=read(fd,buf,BUFFER_SIZE))>0)
    {
      printf("%s\n",buf);
    }
  }
  close(fd);
  exit(0);
}
