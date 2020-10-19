/* Samson Nguyen
 * 1001496565
 * CSE 3320 Assignment 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>

// merge nums at adjacent segments
// starting at start1 and start2, ending at end
void merge(int *nums, int start1, int start2, int end) {
   if (nums[start2 - 1] <= nums[start2]) {
      return;
   }
   int i;
   int j = start1;
   int k = start2;
   for (i = start1; i < end; i++, j++) {
      if (nums[j] > nums[k]) {
         int temp = nums[k];
         int l = k;
         while (l > j) {
            nums[l] = nums[--l];
         }
         nums[j] = temp;
         k++;
      }
   }
}

int main(void) {
   FILE *file = fopen("all_month.csv", "r");
   int *data, *data2;
   char line[256];
   int lines = 0;

   while (fgets(line, 256, file)) {
      lines++;
   }
   lines -= 1; // the first line is not data
   data = (int*)malloc(lines * sizeof(int));
   data2 = (int*)malloc(lines * sizeof(int));

   fclose(file);

   file = fopen("all_month.csv", "r");
   fgets(line, 256, file); // ignore first line
   for (int i = 0; i < lines; i++) {
      fgets(line, 256, file);
      char *tok = strtok(line, ",");
      tok = strtok(NULL, ",");
      data[i] = atoi(tok);
   }
   fclose(file);

   memcpy(data2, data, lines * sizeof(int));
   char filename[16] = "";
   sprintf(filename, "%d.dat", getpid());
   file = fopen(filename, "w+");
   // write numbers to file
   for (int i = 0; i < lines; i++) {
      fprintf(file, "%d\n", data[i]);
   }
   fclose(file);

   clock_t t = clock();
   // insertionSort(data2, 0, lines);
   char str_lines[50];
   sprintf(str_lines, "%d", lines);
   char *argv[3] = {filename, str_lines, NULL};
   int pid = fork();
   if (pid == 0) {
      execv("./sort", argv);
   } else {
      sleep(2);
   }
   t = clock() - t;
   printf("Time elapsed to sort: %f sec\n", (double)t/CLOCKS_PER_SEC);
}
