#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

char *args[128];

int main(int argc, char *argv[]) {
//argv: argv[0]: id, (filename = argv[1]), argv...
 // for(int i = 0; i < argc; i++)
   // printf("%s\n", argv[i]);

  for(int i = 1; i < argc; i++) {
    args[i - 1] = malloc(128);
    memcpy(args[i - 1], argv[i], strlen(argv[i])); 
  }
  
  int pid = fork();

  if(pid == 0) {
    int res = execvp(argv[1], args);
    if(res == -1) {
      perror(argv[1]);
      exit(-1);
    } 
  }  
  printf("\n[%s] %d\n", argv[0], pid);
  int ret = 0;
  waitpid(pid, &ret, 0);
  for(int i = 0; i < argc - 1; i++)
    free(args[i]);
  ret = WEXITSTATUS(ret);
  if(ret == 0) {
    printf("[%s] done  ", argv[0]);
    for(int i = 1; i < argc; i++)
      printf("%s ", argv[i]);
    puts("");
  } else {
    if(ret > 128) ret = ret - 256;
    printf("[%s] exit %d  ", argv[0], ret);
    for(int i = 1; i < argc; i++)
      printf("%s ", argv[i]);
    puts("");
  }

  return 0;
}