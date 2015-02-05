#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char const *argv[])
{
	int i;
	freopen("childoutput","w",stdout);
	for (i = 0; i < argc; ++i)
	{
		printf("%s\n",argv[i]);
	}
	return 0;
}