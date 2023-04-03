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
        randomNumber = rand() % (L+50);
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

    int64_t avg;
    int64_t count=0;

    int keyCount = 0;
    int max = 0; //Can be 0 since no negative ints aside from hidden keys.
    FILE* file =  fopen("keys.txt", "r");
    char* line = malloc(256);

    while (fgets(line, sizeof(line), file)) {
        if(atoi(line) > max)
        max = atoi(line);

        avg = avg + atoi(line);
        if(keyCount < H && atoi(line) == -1){
            printf("Found hidden key at index %ld\n", count);
            keyCount++;
        }
        count++;
    }
    
    fclose(file);

    printf("Max: %d\nAverage: %ld\n", max, avg/(count));
    free(line);


}
