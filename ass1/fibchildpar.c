#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	int n,i,status,pid1,*a,j;
	printf("Enter the number of fibonacci numbers required : ");
	scanf("%d",&n);
	a=(int*)malloc(n*sizeof(int));
	pid_t * p_array = (pid_t *)malloc(sizeof(pid_t)* n);
	for(i=0;i<n;++i)
	{
		if(pid1=fork()){
      p_array[i]=pid1;
			continue;
		}
		else {
			int c=0,b=1,temp;
			for(j=0;j<i;j++){
				temp=b;
				b=b+c;
				c=temp;
			}
			exit(b);
		}
	}
	for(i=0;i<n;i++){
    waitpid(p_array[i],&status,0);
    status = WIFEXITED(status)?WEXITSTATUS(status):-1;
    /*status>>=8;*/
    a[i]=status;
		printf("fib(%d) = %d\n",i+1,a[i]);
	}
	return 0;
}
