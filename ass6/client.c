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
        printf("\n<Ctrl-C> is pressed\n");
    	input_flag=1;
    }
}

int main(int argc, char const *argv[])
{
	key_t msgq=MSGQ,shmp=SHMPID,shmm=SHMMSG,sem1=SEM1,sem2=SEM2;
	int msgqid,shmpid,shmmsg,mutexpid,mutexmsg,size,my_pid;
    int* sharepid;
	char* sharemsg;
    msg recv_message;
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
    my_pid=getpid();
    int pid=fork();
    if(pid==0){
    	mutex(mutexpid,-1);
    	size=sharepid[0];
    	sharepid[size+1]=my_pid;
    	sharepid[0]++;
    	mutex(mutexpid,1);
    	while(1){
    		signal(SIGINT,SIG_IGN);
    		if(msgrcv(msgqid,&recv_message,MSGSIZE,my_pid,MSG_NOERROR)<0)continue;
    		printf("\nReceived message:\"%s\"\n",recv_message.mtext);
    	}
    }
    else{
        printf("\nYou entered the conference. Press <Ctrl-C> to send a message.\n");
    	while(1){
    		char msg[MSGSIZE],msgtext[MSGSIZE],pidtext[MSGSIZE];
    		strcpy(msg,".");
    		signal(SIGINT,sig_handler1);
    		if(input_flag==1){
    			printf("\nEnter your message:");
    			scanf("%s",msg);
    			input_flag=0;
    		}
    		if(strcmp(msg,"bye")==0){
    			mutex(mutexpid,-1);
    			size=sharepid[0];
    			if(size==1)strcpy(msg,"*");
    			int i=1;
    			while(sharepid[i]!=my_pid)i++;
    			sharepid[i]=0;
                sharepid[0]--;
    			mutex(mutexpid,1);
    			if(strcmp(msg,"*")!=0)break;
    		}
    		sprintf(pidtext,"%d",my_pid);
            strcpy(msgtext,getlogin());
            strcat(msgtext,"/");
            strcat(msgtext,pidtext);
    		strcat(msgtext,":");
    		strcat(msgtext,msg);
    		while(semctl(mutexmsg,0,GETVAL,0)>0);
    		mutex(mutexmsg,1);
    		strcpy(sharemsg,msgtext);
    		mutex(mutexmsg,1);
            if(strcmp(msg,"*")==0)break;
    	}
    }
    kill(pid,SIGKILL);
	shmdt(&sharepid);
	shmdt(&sharemsg);
    printf("\nLeaving conference\n");
	return 0;
}