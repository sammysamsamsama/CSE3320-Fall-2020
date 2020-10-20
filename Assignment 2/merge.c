/* merge program */

#include <stdio.h>
#include <stdlib.h>

// read 2 files, 1 int per line, merge into destination file
// argv has filename1, number of lines,
//    filename2, number of lines,
//    and destination filename
int main(int argc, char *argv[]) {
   int size1 = atoi(argv[1]);
   int size2 = atoi(argv[3]);
   double *nums1 = (double*)malloc(size1 * sizeof(double));
   double *nums2 = (double*)malloc(size2 * sizeof(double));
   double *dest = (double*)malloc((size1 + size2) * sizeof(double));
   FILE *file1 = fopen(argv[0], "r");
   FILE *file2 = fopen(argv[2], "r");
   FILE *file3 = fopen(argv[4], "w+");

   // read numbers from files to arrays
   for (int i = 0; i < size1; i++) {
      fscanf(file1, "%lf", &(nums1[i]));
   }
   fclose(file1);
   for (int i = 0; i < size2; i++) {
      fscanf(file2, "%lf", &(nums2[i]));
   }
   fclose(file2);

   // merge nums1 and nums2 into dest
   int i = 0, size0 = size1 + size2;
   if (nums1[size1 - 1] <= nums2[0]) {
      while (i < size1) {
         dest[i] = nums1[i++];
      }
      while (i < size0) {
         dest[i] = nums2[i++];
      }
   } else {
      int j = 0, k = 0;
      while (i < size0) {
         if (nums1[j] <= nums2[k]) {
            dest[i++] = nums1[j++];
         } else {
            dest[i++] = nums2[k++];
         }
         if (j >= size1) {
            while (i < size0 && k < size2) {
               dest[i++] = nums2[k++];
            }
				break;
         } else if (k >= size2) {
            while (i < size0 && j < size1) {
               dest[i++] = nums1[j++];
            }
				break;
         }
      }
   }

   // write numbers to file
   for (int i = 0; i < size0; i++) {
      fprintf(file3, "%lf\n", dest[i]);
   }
   fflush(file3);
   fclose(file3);
   return 0;
}
