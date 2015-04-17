#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int FIFO(int n_frames, int ref[], int size){
  int *arrival_time = 
  return 0;
}
int main(int argc, char *argv[])
{
  int n_frames;
  FILE *fp;
  char *token;
  char line[1000];
  int ref[1000];
  fp = fopen("input.txt", "r");
  if (fp == NULL){
    printf("ERROR OPENING FILE");
    exit(1);
  }

  int size=0;
  while (fgets(line, sizeof(line), fp)!=NULL)
  {
    token = strtok(line, " ");
    while (token != NULL){
      /*printf("\n%s",token);*/
      ref[size++]=atoi(token);
      token = strtok(NULL, " ");
    }
  }
  /*int i;
  for (i = 0; i < size; ++i)
  {
    printf("%d ",ref[i]);
    
  }*/
  return 0;
}


