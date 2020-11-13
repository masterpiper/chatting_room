/*server.c*/
#include "header.h"
#include "header.c"

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
