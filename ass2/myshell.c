#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>

int main(int argc, char *argv[])
{
  char cwd[50];
  char input[50];
  if(cwd!=getcwd(cwd,50)){
    perror("getcwd");
    /*printf("%d error",errno);*/
  }
  int length;
  while(1){
    printf("%s>",cwd);
    fgets(input,50,stdin);
    length=strlen(input);
    if(input[length-1]=='\n')
      input[length-1]='\0';
    /*printf("%s",input);*/
    /*scanf("%s",input);*/
  }

  return 0;
}
