#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<queue>
#include<vector>
#include<map>
#include<unordered_set>

using namespace std;
class naivePriority{
  public:
    map<int,int> pages;
    //first is id, 2nd is key
    /*void updateKey(int id, int key){
      map<int,int>::iterator it;
      it=pages.find(id);
      if(it!=pages.end()){
        it->second = key;
      }
    }*/
    int pop(){
      map<int,int>::iterator it;
      if(pages.size()==0){
        return -1;
      }
      map<int,int>::iterator max_it=pages.begin();
      for(it=pages.begin();it!=pages.end();it++){
        if(it->second>=max_it->second)
          max_it=it;
      }
      int retval = max_it->first;
      pages.erase(max_it);
      return retval;
    }
    void printPages(){
      map<int,int>::iterator it=pages.begin();
      printf("\n\n Printing Priority table");
      for(it=pages.begin();it!=pages.end();it++){
        printf("\n %d %d",it->first,it->second);
      }
    }
};


int LFU(int n_frames, int ref[], int size){
  int result=0;
  naivePriority pq;
  map<int,int>::iterator it;
  int i;
  for (i = 0; i < size; ++i){
    it=pq.pages.find(ref[i]);
    if(it==pq.pages.end()){
      //page fault
      result++;
      if(pq.pages.size()==n_frames){
        //page fault replace
        pq.pop();
        /*pq.printPages();*/
        /*printf("\n%d %d page fault",i,ref[i]);*/
        /*printf(" %d",pq.pop());*/
        pq.pages[ref[i]]=size;
      }else{
        /*printf("\n%d %d page fault",i,ref[i]);*/
        pq.pages[ref[i]]=size;
      }
    }else{
      //updating priority
      pq.pages[ref[i]]--;
    }
  }
  return result;
}


int LRU(int n_frames, int ref[], int size){
  int result=0;
  naivePriority pq;
  map<int,int>::iterator it;
  int i;
  for (i = 0; i < size; ++i){
    it=pq.pages.find(ref[i]);
    if(it==pq.pages.end()){
      //page fault
      result++;
      if(pq.pages.size()==n_frames){
        //page fault replace
        pq.pop();
        /*pq.printPages();*/
        /*printf("\n%d %d page fault",i,ref[i]);*/
        /*printf(" %d",pq.pop());*/
        pq.pages[ref[i]]=size - i;
      }else{
        /*printf("\n%d %d page fault",i,ref[i]);*/
        pq.pages[ref[i]]=size - i;
      }
    }else{
      //updating time
      pq.pages[ref[i]]=size - i;
    }
  }
  return result;
}

int FIFO(int n_frames, int ref[], int size){
  int result=0;
  queue<int> pageq;
  unordered_set<int> page_set;
  unordered_set<int>::iterator it;
  int popped;
  int i;
  for (i = 0; i < size; ++i){
    it=page_set.find(ref[i]);
    if(it==page_set.end()){
      result++;
      /*printf("\n%d %d page fault",i,ref[i]);*/
      if(pageq.size()==n_frames){
        //page fault replace
        popped=pageq.front();
        pageq.pop();
        page_set.erase(popped);
        pageq.push(ref[i]);
        page_set.insert(ref[i]);
      }else{
        //just push
        pageq.push(ref[i]);
        page_set.insert(ref[i]);
      }
    }
  }
  return result;
}

int secondChance(int n_frames, int ref[], int size){
  int result=0;
  queue<int> pageq;
  map<int,int> page_ref;
  map<int,int>::iterator it;
  int popped;
  int i;
  for (i = 0; i < size; ++i){
    it=page_ref.find(ref[i]);
    if(it==page_ref.end()){
      result++;
      /*printf("\n%d %d page fault",i,ref[i]);*/
      if(pageq.size()==n_frames){
        while(1){
          //page fault replace
          popped=pageq.front();
          pageq.pop();
          if(page_ref[popped]==0){
            page_ref.erase(popped);
            break;
          }
          pageq.push(popped);
          page_ref[popped]=0;
        }
        pageq.push(ref[i]);
        page_ref[ref[i]]=0;
      }else{
        //just push
        pageq.push(ref[i]);
        page_ref[ref[i]]=0;
      }
    }else{
      page_ref[ref[i]]=1;
    }
  }
  return result;
}

//takes number of frames as the command line argument
int main(int argc, char *argv[])
{
  int n_frames=20;
  if(argc>1){
    n_frames = atoi(argv[1]);
  }
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
  int choice;
  while (fgets(line, sizeof(line), fp)!=NULL)
  {
    token = strtok(line, " ");
    while (token != NULL){
      /*printf("\n%s",token);*/
      ref[size++]=atoi(token);
      token = strtok(NULL, " ");
    }
  }
  int res;
  printf("\nSelec a page replacement algo: \n\t1.FIFO\n\t2.LRU\n\t3.LFU\n\t4.Second Chance\n");
  scanf("%d",&choice);
  switch (choice) {
    case 1:
      res=FIFO(n_frames,ref,size);
      printf("\n Page faults using FIFO: %d\n",res);
      break;
    case 2:
      res=LRU(n_frames,ref,size);
      printf("\n Page faults using LRU: %d\n",res);
      break;
    case 3:
      res=LFU(n_frames,ref,size);
      printf("\n Page faults using LFU: %d\n",res);
      break;
    case 4:
      res=secondChance(n_frames,ref,size);
      printf("\n Page faults using Second Chance: %d\n",res);
      break;
    default:
      res=-1;
      break;
  }
  printf("%d\n",res);
  return 0;
}


