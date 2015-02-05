#include <unistd.h>
#include<string.h>
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char const *argv[])
{
	int i;
  int id;
  int nbytes;
  const int BSIZE = 100;
  char buf[BSIZE];
  char outputfilename[20];
  strcpy(outputfilename,"childoutput");
	int pipe_value[2];
	pipe_value[0]=strtol(argv[1],NULL,10);
	pipe_value[1]=strtol(argv[2],NULL,10);
  /*for (i = 0; i < 2; ++i)
  {
    printf("%d\n",pipe_value[i]);
  }*/
  nbytes = read(pipe_value[0], buf, BSIZE);   
  strcat(outputfilename,buf);
	freopen(outputfilename,"w",stdout);
	id=strtol(buf,NULL,10);
  printf("%d\n",id);
	return 0;
}
