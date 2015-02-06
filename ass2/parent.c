#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "codes.h"

int write_int_to_child(int childno,int sig,int **ptoc_ids){
  int nbytes;
  char *buf=(char *)(malloc(sizeof(char)*20));
  sprintf(buf,"%d",sig);
  nbytes=strlen(buf)+1;
  nbytes=write(ptoc_ids[childno][1], buf, nbytes); 
  return nbytes;
}
int write_str_to_child(int childno,char* buf,int **ptoc_ids){
  int nbytes;
  nbytes=strlen(buf)+1;
  nbytes=write(ptoc_ids[childno][1], buf, nbytes); 
  return nbytes;
}

int write_to_all(char *buf,int **ptoc_ids){
  int i;
  int nbytes=strlen(buf)+1;
  for (i = 0; i < 5; ++i)
  {
    nbytes=write(ptoc_ids[i][1], buf, nbytes); }
  return nbytes;
}

int main(int argc, char *argv[])
{
  srand(time(NULL));
  int **ptoc_ids,**ctop_ids;
  int sig;
  int pivot;
  int k,sum;
  int rand_child;
  int n=25;
  int get_pivot=0;
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
      write_int_to_child(i,i+1,ptoc_ids);
      /*sprintf(buf,"%d",i+1);*/
      /*nbytes=strlen(buf)+1;*/
      /*close(ptoc_ids[i][0]);*/
      /*printf("\n$$$%d\n",nbytes);*/
      /*nbytes=write(ptoc_ids[i][1], buf, nbytes); */
      /*printf("\n$$$%d\n",nbytes);*/
      /*close(ptoc_ids[i][1]);*/
    }
  }
  for(i=0; i<5 ;++i){
    /*printf("----reading----");*/
    strcpy(buf2,"oo");
    /*close(ctop_ids[i][1]);*/
    read(ctop_ids[i][0],buf2,sizeof(buf2));
    printf("Child %d sends READY\n",i+1);
    if(errno!=0)
      perror("");
    /*close(ctop_ids[i][0]);*/
    printf("\n%d %s",i,buf2);
  }
  printf("Parent READY\n");
  k=n/2;
  while(1){
    while(1){
      rand_child=rand()%5;
      /*printf("---%d----",rand_child+1);*/
      /*sprintf(buf,"%d",REQUEST);*/
      /*nbytes=strlen(buf)+1;*/
      /*close(ptoc_ids[rand_child][0]);*/
      /*printf("----writing----");*/
      /*nbytes=write(ptoc_ids[rand_child][1], buf, nbytes); */
      /*printf("----%d---written---",nbytes);*/
      /*perror("");*/

      write_int_to_child(rand_child,REQUEST,ptoc_ids);
      printf("Parent sends REQUEST to Child %d\n",rand_child+1);

      read(ctop_ids[rand_child][0],buf2,sizeof(buf2));
      sig=strtol(buf2,NULL,10);
      if(sig!=-1){
        pivot=sig;
        printf("Child %d sends %d to parent\n",rand_child+1,pivot);
        break;
      }
    }
    sprintf(buf,"%d",PIVOT);
    nbytes=write_to_all(buf,ptoc_ids);
    /*nbytes=strlen(buf)+1;*/
    /*for (i = 0; i < 5; ++i)
    {
      write_str_to_child(i,buf,ptoc_ids);
      [>write(ptoc_ids[i][1], buf, nbytes); <]
    }*/
    sprintf(buf,"%d",pivot);
    nbytes=write_to_all(buf,ptoc_ids);
    printf("Parent broadcasts pivot %d to all children\n",pivot);
    /*nbytes=strlen(buf)+1;
    for (i = 0; i < 5; ++i)
    {
      write_str_to_child(i,buf,ptoc_ids);
      [>write(ptoc_ids[i][1], buf, nbytes); <]
      [>perror("");<]
    }*/
    sum=0;
    int sig[5];
    for (i = 0; i < 5; ++i)
    {
      read(ctop_ids[i][0],buf2,sizeof(buf2));
      sig[i]=strtol(buf2,NULL,10);
      /*for(j=0;j<5;j++)*/
      printf("Child %d receives pivot and replies %d\n",i+1,sig[i]);
      sum+=sig[i];
      /*perror("");*/
    }
    printf("Parent: m=%d + %d + %d + %d + %d = %d. ",sig[0],sig[1],sig[2],sig[3],sig[4],sum);
    if(sum==k){
      printf(" %d=%d/2.Median Found!.\n",pivot,n);
      sprintf(buf,"%d",EXIT);
      printf("Child 1 terminates\n");
      printf("Child 2 terminates\n");
      printf("Child 3 terminates\n");
      printf("Child 4 terminates\n");
      printf("Child 5 terminates\n");
      nbytes=write_to_all(buf,ptoc_ids);
      printf("Parent terminates\n");
      return 0;
    }else if(sum>k){
      printf("Median not found. All elements smaller than pivot are removed by child processes.\n");
      printf("Process continued again. :(\n");
      sprintf(buf,"%d",SMALL);
      nbytes=write_to_all(buf,ptoc_ids);
    }else if(sum<k){
      printf("Median not found. All elements larger than pivot are removed by child processes.\n");
      printf("Process continued again. :(\n");
      sprintf(buf,"%d",LARGE);
      nbytes=write_to_all(buf,ptoc_ids);
      k=k-sum;
    }
  }
  /*close(ptoc_ids[rand_child][1]);*/

  return 0;
}
