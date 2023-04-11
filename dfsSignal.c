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
bool ran = false;


void sig_handler(int signum){
    sleep(100);
    ran = true;
}

void sig_handler2(int signum){
    ran = false;
}


void kill_process_and_children(pid_t pid) {
    // send SIGKILL signal to process and its children
    char command[50];
    sprintf(command, "pkill -TERM -P %d", pid);
    system(command);
}
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
    double time_spent = 0.0;
    clock_t begin = clock();
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
                int max = 0;
                int64_t avg = 0;
                int count = 0;

                pid_t childPid = getpid();
                int hiddenNodes = 0;
                int firstKey = -1;
                for(int j = start; j < end; j++){
                    if (array[j] > max)
                        max = array[j];
                    avg += array[j];
                    
                    if(array[j] == -1){
                        hiddenNodes++;
                        if(firstKey == -1)
                        firstKey = array[j];
                    }
                }
                avg = avg / (end - start);
                count = end-start;
            
                write(fd[2*i+1], &max, sizeof(int));
                write(fd[2*i+1], &avg, sizeof(int64_t));
                write(fd[2*i+1], &count, sizeof(int));

                write(fd[2*i+1], &childPid, sizeof(pid_t));
                write(fd[2*i+1], &hiddenNodes, sizeof(int));
                write(fd[2*i+1], &firstKey, sizeof(int));
                close(fd[2*i+1]);
                signal(SIGCONT, sig_handler);
                raise(SIGTSTP);
                



    

                for(int i = start; i < end; i++){
                    if(H != 0 && array[i] == -1){
                        printf("Hi I am Process %d with return argument %d and I found the hidden key at position A[%d].\n", getpid(), returnArg, i);
                        H--;
                }
                }
                if(!ran)
                sleep(20);
                
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
            int hiddenNodes = 0;
            int firstKey;
            int cfirstKey;
            pid_t childPid;
            int cHiddenNodes;
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


            for(int j = start; j < end; j++){
                    if (array[j] > max)
                        max = array[j];
                    avg += array[j];
                    if(array[j] == -1){
                        hiddenNodes++;
                        if(firstKey == -1)
                        firstKey = array[j];
                    }     
            }
            avg = avg / (end - start);
            count = end - start;
            
            

            read(fd[2*i], &tempMax, sizeof(int));
            read(fd[2*i], &tempAvg, sizeof(int64_t));
            read(fd[2*i], &tempCount, sizeof(int));
            read(fd[2*i], &childPid, sizeof(pid_t));
            read(fd[2*i], &cHiddenNodes, sizeof(int));
            read(fd[2*i], &cfirstKey, sizeof(int));
            close(fd[2*i]);
            if(tempMax >= max){
                max = tempMax;
                kill(childPid, SIGCONT);
            }
            else if(cHiddenNodes >= H){

                kill_process_and_children(childPid);
            }
            else{
                kill(childPid, SIGCONT);
                
            }
            avg = (avg*count + tempAvg * tempCount)/(tempCount+count);                
            childPid = getpid();
            write(fd[2*(i-1)+1], &max, sizeof(int));
            write(fd[2*(i-1)+1], &avg, sizeof(int64_t));
            write(fd[2*(i-1)+1], &count, sizeof(int));
            write(fd[2*(i-1)+1], &childPid, sizeof(pid_t));
            write(fd[2*(i-1)+1], &hiddenNodes, sizeof(int));
            write(fd[2*(i-1)+1], &firstKey, sizeof(int));
            close(fd[2*(i-1)+1]);
            signal(SIGCONT, sig_handler);
            signal(SIGFPE, sig_handler2);
            raise(SIGTSTP);

            for(int i = start; i < end; i++){
                if(H != 0 && array[i] == -1){
                    printf("Hi I am Process %d with return argument %d and I found the hidden key at position A[%d].\n", getpid(), returnArg, i);
                    H--;
                }
            }
            if(!ran)
            sleep(20);
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
                close(fd[2*i+1]);
                read(fd[2*i], &max, sizeof(int));
                read(fd[2*i], &avg, sizeof(int64_t));
                read(fd[2*i], &count, sizeof(int));
                read(fd[2*i], &childPid, sizeof(pid_t));
                read(fd[2*i], &cHiddenNodes, sizeof(int));
                read(fd[2*i], &cfirstKey, sizeof(int));
                close(fd[2*i]);
                if(tempMax >= max){
                    max = tempMax;
                    kill(childPid, SIGCONT);
                }
                else if(cHiddenNodes >= H){
                    kill_process_and_children(childPid);
                }
                else{
                                    kill(childPid, SIGCONT);

                }
                printf("Max: %d, Avg: %ld\n\n", max, avg);

                int status;

                clock_t end = clock();
                time_spent += (double)(end-begin) / CLOCKS_PER_SEC;
                printf("\nThe program completed in %f seconds\n", (time_spent ));
                exit(0);
            }
            

            
        }
    }
    



}
