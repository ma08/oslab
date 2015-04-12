#define _XOPEN_SOURCE 600
#include <iostream>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <queue>
#include <curl/curl.h>
#include <htmlcxx/html/ParserDom.h>
#define NUMTHREADS 5
#define LEVEL 3
using namespace std;
using namespace htmlcxx;

struct Struct {
  char *ptr;
  size_t len;
};

queue<string> to_do,to_do_next;
vector<pair<pair<string,int>,int > > done;
pthread_mutex_t to_do_next_mutex,to_do_mutex;
map<string,int> done_map;
pthread_barrier_t barrier1,barrier2;
pthread_barrierattr_t attr;
int url_level=1;

void level_increase(int id){
  int ret;
  ret=pthread_barrier_wait(&barrier1);
  if(id==1){
    swap(to_do,to_do_next);
    url_level++;
    cout<<endl<<"The new level number is "<<url_level<<endl;
  }
  ret=pthread_barrier_wait(&barrier2);
}

void init_string(struct Struct *s){
  s->len=0;
  s->ptr=(char*) malloc(s->len+1);
  if(s->ptr==NULL){
    fprintf(stderr,"malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0]='\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct Struct *s){
  size_t new_len=s->len+size*nmemb;
  s->ptr=(char*)realloc(s->ptr, new_len+1);
  if(s->ptr==NULL){
    fprintf(stderr,"realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len,ptr,size*nmemb);
  s->ptr[new_len]='\0';
  s->len=new_len;
  return size*nmemb;
}

void* parse_urls(void* threadid)
{
  int id=(int)threadid;
  while(1){
    int flag=0;
    string url;
    pthread_mutex_lock(&to_do_mutex);
    if(to_do.empty()){
      cout<<endl<<"Thread "<<id<<" found to-do queue empty\n";
      pthread_mutex_unlock(&to_do_mutex);
      level_increase(id);
      if(url_level==LEVEL)break;
      else continue;
    }
    url=to_do.front();
    cout<<endl<<"Thread "<<id<<" dequeueing a url from to-do queue : "<<url<<endl;
    to_do.pop();
    done_map[url]++;
    if(done_map[url]!=1){done_map[url]--;
      flag=1;}
    if(flag==0)done.push_back(make_pair(make_pair(url,id),url_level));
    pthread_mutex_unlock(&to_do_mutex);
    if(flag==1)continue;
    CURL *curl;
    CURLcode res;
    curl=curl_easy_init();
    if(curl){
      struct Struct s;
      init_string(&s);
      curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
      curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, writefunc);
      curl_easy_setopt(curl,CURLOPT_WRITEDATA, &s);
      res=curl_easy_perform(curl);
      string html(s.ptr);
      free(s.ptr);
      HTML::ParserDom parser;
      tree<HTML::Node> dom=parser.parseTree(html);
      tree<HTML::Node>::iterator it=dom.begin();
      tree<HTML::Node>::iterator end=dom.end();
      for(;it!=end;++it){
        if(it->tagName()=="a"){
          it->parseAttributes();
          string str=it->attribute("href").second,str1=url;
          if(str==""||str=="#"||str=="/"||str=="/#")continue;
          else if(str[0]=='/')str1.append(str.substr(1));
          else if(str[0]=='h'&&str[1]=='t'&&str[2]=='t'&&str[3]=='p')str1=str;
          else if(str.substr(0,6)=="mailto")continue;
          else str1.append(str);
          pthread_mutex_lock(&to_do_next_mutex);
          to_do_next.push(str1);
          pthread_mutex_unlock(&to_do_next_mutex);
        }
      }
      curl_easy_cleanup(curl);
    }
  }
  return NULL;
}

int main(int argc, char const *argv[])
{
  pthread_t threads[NUMTHREADS];
  pthread_mutex_init(&to_do_next_mutex,NULL);
  pthread_mutex_init(&to_do_mutex,NULL);
  int ret1,ret2,x; 
  ret1=pthread_barrier_init(&barrier1,&attr,NUMTHREADS);
  ret2=pthread_barrier_init(&barrier2,&attr,NUMTHREADS);
  to_do.push("cse.iitkgp.ac.in/");
  void* status;
  for(int i=0;i<NUMTHREADS;i++){
    x=pthread_create(&threads[i],NULL,parse_urls,(void*)(i+1));
  }
  for(int i=0;i<NUMTHREADS;i++){
    x=pthread_join(threads[i],&status);
      if(x){
        printf("ERROR; return code from pthread_join() is %d\n",x);
        exit(-1);
      }
  }
  for(int i=0;i<done.size();++i){
    cout<<done[i].first.first<<"\t"<<done[i].first.second<<"\t"<<done[i].second<<endl;
  }
  for(int i=0;i<to_do.size();++i){
    string s=to_do.front();
    to_do.pop();
    cout<<s<<"\t0\t"<<"\t"<<LEVEL<<endl;
  }
  pthread_barrierattr_destroy(&attr);
  pthread_mutex_destroy(&to_do_mutex);
  pthread_mutex_destroy(&to_do_next_mutex);
  pthread_barrier_destroy(&barrier1);
  pthread_barrier_destroy(&barrier2);
  return 0;
}