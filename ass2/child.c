#include <unistd.h>
#include<string.h>
#include <stdlib.h>
#include <errno.h>
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
	/*int pipe_value[2];*/
	int read_end=strtol(argv[1],NULL,10);
	int write_end=strtol(argv[2],NULL,10);
  
  /*close(write_end);*/
  nbytes = read(read_end, buf, BSIZE);   
  /*printf("--%s\n",buf);*/
  /*perror("");*/
  close(read_end);

  strcat(outputfilename,buf);
  freopen(outputfilename,"w",stdout);
  printf("%d\n",read_end);
  printf("%d\n",write_end);
  /*for (i = 0; i < 2; ++i)
  {
    printf("%d\n",pipe_value[i]);
  }*/
  id=strtol(buf,NULL,10);
  /*printf("%d\n",id);*/
  sprintf(buf,"%d",1000);
  /*while(read(pipe_value[1],buf,BSIZE)<=0)perror("hoo");*/
  /*if(write(pipe_value[1], buf, BSIZE)<=0)
    perror("hoo"); */
  nbytes = strlen(buf)+1;
  /*close(read_end);*/
  /*printf("----writing %d----",write_end);*/
  write(write_end, buf, nbytes);
  if(errno!=0)
    perror("hoo");
  close(write_end);

	return 0;
}
