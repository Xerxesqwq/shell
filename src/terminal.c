#include "proj.h"
#include "basic.h"
#include "trie.h"
#include "read.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

void terminalExit() {
    saveDicToDisk();
    exit(0);
}

int _status[1 << 16], _statuspos;
char hiscmd[1024][256];
int hisptr = 0, tmphisptr = 0;

struct Prompt prompt;

char* getprompt() {
    static char buf[4096];
    memset(buf, 0, sizeof(buf));
    int pos = 0;
    buf[0] = 0;
    char ch;
    int flag = 0;
    int tipPos = 0;
    int lst = 0;
    int execflag = 0;
    int tabpos = 0;
    while(1) {
        ch = getch();
        if(ch == ' ') execflag = 1;
        if(ch != 9) flag = 0;

        if(ch == 10) {
            //printf("%d\n", pos);
            buf[pos] = 0;
            printf("%s", NORMAL);
            return buf;
        } else if(ch == 9) {//tab
            if(flag == 0) {
                memset(prompt.cmd, 0, sizeof(prompt.cmd));
                buf[pos] = 0;
                memcpy(prompt.cmd, buf, sizeof(buf));
                //printf("%d\n", prompt.cmd);
                trieFind(&prompt);
                tipPos = 0;
                flag = 1;
            }
            tipPos++;
            if(tipPos > prompt.cnt) continue;
            int len = strlen(prompt.res[tipPos]);
            if(tipPos > 1) {
                int _len = strlen(prompt.res[tipPos - 1]);
                for(int i = 1; i < _len; i++) putchar(' ');
                for(int i = 1; i < _len; i++) putchar('\b');
            }
            printf("%s", YELLOW);
            for(int i = 1; i < len; i++) putchar(prompt.res[tipPos][i]);
            for(int i = 1; i < len; i++) putchar('\b');
            tabpos = 1;
            printf("%s", NORMAL);

        } else if(ch == 127) {//backspace
            if(pos <= 0) continue;
            printf("\b%c\b", ' ');
            pos--;
        } else {
            buf[pos] = ch; pos++;
            
            if(pos >= 3) lst = buf[pos - 3] * 1000000 + buf[pos - 2] * 1000 + buf[pos - 1];
            putchar(ch);
            if(lst == 27091067) {
                pos -= 3;
                printf("\b%s", NORMAL);
                putchar(prompt.res[tipPos][tabpos]);
                buf[pos++] = prompt.res[tipPos][tabpos]; tabpos++;
                //printf("%d", pos);
            }
            if(lst == 27091068) {
                pos -= 3;
                if(pos > 0) putchar(buf[pos]);
                pos++;
                putchar(27); putchar(91); putchar(67);
            }
            char *ret = "";
            if(pos == 3 && lst == 27091065) {
                return "";
                puts("qwq");
                printf("%c%c%c", 27, 91, 66);
                if(tmphisptr < 0) continue;
                printf("%s", hiscmd[tmphisptr - 1]);
                tmphisptr--;
                return hiscmd[tmphisptr + 1];
            }
            if(pos == 3 && lst == 27091066) {
                return "";
                puts("qaq");
                if(tmphisptr > hisptr) continue;
                printf("%s", hiscmd[tmphisptr]);
                tmphisptr++;
                return hiscmd[tmphisptr - 1];
            } 
            
            /*
                down: 27 91 66
                left: 27 91 68
                right: 27 91 67
                up: 27 91 65
                backspace: 127
                tab: 9
                enter: 10
            */
        }

        if(!execflag) {
            buf[pos] = 0;
            if(trieExist(buf)) {
                for(int i = 0; i < pos; i++)
                    printf("\b");
                printf("%s", BLUE);
                for(int i = 0; i < pos; i++)
                    printf("%c", buf[i]);
                printf("%s", NORMAL);
            }
        }

    }
    return (char*)0;
}

void builtInInit() {
    trieInsertBuiltIn("exit", 1);
    trieInsertBuiltIn("clear", 2);
    trieInsertBuiltIn("cd", 3);
    trieInsertBuiltIn("history", 4);
    trieInsertBuiltIn("!", 5);
    trieInsertBuiltIn("!!", 6);
    trieInsertBuiltIn("fg", 7);
}

