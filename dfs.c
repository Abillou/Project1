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

    FILE* file =  fopen("keys.txt", "r");   
    int* array = (malloc((L+50)*sizeof(int)));
    char* line = malloc(256);
    
    for(int i = 0; i < (L+50); i++){
        fgets(line, sizeof(line), file);
        array[i] = atoi(line);
    }
    


    //INITIALIZING PIPES OF FOR PN PROCESSES
    int fd[2*(PN)];
 
    int pid;
    int start = 0;
    int end = L+50;
    int pos[H];
    int parentRoot = getpid();
    int returnArg = 1;
    //START THE DFS CHAIN! PARENT -> CHILD -> CHILD OF CHILD ETC.
    for (int i=0; i<PN; i++) 
    {


        pipe(&fd[2*i]);
        end = start+ (end-start)/2;
        pid = fork();
        returnArg++;
        if (pid == -1) {  }

        //CHILD PROCESS
        else if (pid == 0) {
            printf("Hi I'm process %d with return arg %d and my parent is %d.\n", getpid(), returnArg, getppid());
            start = end;
            end = L+50;
            
            if(i == (PN-1)){
                close(fd[2*i]);
                 int max = 0;
                int64_t avg = 0;
                int count = 0;
                for(int j = start; j < end; j++){
                    if (array[j] > max)
                        max = array[j];
                    avg += array[j];
                        
                }
                avg = avg / (end - start);
                count = end-start;
                write(fd[2*i+1], &max, sizeof(int));
                write(fd[2*i+1], &avg, sizeof(int64_t));
                write(fd[2*i+1], &count, sizeof(int));
                close(fd[2*i+1]);

                exit(0);

            }
        }


        else { // parent process (NO FORKING IN HERE!) Some ends need to be closed.
            
            int tempMax;
            int64_t tempAvg;
            int tempCount;
            int max = 0;
            int64_t avg = 0;
            int count =0;
            for(int j = start; j < end; j++){
                    if (array[j] > max)
                        max = array[j];
                    avg += array[j];     
            }
            avg = avg / (end - start);
            count = end - start;
            
            wait(NULL);
            close(fd[2*i+1]);

            read(fd[2*i], &tempMax, sizeof(int));
            read(fd[2*i], &tempAvg, sizeof(int64_t));
            read(fd[2*i], &tempCount, sizeof(int));

            if(tempMax >= max){
                max = tempMax;
            }
            avg = (avg*count + tempAvg * tempCount)/(tempCount+count);
            if(parentRoot != getpid()){
               
            write(fd[2*(i-1)+1], &max, sizeof(int));
            write(fd[2*(i-1)+1], &avg, sizeof(int64_t));
            write(fd[2*(i-1)+1], &count, sizeof(int));
            
            }
            
            if(parentRoot == getpid()){
                printf("Max: %d, Avg: %ld\n", max, avg);
            }
            
            exit(0);
            
        }
    }
    



}
