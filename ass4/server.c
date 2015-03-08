#include <sys/msg.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "codes.h"
#include <string.h>
#include <time.h>

struct pid_idmap{
  pid_t pid;
  char id[100];
  struct pid_idmap *next;
};

int main(int argc, char const *argv[])
{
  int i=0;
  pid_t client_pid;
  time_t recv_time;
  int up_qid,down_qid;
  struct msqid_ds qstat;
  key_t up_key,down_key;
  msg send_message;
  struct pid_idmap *map_list,*head;
  map_list=NULL;
  head=NULL;
  char list[200];
  strcpy(list,"LIST");
  up_key=UPQ;
  down_key=DOWNQ;
  if((up_qid=msgget(up_key,IPC_CREAT|0666))<0){
    perror("msgget");
    exit(0);
  }
  if((down_qid=msgget(down_key,IPC_CREAT|0666))<0){
    perror("msgget");
    exit(0);
  }
  while(1){
    msg recv_message;
    if(msgrcv(up_qid,&recv_message,MSGSIZE,0,MSG_NOERROR)<0){
      perror("msgrcv");
      continue;
    }
    fflush(stdout);
    if(msgctl(up_qid,IPC_STAT,&qstat)<0){
      perror("msgctl");
    }
    client_pid=qstat.msg_lspid;
    recv_time=qstat.msg_stime;
    printf("\nMessage received from client %d : %s \n",client_pid,recv_message.mtext);
    if(recv_message.mtext[0]=='N'&&recv_message.mtext[1]=='E'&&recv_message.mtext[2]=='W'){
      map_list=head;
      if(head==NULL){
        head=(struct pid_idmap*)malloc(sizeof(struct pid_idmap));
        map_list=head;
        i=1;
      }
      else{
        while(map_list->next!=NULL){
          map_list=map_list->next;
        }
        map_list->next=(struct pid_idmap*)malloc(sizeof(struct pid_idmap));
        map_list=map_list->next;
        i++;
      }
      map_list->pid=client_pid;
      strcpy(map_list->id,(recv_message.mtext)+4);
      printf("\nNew Client with Chat ID %s and Process ID %d entered.\n",map_list->id,map_list->pid);
      printf("Total number of clients is %d\n",i);
      strcat(list," ");
      strcat(list,map_list->id);
      map_list->next=NULL;
      map_list=head;
      while(map_list!=NULL){
        msg msg_list;
        msg_list.mtype=map_list->pid;
        strcpy(msg_list.mtext,list);
        while(msgsnd(down_qid,&msg_list,MSGSIZE,IPC_NOWAIT)<0){
          perror("sending list");
        }
        if(msgctl(down_qid,IPC_STAT,&qstat)<0){
          perror("msgctl");
        }
        printf("\nMessage sent to client %d : %s \n",map_list->pid,list);
        map_list=map_list->next;
      }
    }
    else{
      map_list=head;
      while(map_list!=NULL&&map_list->pid!=client_pid){
        map_list=map_list->next;
      }
      if(map_list==NULL){
        printf("Error!!\n");
      }
      else{
        char mess[MSGSIZE];
        char* token;
        strcpy(mess,"MSG~");
        token=strtok(recv_message.mtext,"~");
        token=strtok(NULL,"~");
        strcat(mess,token);
        strcat(mess,"~");
        strcat(mess,ctime(&recv_time));
        if(mess[strlen(mess)-1]=='\n')
          mess[strlen(mess)-1]='\0';
        strcat(mess,"~");
        strcat(mess,map_list->id);
        token=strtok(NULL,"~");
        map_list=head;
        while(map_list!=NULL&&strcmp(token,map_list->id)!=0){
          map_list=map_list->next;
        }
        if(map_list==NULL){
          printf("Error!!\n");
        }
        else{
          send_message.mtype=map_list->pid;
          strcpy(send_message.mtext,mess);
          while(msgsnd(down_qid,&send_message,MSGSIZE,IPC_NOWAIT)<0){
            perror("sending message");
          } 
          if(msgctl(down_qid,IPC_STAT,&qstat)<0){
            perror("msgctl");
          }
          printf("\nMessage sent to client %d : %s \n",map_list->pid,mess);
        }
      }
    }
  }
  return 0;
}
