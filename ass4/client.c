#include<sys/msg.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include"codes.h"
#include<stdio.h>
#include<errno.h>
int main(int argc, char *argv[])
{
  char a[50];
  printf("\nEnter id:");
  scanf("%s",a);

  pid_t pid=getpid();
  /*printf("%d",pid);*/

  int up_qid,down_qid;
  if((up_qid = msgget(UPQ,0666))<0){
    perror("msgget");
    exit(1);
  }
  if((down_qid = msgget(DOWNQ,0666))<0){
    perror("msgget");
    exit(1);
  }
  char buf[100];
  strcpy(buf,"NEW ");
  strcat(buf,a);

  msg* cur_msg= (msg *)(malloc(sizeof(msg)));
  msg get_msg;
  strcpy(cur_msg->mtext,buf);
  cur_msg->mtype=pid;
  if(msgsnd(up_qid,(void *)cur_msg,MSGSIZE,IPC_NOWAIT	) < 0){
    perror("connecting");
    exit(1);
  }
  /*printf("\n%d ",down_qid);*/
  while(1){
    while(msgrcv(down_qid,&get_msg,200,pid,MSG_NOERROR)<0){
      /*perror("");*/
    }
    printf("\n %s",get_msg.mtext);
    exit(1);
  }
  return 0;
}
