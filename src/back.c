#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

char *args[128];

int main(int argc, char *argv[]) {
// argv: argv[1] is job id, argv[2] is filename, argv[3..] are file args
  if(argc < 3) {
    fprintf(stderr, "back: invalid arguments\n");
    return 1;
  }

  int arg_pos = 0;
  for(int i = 2; i < argc && arg_pos < 127; i++, arg_pos++) {
    size_t len = strlen(argv[i]);
    args[arg_pos] = malloc(len + 1);
    memcpy(args[arg_pos], argv[i], len);
    args[arg_pos][len] = '\0';
  }
  args[arg_pos] = (char*)0;

  int pid = fork();

  if(pid == 0) {
    int res = execvp(argv[2], args);
    if(res == -1) {
      perror(argv[2]);
      exit(-1);
    }
  }
  printf("\n[%s] %d\n", argv[1], pid);
  int ret = 0;
  waitpid(pid, &ret, 0);
  for(int i = 0; i < arg_pos; i++)
    free(args[i]);
  ret = WEXITSTATUS(ret);
  if(ret == 0) {
    printf("[%s] done  ", argv[1]);
    for(int i = 2; i < argc; i++)
      printf("%s ", argv[i]);
    puts("");
  } else {
    if(ret > 128) ret = ret - 256;
    printf("[%s] exit %d  ", argv[1], ret);
    for(int i = 2; i < argc; i++)
      printf("%s ", argv[i]);
    puts("");
  }

  return 0;
}