int frontpid, suspendpid;
pid_t pid[1 << 10], pidcnt;
char *ans;
//char __suspend[1 << 10];

void SIGINT_PROCESS(int t) {
    if(frontpid == 0) return;
    kill(frontpid, 9);
    printf("\b\bxerxes-terminal: killed %d\n", frontpid);
}

int ___st;
void SIGSTP_PROCESS() {
    if(frontpid == 0) return;
    kill(frontpid, SIGSTOP);
    printf("\b\bxerxes-terminal: suspended %d\n", frontpid);
    suspendpid = frontpid;
    frontpid = 0;
    //memcpy(__suspend, ans, strlen(ans) + 1);
    waitpid(frontpid, &___st, WNOHANG);
}

char** __cmd;
int builtin(const char *s) {
    static char userHome[256];
    int ret = trieGetBuiltIn(s);
    
    if(ret == -1) return 0;
    if(ret == 1) terminalExit();
    else if(ret == 2) system("clear");
    else if(ret == 3) {
        //puts(__cmd[1]);
        if(__cmd[1] == (char*)0) {
            printf("%serror%s: You need to give a directory.\n", RED, NORMAL);
        }
        else {
            if(strcmp(__cmd[1], "~") == 0)  {
                __cmd[1][0] = '\0';
                char tmp[256]; getUserName(tmp);
                sprintf(__cmd[1], "/home/%s", tmp);
            }
            if(chdir(__cmd[1]) == -1)
                perror("xerxes-terminal"); 
        }  
    } else if(ret == 4) {
        int n = hisptr > 100 ? 100 : hisptr;
        if(__cmd[1] != (char*)0) n = atoi(__cmd[1]); 
        if(n > hisptr) n = hisptr;
        if(n < 0) n = 0;
        printf("%d history commands found:\n", n);
        for(int i = n; i >= 1; i--) {
            printf("%d: %s\n", i, hiscmd[hisptr - i + 1]);
        }
    } else if(ret == 5) {
        if(__cmd[1] == (char*)0) {
            printf("%s!: error%s, you need to give a number.\n", RED, NORMAL);
            return 2;
        }
        int n = atoi(__cmd[1]);
        if(n > hisptr || n < 1) {
            printf("can't find this command.\n");
        } else {
            memcpy(ans, hiscmd[hisptr - n + 1], sizeof(hiscmd[hisptr - n + 1]));
            printf("you want to execute: %s\n", hiscmd[hisptr - n + 1]);
            return 2;
        }
    } else if(ret == 6) {
        int n = 1;
        if(n > hisptr || n < 1) {
            printf("not have recent command.\n");
        } else {
            memcpy(ans, hiscmd[hisptr - n + 1], sizeof(hiscmd[hisptr - n + 1]));
            printf("you want to execute: %s\n", hiscmd[hisptr - n + 1]);
            return 2;
        }
    } else if(ret == 7) {
        if(suspendpid == 0) {
            printf("xerxes-terminal: %sno current job.%s", RED, NORMAL);
            puts("");
            return 1;
        }
        kill(suspendpid, SIGCONT);
        frontpid = suspendpid;
        suspendpid = 0;
        printf("xerxes-terminal: continued %d\n", frontpid);
        waitpid(frontpid, &___st, WUNTRACED);
        frontpid = 0;
    }
    return 1;
}

int __id = 0;
void exec(const char* file, char * const argv[], int __status) {
    int ret = 0;
    if(__status == 0) { //not background
        ret = execvp(file, argv); 
        char errormsg[1024];
        sprintf(errormsg, "%s", prompt.cmd);
        perror(errormsg);
        exit(0);
    } else {
        char* args[256];
        for(int i = 0; i < 128; i++)
            args[i] = malloc(256);
        int id = __id % 9;
        if(!id) id = 9;
        args[0][0] = '0' + id;
        args[0][1] = '\0';
        
        for(int i = 0; ~i; i++) {
            if(argv[i] == (char*)0) {
                args[i + 1] = (char*)0;
                break;
            }
            int len = strlen(argv[i]);
            memcpy(args[i + 1], argv[i], len);
            args[i + 1][len] = '\0';
        }
        
        execvp("./back", args);
        
        char errormsg[1024];
        sprintf(errormsg, "%s", prompt.cmd);
        perror(errormsg);
        exit(0);
    }
}

