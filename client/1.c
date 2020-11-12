#include <string.h>
#include <stdio.h>
int main()
{
  char b[1024];
  char b1[1024];
  strcpy(b,"buffer");
  strcpy(b1,"client:");
  strcat(b1,b);
  printf("%ld\n",strlen(b1));
  printf("%s\n",b1);
}
