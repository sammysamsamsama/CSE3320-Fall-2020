/* Samson Nguyen
 * 1001496565
 * CSE 3320 Assignment 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>

void insertionSort(int *nums, int size) {
   int i, j, num;
   for (i = 1; i < size; i++) {
      num = nums[i];
      for (j = i - 1; j >= 0 && nums[j] > num; j--) {
         nums[j + 1] = nums[j];
      }
      nums[j + 1] = num;
   }
}

int main(void) {
   FILE *file = fopen("all_month.csv", "r");
   int *data;
   char line[256];
   int lines = 0;

   while (fgets(line, 256, file)) {
      lines++;
   }
   lines -= 1; // the first line is not data

   fclose(file);
   file = fopen("all_month.csv", "r");
   data = (int*)malloc(lines * sizeof(int));

   fgets(line, 256, file); // ignore first line
   for (int i = 0; i < lines; i++) {
      fgets(line, 256, file);
      char *tok = strtok(line, ",");
      tok = strtok(NULL, ",");
      data[i] = atoi(tok);
   }
   clock_t t = clock();
   insertionSort(data, lines);
   t = clock() - t;
   printf("Time elapsed to sort: %f sec\n", (double)t/CLOCKS_PER_SEC);

   fclose(file);
}
