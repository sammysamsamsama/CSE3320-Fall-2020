/* insertion sort program */

#include <stdio.h>
#include <stdlib.h>

// read file, 1 int per line, sort, write file 1 int per line
// argv has filename and number of lines
int main(int argc, char *argv[]) {
   int size = atoi(argv[1]);
   int *nums = (int*)malloc(size * sizeof(int));
   int i, j, num;
   FILE *file = fopen(argv[0], "r");

   // read numbers from file to nums
   for (i = 0; i < size; i++) {
      fscanf(file, "%d", &nums[i]);
   }
   fclose(file);

   // sort numbers
   for (i = 0; i < size; i++) {
      num = nums[i];
      for (j = i - 1; j >= 0 && nums[j] > num; j--) {
         nums[j + 1] = nums[j];
      }
      nums[j + 1] = num;
   }

   file = fopen(argv[0], "w+");
   // write numbers to file
   for (i = 0; i < size; i++) {
      fprintf(file, "%d\n", nums[i]);
   }
   fclose(file);
}
