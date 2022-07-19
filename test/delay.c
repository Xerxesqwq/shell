#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    printf("delay pid: %d\n", getpid());
    for(int i = 1; i <= 10; i++) {
        sleep(1);
        printf("delay %d\n", i);
    }
    return 1;
}
