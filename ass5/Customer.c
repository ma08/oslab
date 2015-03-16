#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <sys/sem.h>

int main(int argc, char const *argv[])
{
	key_t bid=10,cid=11,mid=12,chid=13;
	int barberid,customerid,mutexid,chairsid,chairs;
	struct sembuf sop;
	barberid=semget(bid,1,IPC_CREAT|0666);
	customerid=semget(cid,1,IPC_CREAT|0666);
	mutexid=semget(mid,1,IPC_CREAT|0666);
	chairsid=semget(chid,1,IPC_CREAT|0666);
	chairs=semctl(chairsid,0,GETVAL,0);
	printf("\nThe customer enters the barber shop.\n");
	sop.sem_num=0;
	sop.sem_flg=0;
	sop.sem_op=-1;
	semop(mutexid,&sop,1);
	int no_customers=semctl(customerid,0,GETVAL,0);
	if(no_customers==chairs){
		sop.sem_op=1;
		semop(mutexid,&sop,1);
		printf("\nThere are no available chairs for waiting...Customer is exiting to find another barber shop... :-|\n\n");
		exit(0);
	}
	printf("\nCustomer sits in a vacant waiting chair...\n");
	semctl(customerid,0,SETVAL,no_customers+1);
	sop.sem_op=1;
	semop(mutexid,&sop,1);
	printf("\nWaiting for the barber to be free...\n");
	sop.sem_op=-1;
	semop(barberid,&sop,1);
	printf("\nGetting a haircut finally!! Yayy!\n");
	sleep(5);
	printf("\nExiting the barber shop after getting a nice haircut :)\n\n");
	return 0;
}