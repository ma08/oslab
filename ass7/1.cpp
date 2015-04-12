#include <iostream>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <htmlcxx/html/ParserDom.h>
using namespace std;

using namespace htmlcxx;

struct Struct {
  char *ptr;
  size_t len;
};

void init_string(struct Struct *s) {
  s->len = 0;
  s->ptr =(char*) malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct Struct *s)
{
  size_t new_len = s->len + size*nmemb;
  s->ptr =(char*) realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}

int main(void)
{
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if(curl) {
    struct Struct s;
    init_string(&s);
    string url="iitkgp.ac.in";
    curl_easy_setopt(curl, CURLOPT_URL,url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    res = curl_easy_perform(curl);
    string html(s.ptr);
    free(s.ptr);
   HTML::ParserDom parser;
  tree<HTML::Node> dom = parser.parseTree(html);
  //Dump all links in the tree
  tree<HTML::Node>::iterator it = dom.begin();
  tree<HTML::Node>::iterator end = dom.end();
  for (; it != end; ++it)
  {
    if (it->tagName()=="a")
    {
      it->parseAttributes();
      std::cout<<it->attribute("href").second<<endl;
    }
  }
    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  return 0;
}