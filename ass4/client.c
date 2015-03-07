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
  char *token;
  char msg_text[100];
  char msg_time[100];
  char sender_id[100];
  char client_list[20][10];
  char chat_id[50];
  printf("\nEnter id:");
  scanf("%s",chat_id);
  int i=0,j=0,k=0;

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
  strcat(buf,chat_id);

  msg* cur_msg= (msg *)(malloc(sizeof(msg)));
  msg get_msg;
  strcpy(cur_msg->mtext,buf);
  cur_msg->mtype=pid;
  while(msgsnd(up_qid,(void *)cur_msg,MSGSIZE,IPC_NOWAIT  ) < 0){
    perror("connecting");
    /*exit(1);*/
  }
  /*exit(1);*/
  /*printf("\n%d ",down_qid);*/
  while(1){
    while(msgrcv(down_qid,&get_msg,200,pid,MSG_NOERROR|IPC_NOWAIT)<0){
      /*perror("receiving");*/
    }
    if(get_msg.mtext[0]=='L'&&get_msg.mtext[1]=='I'){
      i=0;
      strcpy(list,get_msg.mtext+5);
      if(get_msg.mtext[0]=='L'&&get_msg.mtext[1]=='I'){
        strcpy(list,get_msg.mtext+5);
      }
      token=strtok(list," ");
      if(strcmp(token,chat_id)!=0){
        strcpy(client_list[i++],token);
      }
      while((token=strtok(NULL," "))!=NULL){
        if(strcmp(token,chat_id)!=0){
          strcpy(client_list[i++],token);
        }
      }
    }else if(get_msg.mtext[0]=='M'&&get_msg.mtext[1]=='S'){
      strcpy(buf,get_msg.mtext+4);
      token=strtok(buf,"~");
      strcpy(msg_text,token);
      /*printf("--- %s \n",msg_text);*/
      token=strtok(NULL,"~");
      strcpy(msg_time,token);
      token=strtok(NULL,"~");
      strcpy(sender_id,token);
      printf("\n\n(%s) %s: %s",msg_time,sender_id,msg_text);
    }
    printf("\nList of clients");
    for (j = 0; j < i; j++) {

      printf("\n %d.%s ",j+1,client_list[j]);
    }
    printf("\nEnd of List of clients");
    if(i>=1){
      printf("\nDo you want to send a message?\nEnter 0 to not send a message:");
      scanf("%d",&k);
      if(k!=0){
        j=-1;
        /*printf("\n%d\n",i);*/
        while(!(j>=1&&j<=i)){
          printf("\nEnter client number to send message:");
          scanf("%d",&j);
          /*printf("\n%d",j);*/
        }  
        char c;

        while((c = getchar()) != '\n' && c != EOF)
          /* discard */ ;
        strcpy(buf,"MSG~");
        msg_text[0]='\n';
        msg_text[1]='\0';
        while(msg_text[0]=='\0'||strcmp(msg_text,"\n")==0){
          printf("\nEnter message to send:");
          fgets(msg_text,100,stdin);
        }
        if(msg_text[strlen(msg_text)-1]=='\n'){
          msg_text[strlen(msg_text)-1]='\0';
        }
        /*printf("\n\n%s",msg_text);*/
        strcat(buf,msg_text);
        strcat(buf,"~");
        strcat(buf,client_list[j-1]);
        cur_msg= (msg *)(malloc(sizeof(msg)));
        cur_msg->mtype=pid;
        strcpy(cur_msg->mtext,buf);
        while(msgsnd(up_qid,(void *)cur_msg,MSGSIZE,IPC_NOWAIT  ) < 0){
          perror("sending message");
          /*exit(1);*/
        }
      }
    }
    fflush(stdout);
  }




  /*printf("\n 1. ");
    i++;
    while(list[j]!='\0'){
    if(list[j]==' '){
    printf("\n %d. ",i);
    i++;
    }
    else{
    printf("%c",list[j]);
    }
    j++;
    }*/
  /*printf("\nEnter ")*/
  return 0;
}
