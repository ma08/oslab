#define _XOPEN_SOURCE 600
#define NUMTHREADS 5
#define LEVEL 3
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
    cout<<endl<<"The new level number is "<<url_level<<".\n";
  }
  ret=pthread_barrier_wait(&barrier2);
}

void init_string(struct Struct *s){
  s->len=0;
  s->ptr=(char*)malloc(s->len+1);
  if(s->ptr==NULL)exit(EXIT_FAILURE);
  s->ptr[0]='\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct Struct *s){
  size_t new_len=s->len+size*nmemb;
  s->ptr=(char*)realloc(s->ptr,new_len+1);
  if(s->ptr==NULL)exit(EXIT_FAILURE);
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
      cout<<endl<<"Thread "<<id<<" found to-do queue empty.\n";
      pthread_mutex_unlock(&to_do_mutex);
      level_increase(id);
      if(url_level==LEVEL)break;
      else continue;
    }
    url=to_do.front();
    cout<<endl<<"Thread "<<id<<" dequeueing a url from to-do queue : "<<url<<".\n";
    to_do.pop();
    if(done_map[url]==1)flag=1;
    else done_map[url]=1;
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
      curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,writefunc);
      curl_easy_setopt(curl,CURLOPT_WRITEDATA,&s);
      res=curl_easy_perform(curl);
      string html(s.ptr);
      free(s.ptr);
      HTML::ParserDom parser;
      tree<HTML::Node> dom=parser.parseTree(html);
      tree<HTML::Node>::iterator it=dom.begin();
      tree<HTML::Node>::iterator end=dom.end();
      for(;it!=end;++it){
        if(it->tagName()=="a"||it->tagName()=="A"){
          it->parseAttributes();
          string str=it->attribute("href").second,str1=url;
          if(url[url.length()-1]!='/')str1.append("/");
          if(str!=""&&str[0]!='/'&&str[0]!='#'&&str.substr(0,6)!="mailto"&&str.substr(0,4)!="http"){
            str1.append(str);
            str=str1;
          }
          if(str.substr(0,4)!="http"||str.substr(0,6)=="mailto")continue;
          pthread_mutex_lock(&to_do_next_mutex);
          to_do_next.push(str);
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
  int x; 
  pthread_barrier_init(&barrier1,&attr,NUMTHREADS);
  pthread_barrier_init(&barrier2,&attr,NUMTHREADS);
  to_do.push("http://cse.iitkgp.ac.in");
  void* status;
  for(int i=0;i<NUMTHREADS;i++)x=pthread_create(&threads[i],NULL,parse_urls,(void*)(i+1));

  for(int i=0;i<NUMTHREADS;i++)x=pthread_join(threads[i],&status);

  cout<<"\n<URL>\t<Thread-id>\t<Level>\n\n";
  for(int i=0;i<done.size();++i){
    cout<<done[i].first.first<<"\t"<<done[i].first.second<<"\t"<<done[i].second<<endl;
  }
  while(to_do.size()>0){
    string s=to_do.front();
    to_do.pop();
    cout<<s<<"\t0"<<"\t"<<LEVEL<<endl;
  }
  pthread_barrierattr_destroy(&attr);
  pthread_mutex_destroy(&to_do_mutex);
  pthread_mutex_destroy(&to_do_next_mutex);
  pthread_barrier_destroy(&barrier1);
  pthread_barrier_destroy(&barrier2);
  return 0;
}