#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char const *argv[])
{
	int i;
	freopen("childoutput","w",stdout);
	int pipe_value[2];
	pipe_value[0]=strtol(argv[1],NULL,10);
	pipe_value[1]=strtol(argv[2],NULL,10);
	for (i = 0; i < 2; ++i)
	{
		printf("%d\n",pipe_value[i]);
	}
	return 0;
}