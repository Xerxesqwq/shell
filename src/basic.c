#include "proj.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

void getUserName(char *s) {
    cuserid(s);
}

void getHostName(char *s) {
    gethostname(s, 32);
}

static char* getdic(const char *s) {
    char userHome[256], tmp[128];
    getUserName(tmp); sprintf(userHome, "/home/%s", tmp); 
    static char ret[1024];
    if(strcmp(userHome, s) == 0) {
        ret[0] = '~'; ret[1] = '\0';
        return ret;
    }
    int pos = 0;
    int len = strlen(s);
    int cnt = 0;

    for(int i = 0; i < len; i++) {
        if(s[i] == '/') pos = 0;
        else ret[pos++] = s[i];
    }
    ret[pos] = 0;
    if(!pos) {
        ret[pos] = '/';
        ret[pos + 1] = 0;
    }
    return ret;
}

// char* getPrompt() {
//     static char prompt[1024];
//     fgets(prompt, sizeof(prompt), stdin);
//     int pos = 0; while(prompt[pos] != '\n') pos++;
//     for(int i = pos; i < 1024; i++) prompt[i] = '\0';
//     return prompt;
// }

void printInfo() {
    char username[32], hostname[32];
    char pwd[1024]; getcwd(pwd, 1024);
    getUserName(username);
    getHostName(hostname);
    char *dic = getdic(pwd);
    printf("%s[%s%s@%s %s%s%s]$ %s",WHITE, RED, username, hostname, GREEN, dic, WHITE, NORMAL);
    
}

void split(char *res[128], const char *src, char splitch) {
    int cnt = 0, cur = 0, andcnt = 0;
    int len = strlen(src);
    for(int i = 0; i < 128; i++) res[i][255] = '\0';
    for(int i = 0; i < len; i++) {
        if(src[i] == splitch) {
            if(i != 0 && src[i - 1] == splitch) continue;
            res[cnt][cur] = '\0';
            if(splitch == '&') res[cnt][255] = ++andcnt;
            cnt++; cur = 0;
            continue;
        }
        res[cnt][cur++] = src[i] == '&' ? '\0' : src[i];
    }
    res[cnt][cur] = '\0';
    if(strlen(res[cnt]) == 0) res[cnt] = (char*)0;
    else res[++cnt] = (char*)0;
}

#ifndef __XERXES_TERMINAL__

int main(int argc, char *argv[]) {
    char username[32], hostname[32];
    chdir("./testdir");

    char pwd[1024]; getcwd(pwd, 1024);
    getUserName(username);
    getHostName(hostname);
    while(1) {
        char *dic = getdic(pwd);
        printf("%s[%s%s@%s %s%s%s]$ %s",WHITE, RED, username, hostname, GREEN, dic, WHITE, NORMAL);
        char prompt[1024];
        fgets(prompt, sizeof(prompt), stdin);

        int pos = 0; while(prompt[pos] != '\n') pos++;
        for(int i = pos; i < 1024; i++) prompt[i] = '\0';
        if(strcmp(prompt, "exit") == 0) {
            puts("bye"); return 0;
        }
        if(strcmp(prompt, "pwd") == 0) puts(getcwd(pwd, 1024));
    }
    return 0;
}

#endif