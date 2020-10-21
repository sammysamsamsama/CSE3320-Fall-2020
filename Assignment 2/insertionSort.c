/* insertion sort program */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

// read file, 1 int per line, sort, write file 1 int per line
// argv has filename and number of lines
int main(int argc, char *argv[]) {
   int size = atoi(argv[1]);
   t_data *nums = (t_data*)malloc(size * sizeof(t_data));
   t_data num;
   int i, j;
   FILE *file = fopen(argv[0], "r");
   char line[256];

   // read numbers from file to nums
   for (i = 0; i < size; i++) {
      // fscanf(file, "%lf", &nums[i]);
      fgets(line, 256, file);
      char *tok = strtok(line, ",");
		strcpy(nums[i].time, tok);
      tok = strtok(NULL, ",");
		nums[i].latitude = atof(tok);
      tok = strtok(NULL, ",");
		nums[i].longitude = atof(tok);
      tok = strtok(NULL, ",");
		nums[i].depth = atof(tok);
      tok = strtok(NULL, ",");
		nums[i].mag = atof(tok);
   }
   fclose(file);

   // sort numbers
   for (i = 0; i < size; i++) {
      num = nums[i];
      for (j = i - 1; j >= 0 && nums[j].latitude > num.latitude; j--) {
         nums[j + 1] = nums[j];
      }
      nums[j + 1] = num;
   }

   file = fopen(argv[0], "w+");
   // write numbers to file
   for (i = 0; i < size; i++) {
      fprintf(file, "%s,%lf,%lf,%f,%f\n", nums[i].time, nums[i].latitude, nums[i].longitude, nums[i].depth, nums[i].mag);
   }
   fflush(file);
   fclose(file);
}
