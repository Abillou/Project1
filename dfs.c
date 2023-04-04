#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <signal.h>
#include <time.h>  
#include <stdbool.h>
int L, H, PN;

void generateTextFile(){
    //Initializes Array of Locations
    int *loc = malloc( (L+50)*4);
    for(int i = 0; i < L+50; i++)
        loc[i] = 0;
    
    int count = 0;
    srand(time(NULL));
    int randomNumber;
    
    //Picks random indexs to be -1 (No Duplicates)
    while(count < 50){
        randomNumber =  (rand() % (L+50));
        if (loc[randomNumber] == 0){
            loc[randomNumber] = -1;
            count++;
            
        } 
    } 

    //Creates/Overwrites a text file name keys
    FILE *keys;
    keys = fopen("keys.txt", "w");

    
    for(int i = 0; i < (L+50); i++){
        //Generates a random positive integer. 
        randomNumber = rand() % 10000; 

        //If the corresponding index to the array is -1, place -1 in the file
        if(loc[i] == -1){
            fputs("-1\n", keys);
        }

        //Otherwise, place the randomly generated number.
        else
            fprintf(keys, "%d\n", randomNumber);      
    }

    //Close file.
    fclose(keys);
    free(loc);
}

int main(int argc, char* argv[]){
    if(argc != 4){
        printf("Not enough arguements");
        return -1;
    }

    L = atoi(argv[1]);
    H = atoi(argv[2]);
    PN = atoi(argv[3]);

    generateTextFile();

    int pid;
    int root = getpid();
    //INITIALIZING PIPES OF FOR PN PROCESSES
    int fd[2*(PN)];
    for (int i = 0; i < (PN); i++) {
    pipe(&fd[2*i]);
    }
    int start =0; 
    int end;
    int keyCount = 0;
    int tempMax; 
    int tempAvg; 
    FILE* file =  fopen("keys.txt", "r");
    //START THE DFS CHAIN! PARENT -> CHILD -> CHILD OF CHILD ETC.
    for (int i=0; i<PN; i++) 
    {
        if(end == 0)
        end = L/2;

        else
        end = end + end/2;

        pid = fork();
        if (pid == -1) {  }
        else if (pid == 0) {
            start = end;

            read(fd[i*2], &keyCount, sizeof(int));
            read(fd[i*2], &file, sizeof(file));
           

        }
        else { // parent process (NO FORKING IN HERE!)
            int64_t avg;
            int64_t count=0;


            int max = 0; //Can be 0 since no negative ints aside from hidden keys.
            
            char* line = malloc(256);
            while (start != end && fgets(line, sizeof(line), file)) {
                if(atoi(line) > max)
                    max = atoi(line);

                avg = avg + atoi(line);
                if(keyCount < H && atoi(line) == -1){
                    printf("Process %d Here! Found hidden key at index %d\n", getpid(), start);
                    keyCount++;
                }
                count++;
                start++;
            }
            int c = getc(file);
            write(fd[i*2+1], &keyCount, sizeof(int));
            write(fd[i*2+1], &file, sizeof(file));
            write(fd[i*2+1], &max, sizeof(int));
            write(fd[i*2+1], &avg, sizeof(int));
            
            close(fd[i*2+1]);
            fclose(file);

            if(getpid() == (root +1)){
            
            read(fd[i*2], &tempMax, sizeof(int));

            read(fd[i*2], &tempMax, sizeof(int));
            }

           close(fd[i*2]);
            free(line);

    
            wait(NULL);
            if(root == getpid()){
                
                 printf("Max: %d\nAverage: %ld\n", max, avg/(count));
            }

            
            exit(0);
        }
    }




}
