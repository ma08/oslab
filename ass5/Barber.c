#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <sys/sem.h>

int main(int argc, char const *argv[])
{
	key_t bid=10,cid=11,mid=12;
	int barberid,customerid,mutexid,n,waiting=0;
	struct sembuf sop;
	barberid=semget(bid,1,IPC_CREAT|0666);
	customerid=semget(cid,1,IPC_CREAT|0666);
	mutexid=semget(mid,1,IPC_CREAT|0666);
	printf("%d %d %d\n",barberid,customerid,mutexid);
	semctl(barberid,0,SETVAL,0);
	semctl(customerid,0,SETVAL,0);
	semctl(mutexid,0,SETVAL,2);
	printf("\nThe barber enters the shop at the beginning of the day..\n");
	while(1){
		int no_customers=semctl(customerid,0,GETVAL,0);
		if(no_customers==0){
			printf("\nThere are no customers.. The barber goes to sleep...zzzz\n");
			sop.sem_num=0;
			sop.sem_flg=1;
			sop.sem_op=-1;
			semop(customerid,&sop,1);
		}
		printf("\nWoah!! A customer is waiting for the barber!!\n");
		semop(mutexid,&sop,1);
		no_customers=semctl(customerid,0,GETVAL,0);
		semctl(customerid,0,SETVAL,no_customers-1);
		printf("\nBarber getting ready to cut the hair..\n");
		sop.sem_op=1;
		semop(barberid,&sop,1);
		printf("\nThe barber cuts the hair of the customer.\n");
		sleep(5);
		semop(mutexid,&sop,1);
		printf("\nCutting of hair is completed.. Customer exits.\n");
	}
	return 0;
}