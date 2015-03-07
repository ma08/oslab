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
	msg* message;
	while(1){
		message=(msg*)malloc(sizeof(msg));
		if(msgrcv(up_qid,message,MSGSIZE,0,MSG_NOERROR|IPC_NOWAIT)<0){
			perror("msgrcv");
			continue;
		}
		if(msgctl(up_qid,IPC_STAT,&qstat)<0){
			perror("msgctl");
		}
		client_pid=qstat.msg_lspid;
		if(message->mtext[0]=='N'&&message->mtext[1]=='E'&&message->mtext[2]=='W'){
			while(map_list!=NULL){
				map_list=map_list->next;
			}
			map_list=(struct pid_idmap*)malloc(sizeof(struct pid_idmap));
			map_list->pid=client_pid;
			strcpy(map_list->id,(message->mtext)+4);
			strcat(list,map_list->id);
			strcat(list," ");
			map_list->next=NULL;
			if(head==NULL)head=map_list;
			map_list=head;
			while(map_list!=NULL){
				msg* msg_list;
				msg_list=(msg*)malloc(sizeof(msg));
				msg_list->mtype=map_list->pid;
				strcpy(msg_list->mtext,list);
				msgsnd(down_qid,&msg_list,200,IPC_NOWAIT);
			}
		}
	}
	return 0;
}