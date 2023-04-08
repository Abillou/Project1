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
    char* input = malloc(50);
    printf("How Many Children (2, 3, 4)?\n");
    scanf("%d",&maxChildren);//get input from user

    if (maxChildren==0 || (maxChildren < 2) || maxChildren>5)//if the input is not a number......
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
  int result = ceil(log2(PN)); 
  int returnArg = 1;
  printf("Level of tree: %d\n", result);
    

    //Makes N number of childs for Parent without Children making Processes.

    //Idea: Try utilizing the level of the tree to make the processes!    

    pid_t childMaker;


    int pid;
    int start = 0;
    int end = L+50;
       //START THE BFS TREE
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
         increments = (end-start)/maxChildren;
         end = start;
         
        for(int i = 0; i < maxChildren; i++){
            
             
            
            if(childMaker == getpid() && returnArg < PN){
                pid = fork();
                if(i!=0)
                    start = start+increments;
                end = end+increments;   
                returnArg = returnArg+1;
            }
            
        }

        
        if (pid == -1) {  }

        //CHILD PROCESS
        else if (pid == 0) {
                
                printf("Process %d created by process %d with return code %d\n", getpid(), getppid(), returnArg);
                printf("My start is %d and my end is %d\n",start,end);
                pid = getpid();
                if(j == (result-1))
                exit(0);

        
        }
        


        else { // parent process (NO FORKING IN HERE TO KEEP CHAIN FORMAT!) Some ends need to be closed.


                wait(NULL);
                exit(0);
            }
            

            
    }
    
}



