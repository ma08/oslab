#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
#include <libgen.h>

//http://stackoverflow.com/questions/2180079/how-can-i-copy-a-file-on-unix-using-c
int cp(const char *to, const char *from)
{
  int fd_to, fd_from;
  char buf[4096];
  ssize_t nread;
  fd_from = open(from, O_RDONLY);
  if (fd_from < 0){
    return -1;
  }
  else{
    fd_to = open(to, O_WRONLY | O_CREAT , 0666);
    if (fd_to < 0){
      close(fd_from);
      return -1;
    }
    else{
      while (nread = read(fd_from, buf, sizeof buf), nread > 0)
      {
        char *out_ptr = buf;
        ssize_t nwritten;
        do {
          nwritten = write(fd_to, out_ptr, nread);
          if (nwritten >= 0)
          {
            nread -= nwritten;
            out_ptr += nwritten;
          }
          else if (errno != EINTR)
          {
            close(fd_from);
            if (fd_to >= 0)
              close(fd_to);
            return -1;
          }
        } while (nread > 0);
      }
      if (nread == 0)
      {
        if (close(fd_to) < 0)
        {
          close(fd_from);
          return -1;
        }
        close(fd_from);
        return 0;
      }
    }
  }
  return -1;
}


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

int parsecommand(char **parsed,char *str, int len, char *input_red, char *output_red){
  int count=0;
  /*char *input_red_orig=input_red;*/
  /*char *output_red_orig=output_red;*/
  char* red_arr=output_red;
  int redirection=0;
  int red_count=0;
  int i=0,k=0;
  while(i<len){
    k=0;
    while(i<len&&str[i]==' ')
      i++;
    if(i==len)
      break;
    while(i<len&&(str[i]!=' ')){
      if(redirection){
        if(str[i]=='>'||str[i]=='<'){
          //error
        }
        else{
          red_arr[red_count++]=str[i];
          if(str[i]=='\\'&&k<len-1){
            if(str[i+1]==' '){
              red_arr[red_count++]=str[++i];
            }
          }
          i++;
          continue;
        }
      }
      else{
        redirection= str[i]=='>'?1:str[i]=='<'?2:0;
        if(redirection){
          red_count=0;
          if(redirection==1){
            red_arr=output_red;
          }
          else{
            red_arr=input_red;
          } 
          i++;
          while(i<len&&str[i]==' ')
            i++;
          continue;
        }
      }
      parsed[count][k++]=str[i];
      if(str[i]=='\\'&&k<len-1){
        if(str[i+1]==' '){
          parsed[count][k-1]=str[++i];
        }
      }
      i++;
    }
    if(redirection){
      redirection=0;
      red_arr[red_count++]='\0';
      continue;
    }

    parsed[count][k]='\0';
    count++;
  }
  return count;
}


