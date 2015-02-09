#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include "codes.h"

#define N_INF -100000

int write_to_parent(int sig, int write_end){
  int nbytes;
  char *buf=(char *)(malloc(sizeof(char)*20));
  sprintf(buf,"%d",sig);
  nbytes=strlen(buf)+1;
  nbytes=write(write_end, buf, nbytes); 
  return nbytes;
}

int drop_numbers(int **data, int n){
  int i,j=0;
  int *arr=*data;
  int count=0;
  for(i=0;i<n;++i)
  {if(arr[i]!=N_INF)
    count++;
  }
  int *final_arr=(int *)(malloc(sizeof(final_arr)*count));
  for(i=0;i<n;++i)
  {if(arr[i]!=N_INF){
      final_arr[j++]=arr[i];
    }
  }
  free(arr);
  *data=final_arr;
  return count;
}

int main(int argc, char const *argv[])
{
  srand(time(NULL));
	int i,j,ret;
  int num_greater;
  int sig;
  int id,num;
  int pivot;
  int nbytes;
  FILE *fp;
  int *data;
  data=(int *)(malloc(sizeof(int)*5));
  int size_data=0;
  const int BSIZE = 100;
  char buf[BSIZE];
  char outputfilename[20];
  char datafilename[20];
  strcpy(datafilename,"data_");
	int read_end=strtol(argv[1],NULL,10);
	int write_end=strtol(argv[2],NULL,10);
  nbytes = read(read_end, buf, BSIZE);
  strcat(datafilename,buf);
  strcat(datafilename,".txt");
  id=strtol(buf,NULL,10);
  if((fp=fopen(datafilename,"r+"))!=NULL){
    for(i=0;i<5;++i)
    {ret=fscanf(fp, "%d", &num);
      if(ret!=EOF){
        size_data++;
        data[i]=num;
      }
      else{
        perror("Erroor");
      }
    }
    write_to_parent(READY,write_end);
    strcpy(buf,"oo");
    while(1){
      errno=0;
      nbytes = read(read_end, buf, 5);
      sig=strtol(buf,NULL,10);
      switch(sig){
        case REQUEST:
          if(size_data>0){
            while(data[(num=rand()%size_data)]==N_INF){
            };
            write_to_parent(data[num],write_end);
          }else{
            write_to_parent(-1,write_end);
          }
          break;
        case PIVOT:
          nbytes = read(read_end, buf, 5);
          sig=strtol(buf,NULL,10);
          pivot=sig;
          num_greater=0;
          for(i=0;i<size_data;++i)
          {if(data[i]>pivot)
              num_greater++;
          }
          write_to_parent(num_greater,write_end);
          break;
        case SMALL:
          for(i=size_data-1;i>=0;--i)
          {if(data[i]<pivot){
              data[i]=N_INF;
            }
          }
          size_data=drop_numbers(&data,size_data);
          break;
        case LARGE:
          for (i=size_data-1;i>=0;--i)
          {if(data[i]>pivot){
              data[i]=N_INF;
            }
          }
          size_data=drop_numbers(&data,size_data);
          break;
        case EXIT:
          return 0;
      }
    }
  }
  else{
    perror(datafilename);
    exit(1);
  }
	return 0;
}