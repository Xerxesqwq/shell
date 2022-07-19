#ifndef __XERXES_TERMINAL__

#define __XERXES_TERMINAL__ 1
#define NORMAL  "\x1B[0m"
#define RED  "\x1B[31m"
#define GREEN  "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE  "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN  "\x1B[36m"
#define WHITE  "\x1B[37m"

struct Prompt {
    int cnt;
    char cmd[1024];
    char res[1024][256];
};


#endif