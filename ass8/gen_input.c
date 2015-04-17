#include<stdio.h>
#include<stdlib.h>

//get relative page number
int gReP(int i,int j,int n,int d,int start){
  return start+(i*n+j)/d;
}

//takes matrix dimension and number of integers
//per page as commandline arguments
int main(int argc, char *argv[])
{
  int n=2;
  int d=2;
  if(argc>=3){
    n=atoi(argv[1]);
    d=atoi(argv[2]);
  }
  int matrix_A_start=101;
  /*int matrix_B_start=101+(n*n)/d+100;*/
  int matrix_B_start=501;
  int matrix_C_start=1001;
  /*int matrix_C_start=matrix_B_start+(n*n)/d+100;*/
  int i,j,k;
  for (i = 0; i < n; ++i)
    for (j = 0; j < n; ++j)
    {
      for (k = 0; k < n; ++k)
        printf("%d %d ",gReP(i,k,n,d,matrix_A_start),gReP(k,j,n,d,matrix_B_start));
      printf("%d ",gReP(i,j,n,d,matrix_C_start));
    }
  return 0;
}

