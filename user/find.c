find.c

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int match(char*, char*);

void
find(char *dir, char *para)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;


    if((fd = open(dir, 0)) < 0){
        fprintf(2, "find: nedokaze otvorit %s\n", dir);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: nedokaze stat %s\n", dir);
        close(fd);
        return;
    }

    switch(st.type){
    case T_FILE:
        fprintf(2, "find: %s je subor \n", dir);
        break;
    case T_DIR:
        if(strlen(dir) + 1 + DIRSIZ + 1 > sizeof(buf)){
            fprintf(2, "find: cesta je velmi dlha \n");
            break;
        }
        strcpy(buf, dir);

        p = buf + strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de))){
            if(de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if(stat(buf, &st) < 0){
                fprintf(2, "find: cannot stat %s\n", buf);
                continue;
            }
            switch(st.type){
            case T_FILE:
                if(match(para, de.name)){
                    printf("%s\n", buf);
                }
                break;
            case T_DIR:
                if(strcmp(de.name, ".") && strcmp(de.name, "..") ) find(buf, para);
                break;
            }
        }
        break;
    }
}

int
main(int argc, char *argv[])
{
    if(argc == 2){
        find(".", argv[1]);
    } else if(argc == 3){
        find(argv[1], argv[2]);
    } else {
        printf("error\n");
    }
    exit();
}

int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

int matchstar(int c, char *re, char *text)
{
  do{
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}