int main(int argc, char *argv[])
{
  int status;
  int write_end;
  int read_end;
  int saved_stdout;
  int in_pipe_id[2];
  int out_pipe_id[2];
  int num_pipes=0;
  int saved_stdin;
  int input_fd;
  int output_fd;
  char **cmd_words;
  int i;
  int pipe_iter=0;
  cmd_words=(char**)(malloc(sizeof(char*)*20));
  for(i=0;i<20;++i){
    cmd_words[i]=(char*)(malloc(sizeof(char)*100));
  }
  char cwd[100];
  char input[100];
  char tok_input[100];
  char tok_input_1[100];
  char *token;
  char *token_1;
  char input_red[100];
  char output_red[100];
  input_red[0]='\0';
  output_red[0]='\0';
  if(cwd!=getcwd(cwd,100)){
    perror("getcwd");
  }
  int length;
  while(1){
    num_pipes=0;
    pipe_iter=0;
    printf("%s>",cwd);
    fgets(input,100,stdin);
    /*printf("uuuuuuuuu");*/
    length=strlen(input);
    if(input[length-1]=='\n'){
      input[length-1]='\0';
      length--;
    }
    strcpy(tok_input,input);
    strcpy(tok_input_1,input);
    if(token==NULL)
      token=input;
    token_1=strtok(tok_input_1,"|");
    while((token_1=strtok(NULL,"|"))!=NULL){
      num_pipes++;
      /*printf("ruuuuuuuu");*/
    } 
    /*printf("%d",num_pipes);*/
    saved_stdin=dup(STDIN_FILENO);
    /*perror("");*/
    saved_stdout=dup(STDOUT_FILENO);
    /*perror("");*/
    token = strtok(tok_input, "|");
    while(token!=NULL){
      /*printf("\n\n---\n\n");
      token = strtok(NULL, "|");
      continue;*/
      if(num_pipes > 0){
        if(pipe_iter==0){
          pipe(out_pipe_id);
          write_end=out_pipe_id[1];
          dup2(write_end,STDOUT_FILENO);
        }else if(pipe_iter==num_pipes){
          read_end=out_pipe_id[0];
          dup2(saved_stdout,STDOUT_FILENO);
          dup2(read_end,STDIN_FILENO);
        }else{
          read_end=out_pipe_id[0];
          pipe(out_pipe_id);
          write_end=out_pipe_id[1];
          dup2(read_end,STDIN_FILENO);
          dup2(write_end,STDOUT_FILENO);
        }
      }
      input_red[0]='\0';
      output_red[0]='\0';
      /*printf("\n--%s--",token);*/
      int size=parsecommand(cmd_words,token,strlen(token),input_red,output_red);
      if (input_red[0]!='\0'){
        input_fd=open(input_red,O_RDONLY);
        if(errno){
          perror(input_red);
          break;
        }
        dup2(input_fd,STDIN_FILENO);
        close(input_fd);
      }
      if(output_red[0]!='\0'){
        output_fd = open(output_red,O_WRONLY|O_CREAT| O_TRUNC,0666);
        if(errno){
          perror(output_red);
          break;
        }
        dup2(output_fd,STDOUT_FILENO);
        close(output_fd);
      }
      /*printf("\n-------------------------\n");
      for (i = 0; i < size; ++i)
      {
        printf("%s  ",cmd_words[i]);
        
      }
      printf("\ninput_red: %s",input_red);
      printf("\noutput_red: %s\n",output_red);*/
      /*token = strtok(NULL, "|");
      continue;*/
      if(size==0)
        continue;
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
          }
          else{
            perror("");
          }
          printf("\n");
        }
      }
      else if(strcmp(cmd_words[0],"cp")==0){
        
        if(access (cmd_words[1], R_OK)==0){
          if(access (cmd_words[2], W_OK)==0){
            struct stat     statbuf1;
            struct stat     statbuf2;
            if (stat(cmd_words[1], &statbuf1) == -1){}
            if (stat(cmd_words[2], &statbuf2) == -1){}
            time_t t1=statbuf1.st_mtime;
            time_t t2=statbuf2.st_mtime;
            if(t2>t1){
              printf("%s: is more recent than %s\n",cmd_words[2],cmd_words[1]);
            }
            else{
              if(cp(cmd_words[2],cmd_words[1])!=0){
                perror("");
              }
            }
          }else{ if(errno==ENOENT){
              if(cp(cmd_words[2],cmd_words[1])!=0){
                perror("");
              }
            }else if(errno==EACCES){
              perror(cmd_words[2]);
            }
          }
        }else{
          perror(cmd_words[1]);
        }
      }
      else{
        int ret;
        pid_t pid=-1;
        int background=0;
        int bool1=cmd_words[size-1][strlen(cmd_words[size-1])-1]=='&';
        int bool2=strcmp(cmd_words[size-1],"&")==0;
        if(bool1)
          cmd_words[size-1][strlen(cmd_words[size-1])-1]='\0';
        if(bool2)
          size--;
        background=bool1||bool2;
        char *path=cmd_words[0];
        char filepath[40];
        char **cmd;
        cmd = (char **)(malloc(sizeof(char **)*size));
        cmd[0]=basename(cmd_words[0]);
        for (i = 1; i < size; ++i)
        {
          cmd[i]=cmd_words[i];
        }
        cmd[i]=(char*)0;
        if(access (cmd_words[0], X_OK)==0){
          if(cmd_words[0][0]!='.'&&cmd_words[0][0]!='/'){
            strcpy(filepath,".");
            //strcpy(filepath,cwd);
            strcat(filepath,"/");
            strcat(filepath,cmd_words[0]);
            path=filepath;
          }
          pid=fork();
        }else{
          if(errno==ENOENT&&cmd_words[0][0]!='.'&&cmd_words[0][0]!='/'){
            char *pathenv = getenv("PATH");
            int i=0;
            int k=0;
            while(i<strlen(pathenv)){
              while(i<strlen(pathenv)&&pathenv[i]!=':'){
                filepath[k++]=pathenv[i++];
              }
              if(k>0){
                filepath[k]='\0';
              }
              strcat(filepath,"/");
              strcat(filepath,cmd_words[0]);
              if(access (filepath, X_OK)==0){
                path=filepath;
                pid=fork();
                break;
              }
              i++;
              k=0;
            }
            if(pid==-1)
              printf("%s : No such file or Directory\n",cmd[0]);
          }else{
            perror(cmd_words[0]);
          }
        }
        if(pid==0){
          if(background){
            freopen("/dev/null", "r", stdin);
            freopen("/dev/null", "w", stdout);
          }
          ret=execv(path,cmd);
          return 0;
        }
        else{
          if(pid!=-1&&!background){
            waitpid(pid,&status,0);
          }
        }
      }
      /*printf("\n%s",token);*/
      /*printf("\nbooo");*/
      errno=0;
      token = strtok(NULL, "|");
      /*if(num_pipes==0){
        if (input_red[0]!='\0'){
          dup2(saved_stdin,STDIN_FILENO);
        }
        if (output_red[0]!='\0'||(num_pipes>0&&pipe_iter!=num_pipes)){
          dup2(saved_stdout,STDOUT_FILENO);
        }
      }*/
      if(num_pipes>0&&pipe_iter!=num_pipes){
        close(write_end);
      }
      if(pipe_iter>0){
        close(read_end);
      }
      pipe_iter++;

      /*return 0;*/
      /*printf("\n%s",token);*/
      /*printf("\n---");*/
    }
    dup2(saved_stdin,STDIN_FILENO);
    dup2(saved_stdout,STDOUT_FILENO);
    close(saved_stdin);
    close(saved_stdout);

    if(cwd!=getcwd(cwd,100)){
      perror("getcwd");
    }
  }
   
  return 0;
}
