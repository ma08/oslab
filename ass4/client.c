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
  char list[300];
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
  while(msgsnd(up_qid,(void *)cur_msg,MSGSIZE,IPC_NOWAIT  ) < 0){
    perror("connecting");
    /*exit(1);*/
  }
  /*exit(1);*/
  printf("\n%d ",down_qid);
  while(msgrcv(down_qid,&get_msg,200,pid,MSG_NOERROR|IPC_NOWAIT)<0){
    perror("receiving");
  }
  if(get_msg.mtext[0]=='L'&&get_msg.mtext[1]=='I'){
    strcpy(list,get_msg.mtext+5);
  }
  int i=1;
  int j=0;
  printf("\nList of clients");
  printf("\n1. ");
  i++;
  while(list[j]!='\0'){
    if(list[j]==' '){
      printf("\n%d. ",i);
      i++;
    }
    else{
      printf("%c",list[j]);
    }
    j++;
  }
  

  while(1){
    
    exit(1);
  }
  return 0;
}
