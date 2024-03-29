#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	srand(time(NULL));
	key_t key1=111,key2=121,key3=131,key4=141,key5=151,key6=161;
	int semid1,semid2,semid3,semid4,semid5,semid6,i,j,n,t,iterations=0,tries=0;
	struct sembuf sop;
	semid1=semget(key1,3,IPC_CREAT|0666);
	semid2=semget(key2,3,IPC_CREAT|0666);
	semid3=semget(key3,3,IPC_CREAT|0666);
	semid4=semget(key4,3,IPC_CREAT|0666);
	semid5=semget(key5,3,IPC_CREAT|0666);
	semid6=semget(key6,1,IPC_CREAT|0666);
	printf("\nEnter the number of Lions : ");
	scanf("%d",&n);
	printf("\nEnter the number of iterations for each lion : ");
	scanf("%d",&t);
	pid_t pid;
	for(i=0;i<n;++i){
		sleep(1);
		printf("\nLion %d enters the arena!!! :D \n",i+1);
		pid=fork();
		if(pid==0){
			srand(time(NULL));
			int meat_pit=((rand()%1000)%3)+1;
			while(iterations<t){
				iterations++;
				printf("\nLion %d is requesting control of meat pit %d...\n",i+1,meat_pit);
				int available=semctl(semid1,meat_pit-1,GETVAL,0);
				int no_rangers=semctl(semid2,meat_pit-1,GETVAL,0);
				int no_jackals=semctl(semid4,meat_pit-1,GETVAL,0);
				int no_lions=semctl(semid3,meat_pit-1,GETVAL,0);
				sop.sem_num=meat_pit-1;
				sop.sem_flg=0;
				if(available>0&&no_jackals==0&&no_rangers==0){
					sop.sem_op=1;
					semop(semid3,&sop,1);
					printf("\nLion %d is granted control of meat pit %d!!\n\nThe meat in the pit is %d\n",i+1,meat_pit,available);
					sop.sem_op=-1;
					semop(semid1,&sop,1);
					printf("\nLion %d eats a unit of meat from meat pit %d\n",i+1,meat_pit);
					sleep(1);
					printf("\nLion %d leaves meat pit %d.\n",i+1,meat_pit);
					semop(semid3,&sop,1);
					if(no_lions==0&&available>1){
						printf("\nLion %d sends a signal to wait queues of all meat pits.\n",i+1);
						for(j=0;j<3;j++){
							if(semctl(semid5,j,GETVAL,0)==0){
								semctl(semid6,0,SETVAL,meat_pit);
								sop.sem_num=j;
								sop.sem_op=1;
								semop(semid5,&sop,1);
							}
						}
					}
					else meat_pit=((rand()%1000)%3)+1;
				}
				else {
					printf("\nLion %d is denied access to meat pit %d. :(\n",i+1,meat_pit);
					if(available==0)printf("\nThere is no meat in the meat pit %d ... :(\n",meat_pit);
					else if(no_rangers>0)printf("\nThere are rangers in meat pit %d. :(\n",meat_pit);
					else if(no_jackals>0)printf("\nThere are jackals in meat pit %d. :(\n",meat_pit);
					tries++;
					if(tries==3){
						printf("\nLion %d tried all the meat pits. Joining the wait queue on the meat pit %d ...\n",i+1,meat_pit);
						tries=0;
						sop.sem_num=meat_pit-1;
						sop.sem_op=-1;
						semop(semid5,&sop,1);
						meat_pit=semctl(semid6,0,GETVAL,0);
						printf("\nGot a wake up signal from meat pit %d \n",meat_pit);
					}
					else{
						meat_pit=(meat_pit+1)%4;
						if(meat_pit==0)meat_pit=1;
					}
				}
			}
			printf("\nLion %d exits the arena... :'( \n\n",i+1);
			exit(0);
		}
	}
	for(i=0;i<n;i++)wait(NULL);
	return 0;
}