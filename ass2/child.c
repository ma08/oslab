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
  
  close(pipe_value[1]);
  nbytes = read(pipe_value[0], buf, BSIZE);   
  close(pipe_value[0]);

  strcat(outputfilename,buf);
  freopen(outputfilename,"w",stdout);
  for (i = 0; i < 2; ++i)
  {
    printf("%d\n",pipe_value[i]);
  }
  id=strtol(buf,NULL,10);
  /*printf("%d\n",id);*/
  sprintf(buf,"%d",1000);
  if(write(pipe_value[1], buf, BSIZE)<=0)
    perror("hoo"); 
  close(pipe_value[1]);

	return 0;
}
