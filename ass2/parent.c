#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
  int **ptoc_ids,**ctop_ids;
  int nbytes;
  char buf[100];
  char buf2[100];
  pid_t pid=-1;
  pid_t child_pids[5];
  char a[10];
  char b[10];
  int i;
  ptoc_ids=(int**)(malloc(sizeof(int*)*5));
  ctop_ids=(int**)(malloc(sizeof(int*)*5));
  char **cmd = (char **)(malloc(sizeof(char **)*4));
  cmd[0]="child.out";
  cmd[3]=(char *)0;
  for(i=0;i<5;++i){
    ptoc_ids[i]=(int*)(malloc(sizeof(int)*2));
    ctop_ids[i]=(int*)(malloc(sizeof(int)*2));
  }
  for (i = 0; i < 5; ++i)
  {
    pipe(ptoc_ids[i]);
    pipe(ctop_ids[i]);
    /*printf("\n%d %d",ptoc_ids[i][0],ptoc_ids[i][1]);*/
    /*printf("\n%d %d",ctop_ids[i][0],ctop_ids[i][1]);*/
    /*printf("\n%d %d\n",pipe_ids[i][0],pipe_ids[i][1]);*/
  }
  for (i = 0; i < 5; ++i)
  {
    pid=fork();
    child_pids[i]=pid;
    sprintf(a,"%d",ptoc_ids[i][0]);
    sprintf(b,"%d",ctop_ids[i][1]);
    cmd[1]=a;
    cmd[2]=b;
    if(pid==0){
      execvp("./child.out",cmd);
      /*perror("");*/
    }else{
      sprintf(buf,"%d",i);
      nbytes=strlen(buf)+1;
      close(ptoc_ids[i][0]);
      /*printf("\n$$$%d\n",nbytes);*/
      nbytes=write(ptoc_ids[i][1], buf, nbytes); 
      /*printf("\n$$$%d\n",nbytes);*/
      close(ptoc_ids[i][1]);
    }
  }
  for(i=0; i<5 ;++i){
    /*printf("----reading----");*/
    strcpy(buf2,"oo");
    close(ctop_ids[i][1]);
    read(ctop_ids[i][0],buf2,sizeof(buf2));
    if(errno!=0)
      perror("");
    close(ctop_ids[i][0]);
    printf("\n%d %s",i,buf2);
  }
  return 0;
}
