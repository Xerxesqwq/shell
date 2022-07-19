#include "proj.h"

#ifndef __TRIE__
#define __TRIE__ 1

#include <bits/types/clock_t.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>

char triech[16384][128];
char _ch[1 << 20];
int trieval[1 << 19];
int trieAnswerCnt;
int _head[1 << 16], nxt[1 << 16], _cnt;
int builtInID[1 << 16];
char trieAns[1024][256];

void saveDicToDisk() {
    int shmid = shmget((key_t)998244353, 1 << 23, IPC_CREAT | 0666);
    char* buf = (char*)shmat(shmid, (void*)0, 0);
    size_t offset = 0;

    memcpy(buf + offset, triech, sizeof(triech));
    offset += sizeof(triech);
    memcpy(buf + offset, _ch, sizeof(_ch));
    offset += sizeof(_ch);
    memcpy(buf + offset, trieval, sizeof(trieval));
    offset += sizeof(trieval);
    memcpy(buf + offset, &trieAnswerCnt, 4);
    offset += 4;
    memcpy(buf + offset, _head, sizeof(_head));
    offset += sizeof(_head);
    memcpy(buf + offset, nxt, sizeof(nxt));
    offset += sizeof(nxt);
    memcpy(buf + offset, &_cnt, 4);
    offset += 4;
    memcpy(buf + offset, builtInID, sizeof(builtInID));
    offset += sizeof(builtInID);
    memcpy(buf + offset, trieAns, sizeof(trieAns));
    offset += sizeof(trieAns);
    // memcpy(buf + offset, hiscmd, sizeof(hiscmd));
    // offset += sizeof(hiscmd);
    // unsigned long long res = ((unsigned long long)hisptr << 32 | tmphisptr);
    // memcpy(buf + offset, &res, 8);
    // offset += 8;
    // printf("size = %d\n", offset);
    shmdt(buf);
  //  printf("0 = %d\n", triech[0][0]);
    int __pid = fork();
    if(__pid == 0) {
        char *args[2] = {"c", (char*)0};
        execvp("./compress", args);
    }
    return;
}

void readDicFromDisk() {
    int shmid = shmget((key_t)998244353, 1 << 23, IPC_CREAT | 0666);
    char* buf = (char*)shmat(shmid, (void*)0, 0);
    int fd = open("../config/terminal.dat", O_RDONLY);
    if(fd == -1) {
        shmdt(buf);
        shmctl(shmid, IPC_RMID, 0);
        return;
    }
    int zsiz = read(fd, buf, 1 << 23);
    
    int __pid = fork();
    if(__pid == 0) {
        char s[16]; memset(s, 0, sizeof(s));
        sprintf(s, "%d", zsiz);
        char *args[3] = {"d", s, (char*)0};
        execvp("./compress", args);
    }
    int __decom;
    waitpid(__pid, &__decom, 0);
   // printf("0 = %d\n", buf[0]);
    size_t offset = 0;
    memcpy(triech, buf + offset, sizeof(triech));
    offset += sizeof(triech);
    memcpy(_ch, buf + offset, sizeof(_ch));
    offset += sizeof(_ch);
    memcpy(trieval, buf + offset, sizeof(trieval));
    offset += sizeof(trieval);
    memcpy(&trieAnswerCnt, buf + offset, 4);
    offset += 4;
    memcpy(_head, buf + offset, sizeof(_head));
    offset += sizeof(_head);
    memcpy(nxt, buf + offset, sizeof(nxt));
    offset += sizeof(nxt);
    memcpy(&_cnt, buf + offset, 4);
    offset += 4;
    memcpy(builtInID, buf + offset, sizeof(builtInID));
    offset += sizeof(builtInID);
    memcpy(trieAns, buf + offset, sizeof(trieAns));
    shmdt(buf);
    shmctl(shmid, IPC_RMID, 0);
}

void trieInsert(const char *s) {
    //printf("\n\ntrie:%s\n\n", s);
    int len = strlen(s), u = 0;
    for(int i = 0;i < len; i++) {
        int c = s[i];
        if(!triech[u][c]) {
            triech[u][c] = ++_cnt;
            _ch[_cnt] = s[i];
            nxt[_cnt] = _head[u]; _head[u] = _cnt;
        }
        u = triech[u][c];
    }
    //printf("\n insert: s = %s, len = %d ,u = %d\n, trieval[u] = %d", s, strlen(s),  u, trieval[u] + 1);
    trieval[u]++; 
    //printf("trieval = %d\n", trieval[u]);
}

void trieInsertBuiltIn(const char *s, int id) {
    int len = strlen(s), u = 0;
    for(int i = 0;i < len; i++) {
        int c = s[i];
        if(!triech[u][c]) {
            triech[u][c] = ++_cnt;
            _ch[_cnt] = s[i];
            nxt[_cnt] = _head[u]; _head[u] = _cnt;
        }
        u = triech[u][c];
    }
    trieval[u]++; builtInID[u] = id;
}

void dfs(int u, char* s, int pos) {
    //printf("%d %c %d\n", u, _ch[u], pos);
    s[pos] = _ch[u];
    if(trieval[u]) {
        trieAnswerCnt++;
        for(int i = 0; i <= pos; i++) {
            trieAns[trieAnswerCnt][i] = s[i];
        }
        trieAns[trieAnswerCnt][pos + 1] = '\0';
    }
    for(int i = _head[u]; i; i = nxt[i])
        dfs(i, s, pos + 1);
}

void trieFind(struct Prompt* prompt) {
        //puts("qwq");
    char ans[1 << 10];
    int u = 0, len = strlen(prompt -> cmd);
    trieAnswerCnt = 0;
    for(int i = 0;i < len; i++){
        int c = prompt -> cmd[i];
        if(!triech[u][c]) {
            prompt -> cnt = 0; trieAnswerCnt = 0;
            return;
        }
        u = triech[u][c];
    }
    dfs(u, ans, 0);
    prompt -> cnt = trieAnswerCnt;
    memcpy(prompt -> res, trieAns, 1 << 18);
}


int trieGetBuiltIn(const char *s) {
    int u = 0, len = strlen(s);
    for(int i = 0;i < len; i++){
        int c = s[i];
        if(!triech[u][c]) return -1;
        u = triech[u][c];
    }
    if(trieval[u] && builtInID[u] > 0) return builtInID[u];
    return -1;
}

int trieExist(const char* s) {
    //
    int u = 0, len = strlen(s);
    for(int i = 0; i < len; i++) {
        int c = s[i];
        if(!triech[u][c]) return 0;
        u = triech[u][c];
    }
   //printf("\n qurry: s = %s, len = %d, u = %d, ans = %d", s, strlen(s), u, trieval[u]);
    return trieval[u];
}

#endif