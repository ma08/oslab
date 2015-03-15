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
	srand(time(NULL));
	key_t key1=111,key2=121,key3=131,key4=141,key5=151;
	int semid1,semid2,semid3,semid4,semid5,i,j;
	struct sembuf sop;
	semid1=semget(key1,3,IPC_CREAT|0666);
	semid2=semget(key2,3,IPC_CREAT|0666);
	semid3=semget(key3,3,IPC_CREAT|0666);
	semid4=semget(key4,3,IPC_CREAT|0666);
	semid5=semget(key5,3,IPC_CREAT|0666);
	for(i=0;i<3;++i){
		semctl(semid1,i,SETVAL,50);
		semctl(semid2,i,SETVAL,0);
		semctl(semid3,i,SETVAL,0);
		semctl(semid4,i,SETVAL,0);
		semctl(semid5,i,SETVAL,0);
	}
	printf("\nThe meat pits are ready and full of meat!!\n");
	int meat_pit=((rand()%1000)%3)+1;
	int tries=0;
	while(1){
		printf("\nThe ranger is requesting control of meat pit %d\n",meat_pit);
		int available=semctl(semid1,meat_pit-1,GETVAL,0);
		int no_lions=semctl(semid3,meat_pit-1,GETVAL,0);
		int no_jackals=semctl(semid4,meat_pit-1,GETVAL,0);
		sop.sem_num=meat_pit-1;
		sop.sem_flg=0;
		if(available<=40&&no_jackals==0&&no_lions==0){
			printf("\nMeat in meat pit %d is %d\n",meat_pit,available);
			sop.sem_op=1;
			semop(semid2,&sop,1);
			printf("\nThe ranger is granted control of meat pit %d!! Yayy!!\n",meat_pit);
			semctl(semid1,meat_pit-1,SETVAL,available+10);
			sleep(1);
			sop.sem_op=-1;
			printf("\nThe ranger leaves meat pit %d.\n",meat_pit);
			semop(semid2,&sop,1);
			printf("\nGiving a signal to all wait queues of meat pits. (By using mutex semaphore)\n");
			for(i=0;i<3;++i){
				if(semctl(semid5,i,GETVAL,0)==0){
					sop.sem_num=i;
					sop.sem_op=1;
					semop(semid5,&sop,1);
				}
			}
			meat_pit=((rand()%1000)%3)+1;
		}
		else{
			printf("\nThe ranger is denied access to meat pit %d. :(\n",meat_pit);
			if(available>40)printf("\nThe meat pit %d has more than 40 units in it!\n",meat_pit);
			else if(no_lions>0)printf("\nThere are lions in meat pit %d. :(\n",meat_pit);
			else if(no_jackals>0)printf("\nThere are jackals in meat pit %d. :(\n",meat_pit);
			tries++;
			if(tries==3){
				printf("\nThe ranger tried all the meat pits. Joining the wait queue on the meat pit %d ...\n",meat_pit);
				tries=0;
				sop.sem_num=meat_pit-1;
				sop.sem_op=-1;
				semop(semid5,&sop,1);
			}
			else{
				meat_pit=(meat_pit+1)%4;
				if(meat_pit==0)meat_pit=1;
			}
		}
	}
	return 0;
}