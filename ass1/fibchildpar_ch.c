#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	int n,i,status,pid1,*a,j;
	printf("\nEnter the number of fibonacci numbers required : ");
	scanf("%d",&n);
	a=(int*)malloc(n*sizeof(int));
	a[0]=1;
	a[1]=1;
	printf("-----------------------------------------------\nfib(1) = 1\nfib(2) = 1\n");
	for(i=2;i<n;++i)
	{
		if(pid1=fork()){
			continue;
		}
		else {
			int c=1,b=1,temp;
			for(j=0;j<i-1;j++){
				temp=c;
				c=b;
				b=temp+b;
			}
			sleep(i);
			printf("fib(%d) = %d\n",i+1,b);
			exit(b);
		}
	}
	sleep(n);
	return 0;
}
