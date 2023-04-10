#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <signal.h>
#include <time.h>  
#include <stdbool.h>
#include <math.h>
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
    int maxChildren;
    int parentRoot = getpid();
    char* input = malloc(50);
    printf("How Many Children (2, 3, 4)?\n");
    scanf("%d",&maxChildren);//get input from user

    if (maxChildren==0 || (maxChildren < 2) || maxChildren>=5)//if the input is not a number......
    {
        printf("Invalid input entered\n\n");
        return -1;
    }
    printf("\n");
    
    generateTextFile();
    
    FILE* file =  fopen("keys.txt", "r");   
    int* array = (malloc((L+50)*sizeof(int)));
    char* line = malloc(256);
    
    for(int i = 0; i < (L+50); i++){
        fgets(line, sizeof(line), file);
        array[i] = atoi(line);
    }
  int result = floor(log2(PN)); 
  int returnArg = 1;
    

    //Makes N number of childs for Parent without Children making Processes.

    //Idea: Try utilizing the level of the tree to make the processes!    

    pid_t childMaker;

    int childCounter=-1;
    int fd[2*PN];
    int pid;
    int start = 0;
    int end = L+50;
       //START THE BFS TREE
    int childTrack[4] = {-1, -1, -1, -1};
    int parentPipe = -1;
    int oldEnd;
    for (int j=0; j<result; j++) 
    {
        if(j!=0){
            if(maxChildren == 2)
            returnArg = 2*(returnArg)-1;
            else if(maxChildren == 3)
            returnArg = 3*(returnArg)-2;
            else
            returnArg = 4*(returnArg)-3;
        }
        childMaker = getpid();
        int increments;
         increments = ceil((end-start)/maxChildren);
         oldEnd = end;
         end = start;
         bool found = false;
        for(int i = 0; i < maxChildren; i++){
            
            if(childMaker == getpid() && returnArg < PN){
                childCounter++;
                
                for(int l = 0; l < maxChildren; l++)
                if(childTrack[l]==-1 ){
                    childTrack[l] = returnArg-1;
                    
                    break;
                }
                
                pipe(&fd[2*(returnArg-1)]);
                pid = fork();
                if(i!=0)
                    start = start+increments;
                end = end+increments;
                if((L+50)-end < 5)
                end = L+50;   
                returnArg = returnArg+1;
            }
 
            
        }

        
        if (pid == -1) {  }

        //CHILD PROCESS
        else if (pid == 0) {
                
            
            for(int l = 0; l < maxChildren; l++){
            if(childTrack[l]!=-1){
                parentPipe = childTrack[l];
                childTrack[l] = -1;
            }
            }

            printf("Process %d created by process %d with return code %d\n", getpid(), getppid(), returnArg);
            pid = getpid();
            if(j == (result-1)){
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
                close(fd[2*parentPipe]);
                write(fd[2*parentPipe+1], &max, sizeof(int));
                write(fd[2*parentPipe+1], &avg, sizeof(int64_t));
                write(fd[2*parentPipe+1], &count, sizeof(int));
                close(fd[2*parentPipe+1]);
                
            exit(0);
            }

        
        }
        


        else { // parent process (NO FORKING IN HERE TO KEEP CHAIN FORMAT!) Some ends need to be closed.
                bool hasChildren = false;
                int childCount = 0;
                int max = 0;
                bool tracked = false;
                int64_t avg = 0;
                int count = 0;
                int tempMax = -1;
                int tempAvg = -1;
                int tempCount = -1;
                for(int r = 0; r < maxChildren; r++){
                    if(childTrack[r] != -1){
                        hasChildren = true;
                        childCount++;
                    }

                    else if(childTrack[r] == -1 && hasChildren && !tracked){
                        start = end;
                        end = oldEnd;
                        tracked = true;
                        
                    }
                    
                }

                if(!hasChildren){
                    end = oldEnd;
                }

                
                if(childCount == 0) //If process with no children end up here
                {
                    for(int j = start; j < end; j++){
                        if (array[j] > max)
                            max = array[j];
                        avg += array[j];
                            
                    }
                    avg = avg / (end - start);
                    count = end-start;
                }

                else if(childCount < maxChildren) //If a process with not the max children ends up here
                {

                    for(int j = start; j < end; j++){
                        if (array[j] > max)
                            max = array[j];
                        avg += array[j];
                            
                    }
                    avg = avg / (end - start);
                    count = end-start;
                    
                    for(int r = 0; r < childCount; r++){
                        //PIPE CALLED ONE
                        read(fd[2*childTrack[r]], &tempMax, sizeof(int));
                        read(fd[2*childTrack[r]], &tempAvg, sizeof(int64_t));
                        read(fd[2*childTrack[r]], &tempCount, sizeof(int));
                    
                        if(tempMax >= max){
                            max = tempMax;
                        }
                        avg = (avg*count + tempAvg * tempCount)/(tempCount+count);
                        count += tempCount; 
                    }


                }

                else{
                    for(int r = 0; r < childCount; r++){
                        //PIPE CALLED TWO
                        if(r!=0){
                            
                        read(fd[2*childTrack[r]], &tempMax, sizeof(int));
                        read(fd[2*childTrack[r]], &tempAvg, sizeof(int64_t));
                        read(fd[2*childTrack[r]], &tempCount, sizeof(int));
                        
                        if(tempMax >= max){
                            max = tempMax;
                        }
                       avg = (avg*count + tempAvg * tempCount)/(tempCount+count); 
                        }
                        else{
                            read(fd[2*childTrack[r]], &max, sizeof(int));
                            read(fd[2*childTrack[r]], &avg, sizeof(int64_t));
                            read(fd[2*childTrack[r]], &count, sizeof(int));
                        }
                        
                    }
                }

                if(parentRoot != getpid()){

                    //PIPE CALLED THREE
                    write(fd[2*parentPipe+1], &max, sizeof(int));
                    write(fd[2*parentPipe+1], &avg, sizeof(int64_t));
                    write(fd[2*parentPipe+1], &count, sizeof(int));
                }

                else{
                    wait(NULL);

                    printf("Max: %d, Avg: %ld\n", max, avg);
                }
                //printf("Parent Process %d: My start is %d and my end is %d\n", getpid(), start, end);
                wait(NULL);
                exit(0);
            }
            

            
    }
    
}



