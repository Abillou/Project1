#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <signal.h>
#include <time.h>  

int L, H, PN;

void generateTextFile(){
    
}

int main(int argc, char* argv[]){
    if(argc != 4){
        printf("Not enough arguements");
        return;
    }

    L = atoi(argv[1]);
    H = atoi(argv[2]);
    PN = atoi(argv[3]);



}
