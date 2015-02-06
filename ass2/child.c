#include <unistd.h>
#include<string.h>
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
  for (i = 0; i < n; ++i)
  {
    if(arr[i]!=N_INF)
      count++;
  }
  int *final_arr=(int *)(malloc(sizeof(final_arr)*count));
  for (i = 0; i < n; ++i)
  {
    if(arr[i]!=N_INF){
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
  strcpy(outputfilename,"childoutput");
  strcpy(datafilename,"data");
	/*int pipe_value[2];*/
	int read_end=strtol(argv[1],NULL,10);
	int write_end=strtol(argv[2],NULL,10);
  
  /*close(write_end);*/
  nbytes = read(read_end, buf, BSIZE);   
  /*printf("--%s\n",buf);*/
  /*perror("");*/
  /*close(read_end);*/

  strcat(outputfilename,buf);
  strcat(datafilename,buf);
  strcat(datafilename,".txt");
  /*printf("\n---%s---\n",datafilename);*/
  freopen(outputfilename,"w",stdout);
  /*printf("%d\n",read_end);*/
  /*printf("%d\n",write_end);*/
  /*for (i = 0; i < 2; ++i)
  {
    printf("%d\n",pipe_value[i]);
  }*/
  id=strtol(buf,NULL,10);
  /*printf("\n--%d\n",id);*/
  /*printf("%d\n",id);*/
  /*sprintf(buf,"%d",1000);*/
  /*while(read(pipe_value[1],buf,BSIZE)<=0)perror("hoo");*/
  /*if(write(pipe_value[1], buf, BSIZE)<=0)
    perror("hoo"); */
  /*nbytes = strlen(buf)+1;*/
  /*close(read_end);*/
  /*printf("----writing %d----",write_end);*/
  /*write(write_end, buf, nbytes);*/
  /*if(errno!=0)
    perror("hoo");
  close(write_end);*/

  if((fp = fopen(datafilename, "r+")) != NULL) {
    /*printf("wooooooooo");*/
    /*perror(datafilename);*/
    for (i = 0; i < 5; ++i)
    {
      ret=fscanf(fp, "%d", &num);
      if(ret!=EOF){
        size_data++;
        data[i]=num;
      }else{
        perror("Erroor");
      }
    }
    /*for (i = 0; i < 5; ++i)
      printf("%d ",data[i]);*/

    /*sprintf(buf,"%d",READY);
    nbytes = strlen(buf)+1;
    write(write_end, buf, nbytes);*/

    write_to_parent(READY,write_end);
    /*close(write_end);*/
    strcpy(buf,"oo");

    /*for (j = 0; j < 2; ++j)*/
    /*{*/
      
    /*}*/
    while(1){
    /*if(id==2){*/

      errno=0;
      nbytes = read(read_end, buf, 5);   
      /*while(read(read_end, buf, 5)==0);*/
      /*perror("");*/
      /*printf("\n---%d--%s--\n",id,buf);*/
      /*close(read_end);*/
      sig=strtol(buf,NULL,10);
      switch (sig) {
        case REQUEST:
          if(size_data>0){
            
            while(data[(num=rand()%size_data)]==N_INF){
              
            };
            write_to_parent(data[num],write_end);
            /*sprintf(buf,"\n%d",data[rand()%size_data]);*/
          }else{
            write_to_parent(-1,write_end);
            /*sprintf(buf,"\n-1");*/
          }
          /*nbytes = strlen(buf)+1;*/
          /*write(write_end, buf, nbytes);*/
          /*printf("hoooooooooo");*/
          break;
        case PIVOT:
          nbytes = read(read_end, buf, 5);   
          /*printf("\n--%d-%s--\n",id,buf);*/
          sig=strtol(buf,NULL,10);
          pivot=sig;
          /*printf("\npivot is %d\n",pivot);*/
          num_greater=0;
          for (i = 0; i < size_data; ++i)
          {
            if(data[i]>pivot)
              num_greater++;
            
          }
          /*sprintf(buf,"\n%d",num_greater);*/
          /*nbytes = strlen(buf)+1;*/
          /*write(write_end, buf, nbytes);*/
          write_to_parent(num_greater,write_end);
          /*perror("");*/
          break;
        case SMALL:
          for (i = size_data-1; i >=0; --i)
          {
            if(data[i]<pivot){
              data[i]=N_INF;
            }
          }
          size_data=drop_numbers(&data,size_data);
          break;
        case LARGE:
          for (i = size_data-1; i >= 0; --i)
          {
            if(data[i]>pivot){
              data[i]=N_INF;
            }
          }
          size_data=drop_numbers(&data,size_data);
          break;
        case EXIT:
          return 0;
        /*default:*/
      }
    /*}*/
    }
  }else{
    perror(datafilename);
    exit(1);
  }

	return 0;
}
