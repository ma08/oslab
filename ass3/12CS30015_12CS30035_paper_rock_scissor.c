#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>

int **ctop_ids;   // pipes as global variables to be able to be accessed in signal handling functions

int write_to_parent(int sig, int write_end){   /* function to write into the parent from the child. */
    int nbytes;
    char *buf=(char *)(malloc(sizeof(char)*20));
    sprintf(buf,"%d",sig);
    nbytes=strlen(buf)+1;
    nbytes=write(write_end, buf, nbytes); 
    return nbytes;
}

void sig_handler1(int signum){          /*  Signal handler function for child 1 */
    if(signum==SIGINT){
        int rps1=(rand()%100)%3+1;
        close(ctop_ids[0][0]);
        write_to_parent(rps1,ctop_ids[0][1]);
    }
    else if(signum==SIGKILL){
        exit(0);
    }
}

void sig_handler2(int signum){        /*  Signal handler function for child 2 */
    if(signum==SIGINT){
        int rps2=(rand()%1000)%3+1;
        close(ctop_ids[1][0]);
        write_to_parent(rps2,ctop_ids[1][1]);
    }
    else if(signum==SIGKILL){
        exit(0);
    }
}

void printmove(int move){
    if(move==1)printf("Rock.\n");
    if(move==2)printf("Paper.\n");
    if(move==3)printf("Scissor.\n");
}

int main(int argc, char *argv[])
{
    srand(time(NULL));            //seeding the rand function
    char buf2[100];               // buffers to send in the pipes
    char buf3[100];
    pid_t pid1,pid2;
    int i;
    ctop_ids=(int**)(malloc(sizeof(int*)*2));
      for(i=0;i<2;++i){
        ctop_ids[i]=(int*)(malloc(sizeof(int)*2));
    } 
    for(i=0;i<2;++i){
        pipe(ctop_ids[i]);                     //creating pipes for both the children
    }
    if((pid1=fork())==0){               // child process 1
        signal(SIGINT,sig_handler1);
        for(;;);
    }
    else {
        if((pid2=fork())==0){             // child process 2
            signal(SIGINT,sig_handler2);
            for(;;);
        }
        else{
            int rps1,rps2;
            float score1=0.0,score2=0.0;
            while(score1<=10&&score2<=10){        // while loop for the iterations
                sleep(1);                             //parent process sleeps for 1 second to help the child processes catch the signal
                kill(pid1,SIGINT);
                kill(pid2,SIGINT);
                close(ctop_ids[0][1]);
                read(ctop_ids[0][0],buf2,sizeof(buf2));     // reading the move from the children
                rps1=strtol(buf2,NULL,10);
                close(ctop_ids[1][1]);
                read(ctop_ids[1][0],buf3,sizeof(buf3));
                rps2=strtol(buf3,NULL,10);
                printf("Child 1 selected ");
                printmove(rps1);
                printf("Child 2 selected ");
                printmove(rps2);
                if(rps1==1){
                    if(rps2==2)score2+=1;
                    else if(rps2==3)score1+=1;
                    else {
                        score1+=0.5;
                        score2+=0.5; 
                    }
                }
                else if(rps1==2){
                    if(rps2==3)score2+=1;
                    else if(rps2==1)score1+=1;                // updating the score
                    else {
                        score1+=0.5;
                        score2+=0.5; 
                    }
                }
                else if(rps1==3){
                    if(rps2==1)score2+=1;
                    else if(rps2==2)score1+=1;
                    else {
                        score1+=0.5;
                        score2+=0.5; 
                    }
                }
                printf("\nScore after this round -");
                printf("    1 : %.1f     2 : %.1f\n\n",score1,score2);
            }
            if(score1==score2){
                int r1=rand();                                      // deciding the winner
                int r2=rand();
                while(r1==r2){
                    int r1=rand();
                    int r2=rand();
                }
                if(r1>r2)printf("Child 1 won the match in tiebreaker.\n\n");
                else if(r1<r2)printf("Child 2 won the match in tiebreaker.\n\n");
            }
            else if(score1>score2)printf("Child 1 won the match.\n\n");
            else printf("Child 2 won the match.\n\n");
            kill(pid1,SIGKILL);                       // killing the child processes.
            kill(pid2,SIGKILL);
        }
    }
  return 0;
}