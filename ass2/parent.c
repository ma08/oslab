#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
  int **pipe_ids;
  pid_t pid=-1;
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
    pid=fork();
    sprintf(a,"%d",pipe_ids[i][0]);
    cmd[1]=a;
    cmd[2]=b;
    sprintf(b,"%d",pipe_ids[i][1]);
    if(pid==0){
      execvp("./child.out",cmd);
      perror("");
    }
  }
  return 0;
}
