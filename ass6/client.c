#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <signal.h>
#include <utmp.h>
#include <pwd.h>
#include <fcntl.h>
#include "codes.h"

void mutex(int op,int smop){
    struct sembuf sop;
    sop.sem_num=0;
    sop.sem_flg=0;
    sop.sem_op=smop;
    semop(op,&sop,1);
}

int input_flag=0;

void sig_handler1(int signum){
    if(signum==SIGINT){
    	input_flag=1;
    }
}

void sig_handler2(int signum){
    if(signum==SIGINT){}
}

int main(int argc, char const *argv[])
{
	key_t msgq=MSGQ,shmp=SHMPID,shmm=SHMMSG,sem1=SEM1,sem2=SEM2,sem3=SEM3;
	int msgqid,shmpid,shmmsg,mutexpid,mutexmsg,serfd,serpid,mutexcrit,size;
    struct msqid_ds buf;
    int* sharepid;
	char* sharemsg;
    msg send_message,recv_message;
    if(access("ser.txt",F_OK)==-1){
        printf("\nser.txt does not exist\n");
        exit(0);
    }
    if((msgqid=msgget(msgq,IPC_CREAT|0666))<0){
    	perror("msgget");
    	exit(0);
    }
    if((shmpid=shmget(shmp,500,IPC_CREAT|0666))<0){
    	perror("shmget");
    	exit(0);
    }
    if((shmmsg=shmget(shmm,5000,IPC_CREAT|0666))<0){
    	perror("shmget");
    	exit(0);
    }
    sharemsg=(char*)shmat(shmmsg,NULL,0);
    sharepid=(int*)shmat(shmpid,NULL,0);
	mutexpid=semget(sem1,1,IPC_CREAT|0666);
	mutexmsg=semget(sem2,1,IPC_CREAT|0666);
    mutexcrit=semget(sem3,1,IPC_CREAT|0666);
    int my_pid=getpid();
    int pid=fork();
    int byeflag=0;
    if(pid==0){
    	mutex(mutexpid,-1);
    	size=sharepid[0];
    	sharepid[size+1]=my_pid;
    	sharepid[0]++;
    	mutex(mutexpid,1);
    	while(1){
    		msg recv_message;
    		signal(SIGINT,sig_handler2);
    		if(msgrcv(msgqid,&recv_message,MSGSIZE,my_pid,MSG_NOERROR)<0){
      			continue;
    		}
    		printf("\n%s\n",recv_message.mtext);
    	}
    }
    else{
    	while(1){
    		char msg[MSGSIZE],msgtext[MSGSIZE];
    		strcpy(msg,".");
    		signal(SIGINT,sig_handler1);
    		if(input_flag==1){
    			printf("\nEnter a message to send:");
    			scanf("%s",msg);
    			input_flag=0;
    		}
    		if(strcmp(msg,"bye")==0){
    			mutex(mutexpid,-1);
    			size=sharepid[0];
    			if(size==1)strcpy(msg,"*");
    			int i=1;
    			while(sharepid[i]!=my_pid)i++;
    			sharepid[i]=sharepid[size];
    			sharepid[0]--;
    			mutex(mutexpid,1);
    			if(strcmp(msg,"*")!=0)break;
    		}
    		sprintf(msgtext,"%d",my_pid);
    		strcat(msgtext,":");
    		strcat(msgtext,msg);
    		while(semctl(mutexmsg,0,GETVAL,0)!=0);
    		mutex(mutexcrit,-1);
    		mutex(mutexmsg,1);
    		strcpy(sharemsg,msgtext);
    		mutex(mutexmsg,1);
    		mutex(mutexcrit,1);
    	}
    }
    kill(pid,SIGKILL);
	shmdt(&sharepid);
	shmdt(&sharemsg);
	return 0;
}