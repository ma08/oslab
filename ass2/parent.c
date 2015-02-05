#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
  int **pipe_ids;
  char buf[100];
  char buf2[100];
  pid_t pid=-1;
  pid_t child_pids[5];
  char a[10];
  char b[10];
  int i;
  pipe_ids=(int**)(malloc(sizeof(int*)*5));
  char **cmd = (char **)(malloc(sizeof(char **)*4));
  cmd[0]="child.out";
  cmd[3]=(char *)0;
  for(i=0;i<5;++i)
    pipe_ids[i]=(int*)(malloc(sizeof(int)*2));
  for (i = 0; i < 5; ++i)
  {
    pipe(pipe_ids[i]);
    /*printf("\n%d %d\n",pipe_ids[i][0],pipe_ids[i][1]);*/
  }
  for (i = 0; i < 1; ++i)
  {
    pid=fork();
    child_pids[i]=pid;
    sprintf(a,"%d",pipe_ids[i][0]);
    sprintf(b,"%d",pipe_ids[i][1]);
    cmd[1]=a;
    cmd[2]=b;
    if(pid==0){
      execvp("./child.out",cmd);
      /*perror("");*/
    }else{
      sprintf(buf,"%d",i);
      close(pipe_ids[i][0]);
      write(pipe_ids[i][1], buf, 5); 
      close(pipe_ids[i][1]);
    }
  }
  for(i=0; i<1 ;++i){
    close(pipe_ids[i][1]);
    read(pipe_ids[i][0],buf2,100);
    close(pipe_ids[i][0]);
    /*printf("%s",buf2);*/
  }
  return 0;
}
