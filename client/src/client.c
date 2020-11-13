/*client.c*/
#include "header.h"
#include "header.c"

int main(int argc, char *argv[])
{
  init_socket(argc,argv);
	connect_socket();
  create_process();
	close(sockfd);
	exit(0);
}
