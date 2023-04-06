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
    


    //INITIALIZING TWO WAY PIPES OF FOR PN PROCESSES
    int fd[2*(PN)];   //Pipe for a Child to Write to a Parent and for a Parent to Read from a child
    int bd[2*(PN)];   //Pipe for a Parent to Write to a Child and for a Child to Read from a Parent 


    int pid;
    int start = 0;
    int end = 0;


    int parentRoot = getpid();  //Tracks the FIRST PROCESS OF THE CHAIN
    int returnArg = 1;

    //START THE DFS CHAIN! NODE -> CHILD -> CHILD OF CHILD -> CHILD OF CHILD OF CHILD ETC.
    for (int i=0; i<PN; i++) 
    {

        //Initiate the Pipes.
        pipe(&fd[2*i]);
        pipe(&bd[2*i]);
        
        pid = fork();
        
        if (pid == -1) {  }

        //CHILD PROCESS
        else if (pid == 0) {

 
            printf("Hi I'm process %d with return arg %d and my parent is %d.\n", getpid(), returnArg+1, getppid());
            returnArg++;
            start = end;
            end = end+(L+50)/PN;
            if(end > (L+50))
            end = (L+50);
            
            //IF IT IS THE LAST PROCESS IN THE CHAIN
            if(i == (PN-1)){
            // CHAIN LOOKS LIKE 

            //           ----   PIPE TO WRITE TO PARENT --------          
            //         |                                       |                                            
            //  ---PREVIOUS PROCESS -------------------------CURRENT PROCESS
            //         |                                       |   
            //           ----  PIPE TO READ FROM PARENT --------
            // FOR VISUALIZATION                 
                
                close(fd[2*i]);
                close(bd[2*i]+1);
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

                read(bd[2*i], &H, sizeof(int));
                close(bd[2*i]);



                for(int i = start; i < end; i++){
                    if(H != 0 && array[i] == -1){
                        printf("Hi I am Process %d with return argument %d and I found the hidden key at position A[%d].\n", getpid(), returnArg, i);
                        H--;
                }
                }
                exit(0);

            }
        }


        else { // parent process (NO FORKING IN HERE TO KEEP CHAIN FORMAT!) Some ends need to be closed.
            int tempMax;
            int64_t tempAvg;
            int tempCount;
            int max = 0;
            int64_t avg = 0;
            int count =0;

            //IF NOT THE START OF THE CHAIN
            if(parentRoot != getpid()){

            // CHAIN LOOKS LIKE 

            //           ----   PIPE TO WRITE TO PARENT --------          ----   PIPE TO WRITE TO CHILD --------
            //           |                                       |       |                                       |   
            //   ------PREV---------------------------------CURRENT PROCESS ------------------------------------NEXT---------
            //           |                                       |       |                                       |   
            //           ----  PIPE TO READ FROM PARENT --------         ----  PIPE TO READ FROM CHILD --------
            // FOR VISUALIZATION 
            close(fd[2*i]+1);
            close(fd[2*(i-1)]);
            close(bd[2*(i)]);
            close(bd[2*(i-1)+1]);

            for(int j = start; j < end; j++){
                    if (array[j] > max)
                        max = array[j];
                    avg += array[j];     
            }
            avg = avg / (end - start);
            count = end - start;
            
            

            read(fd[2*i], &tempMax, sizeof(int));
            read(fd[2*i], &tempAvg, sizeof(int64_t));
            read(fd[2*i], &tempCount, sizeof(int));
            close(fd[2*i]);

            if(tempMax >= max){
                max = tempMax;
            }
            avg = (avg*count + tempAvg * tempCount)/(tempCount+count);                
               
            write(fd[2*(i-1)+1], &max, sizeof(int));
            write(fd[2*(i-1)+1], &avg, sizeof(int64_t));
            write(fd[2*(i-1)+1], &count, sizeof(int));
            close(fd[2*(i-1)+1]);

            read(bd[2*(i-1)], &H, sizeof(int));
            close(bd[2*(i-1)]);

            for(int i = start; i < end; i++){
                if(H != 0 && array[i] == -1){
                    printf("Hi I am Process %d with return argument %d and I found the hidden key at position A[%d].\n", getpid(), returnArg, i);
                    H--;
                }
            }
             
            write(bd[2*(i)+1], &H, sizeof(int));
            close(bd[2*(i)+1]);
            wait(NULL);
            exit(0);
            
            }

            //THE START OF THE CHAIN
            else{

            // CHAIN LOOKS LIKE 

            //           ----   PIPE TO WRITE TO CHILD --------          
            //           |                                       |    
            //  CURRENT PROCESS---------------------------------NEXT PROCESS ----------------------
            //           |                                       |       
            //           ----  PIPE TO READ FROM CHILD --------         
            // FOR VISUALIZATION 
                close(bd[2*i]);
                close(fd[2*i+1]);
                read(fd[2*i], &max, sizeof(int));
                read(fd[2*i], &avg, sizeof(int64_t));
                read(fd[2*i], &count, sizeof(int));
                close(fd[2*i]);
                printf("Max: %d, Avg: %ld\n\n", max, avg);
                write(bd[2*i+1], &H, sizeof(int));
                close(bd[2*i]+1);
                wait(NULL);
                exit(0);
            }
            

            
        }
    }
    



}
