#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	int n,i,status,pid1,j;
	printf("Enter the number of fibonacci numbers required : ");
	scanf("%d",&n);
	for(i=0;i<n;++i)
	{
		if(pid1=fork()){
			wait(&status);
			continue;
		}
		else {
			int a=1,b=1,temp;
			for(j=0;j<i;j++){
				temp=a;
				a=b;
				b=temp+b;
			}
			printf("fib(%d) : %d\n",i+1,b);
			exit(b);
		}
	}
	return 0;
}