void showHelp() {
    FILE *fp = fopen("help.txt", "r");
    int ch = 0;
    while(1) {
        ch = fgetc(fp);
        if(ch == EOF) break;
        putchar(ch);
    }
    exit(1);
}

void argsProcess(int argc, char *args[]) {
    for(int i = 0; i < argc; i++) {
        if(args[i][0] != '-') continue;
        if(args[i][1] == 'h') {
            showHelp();
        } else if(args[i][1] == 'c') {
            system("rm ../config/terminal.dat");
            puts("done!");
            exit(0);
        } else if(args[i][1] == 'n') {
            return;
        } else {
            printf("%s:invalid option\nYou can use -h for help.\n", args[i]);
            exit(1);
        }
        break;
    }
}

int main(int argc, char *args[]) {
    system("echo $'\033]30;Xerxes-Terminal\007'");
    //https://superuser.com/questions/705898/set-name-of-konsole-window
    size_t intellig;
    readDicFromDisk();
    builtInInit();
    argsProcess(argc, args);
    
    signal(SIGINT, SIGINT_PROCESS);
    signal(SIGQUIT, SIGINT_PROCESS);
    signal(SIGTSTP, SIGSTP_PROCESS);
    
    while(1) {

        printInfo();
        ans = getprompt();
        if(strlen(ans) == 0) {
            puts("");
            continue;
        }
        puts("");

        int __andcnt = 0;
        
        char *cmd[128];
        for(int j = 0; j < 128; j++) {
            cmd[j] = malloc(256);
        }
        __cmd = cmd;
        memset(prompt.cmd, 0, 1 << 10);
        memcpy(prompt.cmd, ans, strlen(ans));
        split(cmd, ans, ' ');
        
        int builtinst = builtin(cmd[0]);
        

        if(builtinst == 1) continue;
        if(builtinst == 2) {
            for(int j = 0; j < 128; j++) {
                if(cmd[j] == (char*)0)
                cmd[j] = malloc(256);
            }
            split(cmd, ans, ' ');
        }
        
        builtinst = builtin(ans);
        if(builtinst == 1) continue;

        for(int j = 0; ans[j] != '\0'; j++)
            if(ans[j] == '&') __andcnt++;

        for(int i = 0; ~i; i++) {
            if(cmd[i + 1] == (char*)0) break;
            for(int j = 0; cmd[i][j] != '\0'; j++)
                if(cmd[i][j] == '&') __andcnt++;
        }

        for(int i = 0; ~i; i++) {
            if(cmd[i] == (char*)0) break;
            for(int j = 0; cmd[i][j] != '\0'; j++)
                if(cmd[i][j] == '&') {
                    cmd[i][j] = 0;
                    cmd[i] = (char*) 0;
                    break;
                }
        }

        if(__andcnt > 1) {
            printf("%s [info] %s: This terminal does not support multiple tasks running at the same time, please wait for subsequent updates.%s\n", MAGENTA, GREEN, NORMAL);
            continue;
        }

        if(strcmp(hiscmd[hisptr], ans) != 0) {
            hisptr++;
            memcpy(hiscmd[hisptr], ans, strlen(ans));
        }
        trieInsert(ans);
        
        trieInsert(cmd[0]);
        if(strlen(cmd[0]) == 0) continue;

        pid[++pidcnt] = fork();
        __id++;
        if(__andcnt == 0) frontpid = pid[pidcnt];
        else frontpid = 0;
        if(pid[pidcnt] == 0) {
            signal(SIGINT, SIG_IGN);
            signal(SIGQUIT, SIG_IGN);
            signal(SIGTSTP, SIG_IGN);
            exec(cmd[0], cmd, __andcnt ? 1 : 0);
        }
        waitpid(pid[pidcnt], &_status[_statuspos++], __andcnt ? 1 : WUNTRACED);
        frontpid = 0;
        usleep(1000);    
    } 

    return 0;
}
