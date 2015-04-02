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

int main(int argc, char const *argv[])
{
	key_t msgq=MSGQ,shmp=SHMPID,shmm=SHMMSG,sem1=SEM1,sem2=SEM2;
	int msgqid,shmpid,shmmsg,mutexpid,mutexmsg,serfd,serpid;
    FILE* utmpfp;
    int* sharepid;
    char* sharemsg;
    char sername[10],servpid[7],name[20],readmsg[MSGSIZE],sendmsg[MSGSIZE];
    struct msqid_ds buf;
    msg send_message,recv_message;
    strcpy(sername,"ser.txt");
    if(access(sername,F_OK)!=-1){
        printf("\nser.txt already exists\n");
        exit(0);
    }
    else{
        FILE* f=fopen(sername,"w");
        serpid=getpid();
        sprintf(servpid,"%d",serpid);
        fwrite(servpid,sizeof(servpid),1,f);
        fclose(f);
    }
	if(argc==1){
        printf("\nAllowing all logins presently\n");
        /*printf("\nEnter the list of allowed logins\n");
        exit(0);*/
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
    sharepid[0]=0;
	mutexpid=semget(sem1,1,IPC_CREAT|0666);
	mutexmsg=semget(sem2,1,IPC_CREAT|0666);
	semctl(mutexpid,0,SETVAL,1);
	semctl(mutexmsg,0,SETVAL,0);
    printf("\nInitialisation Complete\n");
    utmpfp=fopen("/var/run/utmp","r");
    struct utmp u;
    while(fread(&u,sizeof(u),1,utmpfp)>0){
        if(getpwnam(u.ut_name)==NULL)continue;
        printf("Sending commence notification to %s\n",u.ut_line);
        sprintf(name,"./commence > /dev/%s",u.ut_line);
        system(name);
    }
    while(1){
        while(semctl(mutexmsg,0,GETVAL,0)<2);
        strcpy(readmsg,sharemsg);
        mutex(mutexmsg,-1);
        mutex(mutexmsg,-1);
        char *token1,*token2,*token3,*saveptr;
        strcpy(sendmsg,readmsg);
        printf("\nReceived message \"%s\"\n",sendmsg);
        token1=strtok_r(readmsg,"/",&saveptr);
        token2=strtok_r(NULL,":",&saveptr);
        token3=saveptr;
        int senderpid=strtol(token2,NULL,10);
        if(strcmp(token3,".")==0)continue;
        else if(strcmp(token3,"*")==0)break;
        else {
            int readpids[20],i=1,size,real_size=0;
            mutex(mutexpid,-1);
            size=sharepid[0];
            for(i=1;i<size+1;i++){
                if(sharepid[i]!=0){readpids[i-1]=sharepid[i];
                    real_size++;}
            }
            mutex(mutexpid,1);
            i=0;
            for(i=0;i<real_size;i++){
                if(readpids[i]==senderpid)continue;
                strcpy(send_message.mtext,sendmsg);
                send_message.mtype=readpids[i];
                printf("\nSending message to pid %d\n",sendmsg,readpids[i]);
                while(msgsnd(msgqid,&send_message,MSGSIZE,IPC_NOWAIT)<0){
                    perror("sending message");
                }
            }
        }
    }
    remove(sername);
    int i=msgctl(msgqid,IPC_RMID,&buf);
    i=semctl(mutexpid,0,IPC_RMID);
    i=semctl(mutexmsg,0,IPC_RMID);
	shmdt(&sharepid);
	shmdt(&sharemsg);
    printf("\nTerminating Conference\n");
	return 0;
}