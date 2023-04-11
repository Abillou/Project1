#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
int L, H, PN;

void generateTextFile() {
  // Initializes Array of Locations
  int* loc = malloc((L + 50) * 4);
  for (int i = 0; i < L + 50; i++) {
    loc[i] = 0;
  }

  int count = 0;
  srand(time(NULL));
  int randomNumber;

  // Picks random indexs to be -1 (No Duplicates)
  while (count < 50) {
    randomNumber = rand() % (L + 50);
    if (loc[randomNumber] == 0) {
      loc[randomNumber] = -1;
      count++;
    }
  }

  // Creates/Overwrites a text file name keys
  FILE* keys;
  keys = fopen("keys.txt", "w");

  for (int i = 0; i < (L + 50); i++) {
    // Generates a random positive integer.
    randomNumber = rand() % 10000;

    // If the corresponding index to the array is -1, place -1 in the file
    if (loc[i] == -1) {
      fputs("-1\n", keys);
    }

    // Otherwise, place the randomly generated number.
    else
      fprintf(keys, "%d\n", randomNumber);
  }

  // Close file.
  fclose(keys);
  free(loc);
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    printf("Not enough arguements");
    return -1;
  }

  L = atoi(argv[1]);
  H = atoi(argv[2]);
  PN = atoi(argv[3]);

  generateTextFile();

  int64_t avg = 0;
  int64_t count = 0;
  int64_t validNumbers = 0;

  int keyCount = 0;
  int max = 0;  // Can be 0 since no negative ints aside from hidden keys.
  FILE* file = fopen("keys.txt", "r");
  char* line = malloc(256);

  FILE* output;
  output = fopen("output.txt", "a+");
  while (fgets(line, 256, file)) {
    int num = atoi(line);
    if (num > max) max = num;
    if (num != -1) {
      avg += num;
      validNumbers++;
    }

    if (keyCount < H && num == -1) {
      printf("Found hidden key at index %ld\n", count);
      fprintf(output,
              "Hi I am process %d with return arg 1. I found the hidden key in "
              "position A[%ld].\n",
              getpid(), count);
      keyCount++;
    }
    count++;
  }

  fclose(file);

  avg /= validNumbers;  // Calculate the average by dividing the sum by the
                        // number of valid numbers.
  printf("Max: %d\nAverage: %ld\n", max, avg);
  fprintf(output, "Max: %d\nAverage: %ld\n", max, avg);
  fclose(output);
  free(line);
}