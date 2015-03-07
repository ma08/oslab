#include <sys/msg.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "codes.h"
#include <string.h>

struct pid_idmap{
	pid_t pid;
	char id[100];
	struct pid_idmap *next;
};

int main(int argc, char const *argv[])
{
  int i=0;
	pid_t client_pid;
	int up_qid,down_qid;
	struct msqid_ds qstat;
	key_t up_key,down_key;
	struct pid_idmap *map_list,*head;
	map_list=NULL;
	head=NULL;
	char list[200];
	strcpy(list,"LIST ");
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
		if(msgrcv(up_qid,&recv_message,MSGSIZE,0,IPC_NOWAIT)<0){
      /*perror("msgrcv");*/
			continue;
		}
    /*printf("woo");*/
		if(msgctl(up_qid,IPC_STAT,&qstat)<0){
			perror("msgctl");
		}
		client_pid=qstat.msg_lspid;
		if(recv_message.mtext[0]=='N'&&recv_message.mtext[1]=='E'&&recv_message.mtext[2]=='W'){
			map_list=head;
      if(head==NULL){
        head=(struct pid_idmap*)malloc(sizeof(struct pid_idmap));
        map_list=head;
      }
      else{
        while(map_list->next!=NULL){
          map_list=map_list->next;
        }
        map_list->next=(struct pid_idmap*)malloc(sizeof(struct pid_idmap));
        map_list=map_list->next;
      }
			
			map_list->pid=client_pid;
			strcpy(map_list->id,(recv_message.mtext)+4);

			strcat(list,map_list->id);
			strcat(list," ");
			map_list->next=NULL;

			map_list=head;
      
			while(map_list!=NULL){
        /*printf("doo");*/
				msg* msg_list;
				msg_list=(msg*)malloc(sizeof(msg));
				msg_list->mtype=map_list->pid;
        /*printf("\n%d",map_list->pid);*/
				strcpy(msg_list->mtext,list);
        while(msgsnd(down_qid,msg_list,MSGSIZE,IPC_NOWAIT)<0){
          perror("sending list");
        }
        map_list=map_list->next;
			}
      /*printf("\n%d ",down_qid);*/
      /*printf("ending");*/
		}
	}
	return 0;
}
