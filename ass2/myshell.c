#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
/*http://stackoverflow.com/questions/9638714/xcode-c-missing-sperm*/
char const * sperm(__mode_t mode) {
    static char local_buff[16] = {0};
    int i = 0;
    // user permissions
    if (S_ISDIR(mode)) local_buff[i] = 'd';
    else local_buff[i]='-';
    i++;
    if ((mode & S_IRUSR) == S_IRUSR) local_buff[i] = 'r';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IWUSR) == S_IWUSR) local_buff[i] = 'w';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IXUSR) == S_IXUSR) local_buff[i] = 'x';
    else local_buff[i] = '-';
    i++;
    // group permissions
    if ((mode & S_IRGRP) == S_IRGRP) local_buff[i] = 'r';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IWGRP) == S_IWGRP) local_buff[i] = 'w';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IXGRP) == S_IXGRP) local_buff[i] = 'x';
    else local_buff[i] = '-';
    i++;
    // other permissions
    if ((mode & S_IROTH) == S_IROTH) local_buff[i] = 'r';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IWOTH) == S_IWOTH) local_buff[i] = 'w';
    else local_buff[i] = '-';
    i++;
    if ((mode & S_IXOTH) == S_IXOTH) local_buff[i] = 'x';
    else local_buff[i] = '-';
    return local_buff;
}

int parsecommand(char **parsed,char *str, int len){
  /*printf("%d\n",len);*/
  int count=0;
  int i=0,k=0;
  while(i<len){
    k=0;
    /*printf("%d--",i);*/
    while(i<len&&str[i]==' ')
      i++;
    /*printf("%d",i);*/
    if(i==len)
      break;
    while(i<len&&(str[i]!=' ')){
      /*printf("%d %d\n",k,i);*/
      parsed[count][k++]=str[i];
      if(str[i]=='\\'&&k<len-1){
        if(str[i+1]==' '){
          parsed[count][k-1]=str[++i];

        }
      }
      i++;
    }
    parsed[count][k]='\0';
    count++;
  }
  return count;
}


int main(int argc, char *argv[])
{
  char **cmd_words;
  int i;
  cmd_words=(char**)(malloc(sizeof(char*)*20));
  for(i=0;i<20;++i){
    cmd_words[i]=(char*)(malloc(sizeof(char)*100));
  }
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
    length=strlen(input)-1;
    if(input[length-1]=='\n')
      input[length-1]='\0';
    int size=parsecommand(cmd_words,input,length);
    /*for (i = 0; i < size; ++i)
    {
      printf("%s\n",cmd_words[i]);
    }*/
    if(strcmp(cmd_words[0],"exit")==0)break;
    else if(strcmp(cmd_words[0],"pwd")==0){
      printf("%s\n",cwd);
    }
    else if(strcmp(cmd_words[0],"cd")==0){
      if(size==1){
        if(chdir(getenv("HOME"))==0){}
        else {perror("cd");}
      }
      else {
        if(chdir(cmd_words[1])==0){}
        else {
          perror("cd");
        }
      }
    }
    else if(strcmp(cmd_words[0],"mkdir")==0){
      if(mkdir(cmd_words[1], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)==0){}
        else {perror("mkdir");}
    }
    else if(strcmp(cmd_words[0],"rmdir")==0){
      if(rmdir(cmd_words[1])==0){}
        else {perror("rmdir");}
    }
    else if(strcmp(cmd_words[0],"ls")==0){
      if(size==2&&strcmp(cmd_words[1],"-l")==0){
        DIR* dirp;
        struct dirent* dp;
        struct stat     statbuf;
        struct passwd  *pwd;
        struct group   *grp;
        struct tm      *tm;
        char            datestring[256];
        if((dirp=opendir("."))!=NULL){
          errno=0;
          while((dp=readdir(dirp))!=NULL){
/*http://pubs.opengroup.org/onlinepubs/009695399/functions/stat.html*/
            /* Get entry's information. */
            if(dp->d_name[0]=='.')
              continue;
            if (stat(dp->d_name, &statbuf) == -1)
              continue;
            /* Print out type, permissions, and number of links. */
            printf("%10.10s", sperm (statbuf.st_mode));
            /*printf("%4d", statbuf.st_nlink);*/
            /* Print out owner's name if it is found using getpwuid(). */
            if ((pwd = getpwuid(statbuf.st_uid)) != NULL)
              printf(" %-8.8s", pwd->pw_name);
            else
              printf(" %-8d", statbuf.st_uid);
            /* Print out group name if it is found using getgrgid(). */
            if ((grp = getgrgid(statbuf.st_gid)) != NULL)
              printf(" %-8.8s", grp->gr_name);
            else
              printf(" %-8d", statbuf.st_gid);
            /* Print size of file. */
            printf(" %9jd", (intmax_t)statbuf.st_size);
            tm = localtime(&statbuf.st_mtime);
            /* Get localized date string. */
            strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);
            printf(" %s %s\n", datestring, dp->d_name);
          }
          if(errno==0){
            closedir(dirp);
          }
        }else{
          perror("");
        }

        
      }else{
        DIR* dirp;
        struct dirent* dp;
        if((dirp=opendir("."))!=NULL){
          errno=0;
          while((dp=readdir(dirp))!=NULL){
            if(dp->d_name[0]=='.')
              continue;
            printf("%s ",dp->d_name);
          }
          if(errno==0){
            closedir(dirp);
          }
        }else{
          perror("");
        }
        printf("\n");
      }
    }
    //if(input[0]=='c'&&input[1]=='d')
    /*printf("%s",input);*/
    /*scanf("%s",input);*/
    if(cwd!=getcwd(cwd,50)){
      perror("getcwd");
      /*printf("%d error",errno);*/
    }
  }
  return 0;
}

