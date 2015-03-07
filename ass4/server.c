#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "codes.h"

int main(int argc, char const *argv[])
{
	int up_qid,down_qid;
	struct msqid_qs qstat;
	key_t up_key,down_key;
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
	msg message;
	while(1){
		if(msgrcv())
	}
	return 0;
}