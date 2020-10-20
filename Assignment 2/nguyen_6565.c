/* Samson Nguyen
 * 1001496565
 * CSE 3320 Assignment 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct DATA {
	char time[25];
	double latitude;
	double longitude;
	float depth;
	float mag;
} DATA;

void distribute_data(int num_ps, double data[], int lines) {
	FILE *file;
	char filename[10];
   for (int div = 0; div < num_ps; div++) {
      sprintf(filename, "%d.dat", div);
      file = fopen(filename, "w+");
      for (int i = div * lines / num_ps; i < (div + 1) * lines / num_ps; i++) {
         fprintf(file, "%f\n", data[i]);
      }
      fclose(file);
   }
}

void merge(const char *f1, int l1, const char *f2, int l2, const char *d) {
	int size1 = l1;
   int size2 = l2;
   double *nums1 = (double*)malloc(size1 * sizeof(double));
   double *nums2 = (double*)malloc(size2 * sizeof(double));
   double *dest = (double*)malloc((size1 + size2) * sizeof(double));
   FILE *file1 = fopen(f1, "r");
   FILE *file2 = fopen(f2, "r");
   FILE *file3 = fopen(d, "w+");

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
}

int main(void) {
   FILE *file = fopen("all_month.csv", "r");
   double *data, *data2;
   char line[256];
   int lines = 0;

   while (fgets(line, 256, file)) {
      lines++;
   }
   lines -= 1; // the first line is not data
   data = (double*)malloc(lines * sizeof(double));
   data2 = (double*)malloc(lines * sizeof(double));

   fclose(file);

   file = fopen("all_month.csv", "r");
   fgets(line, 256, file); // ignore first line
   for (int i = 0; i < lines; i++) {
      fgets(line, 256, file);
      char *tok = strtok(line, ",");
      tok = strtok(NULL, ",");
      data[i] = atof(tok);
   }
   fclose(file);


   // Instrument 1 ps
   memcpy(data2, data, lines * sizeof(double));
	int num_ps = 1;
   distribute_data(num_ps, data, lines);

	char filename[10];
   char str_lines[50];
	strcpy(filename, "0.dat");
   sprintf(str_lines, "%d", lines);
   char *argv[6] = {filename, str_lines, NULL, NULL, NULL, NULL};
   argv[2] = (char*)malloc(50);
   argv[3] = (char*)malloc(50);
   argv[4] = (char*)malloc(50);

   clock_t t = clock();
   int pid = fork();
   if (pid == 0) {
      execv("./sort", argv);
      return 0;
   }
   wait(NULL);
   t = clock() - t;
   printf("Time elapsed to sort with 1 ps: %f sec\n", (double)t/CLOCKS_PER_SEC);
   system("mv ./*.dat ./ps1_data/");

   // Instrument 2 ps
   memcpy(data2, data, lines * sizeof(double));
	num_ps = 2;
   distribute_data(num_ps, data, lines);

   t = clock();
   pid = fork();
   if (pid == 0) {
      // child process
      int p1 = fork();
      if (p1 > 0) {
         // child-parent
         sprintf(str_lines, "%d", lines / num_ps);
         strcpy(argv[0],"0.dat");
         argv[1] = str_lines;
         execv("./sort", argv);
         return 0;
      } else {
         // child-child
         sprintf(str_lines, "%d", lines - lines / num_ps);
         strcpy(argv[0],"1.dat");
         argv[1] = str_lines;
         execv("./sort", argv);
         return 0;
      }
   }
   wait(NULL);
	t = clock() - t;
	printf("Time elapsed to sort with %d ps: %f sec\n", num_ps, (double)t/CLOCKS_PER_SEC);
	merge("0.dat", lines / num_ps, "1.dat", lines - lines / num_ps, "2.dat");
   system("mv ./*.dat ./ps2_data/");
   // system("rm *.dat");

   // Instrument 4 ps
   memcpy(data2, data, lines * sizeof(double));
   num_ps = 4;
   distribute_data(num_ps, data, lines);

   t = clock();
   // sort every 4th of the data
   pid = fork();
   if (pid == 0) {
      // child process
      int p1 = fork();
      if (p1 == 0) {
         // child-child
         int p2 = fork();
         if (p2 == 0) {
            // child-child-child
            sprintf(str_lines, "%d", lines / num_ps);
            strcpy(argv[0],"0.dat");
            argv[1] = str_lines;
            execv("./sort", argv);
            return 0;
         }
         sprintf(str_lines, "%d", lines / num_ps);
         strcpy(argv[0],"1.dat");
         argv[1] = str_lines;
         execv("./sort", argv);
         return 0;
      }
      // child-parent
      p1 = fork();
      if (p1 == 0) {
         // child-parent-child
         int p2 = fork();
         if (p2 == 0) {
            sprintf(str_lines, "%d", lines / num_ps);
            strcpy(argv[0],"2.dat");
            argv[1] = str_lines;
            execv("./sort", argv);
            return 0;
         }
      }
      sprintf(str_lines, "%d", lines - 3 * lines / num_ps);
      strcpy(argv[0],"3.dat");
      argv[1] = str_lines;
      execv("./sort", argv);
      return 0;
   }
   while(wait(&pid) != -1);
	t = clock() - t;
	printf("Time elapsed to sort with %d ps: %f sec\n", num_ps, (double)t/CLOCKS_PER_SEC);
	merge("0.dat", lines / num_ps, "1.dat", lines / num_ps, "4.dat");
	merge("2.dat", lines / num_ps, "3.dat", lines - lines / num_ps, "5.dat");
	merge("4.dat", 2 * lines / num_ps, "5.dat", lines - 2 * lines / num_ps, "6.dat");
	system("mv ./*.dat ./ps4_data/");

   // Instrument 10 ps
   memcpy(data2, data, lines * sizeof(double));
   num_ps = 10;
   distribute_data(num_ps, data, lines);

   t = clock();
   // sort every 10th of the data
   int p1 = fork();
   int p2 = fork();
   int p3 = fork();
   if (p1 == 0 && p2 == 0 && p3 == 0) {
      // 0.dat
      sprintf(str_lines, "%d", lines / num_ps);
      strcpy(argv[0],"0.dat");
      argv[1] = str_lines;
      execv("./sort", argv);
      return 0;
   } else if (p1 == 0 && p2 == 0 && p3 > 0) {
      // 1.dat
      sprintf(str_lines, "%d", lines / num_ps);
      strcpy(argv[0],"1.dat");
      argv[1] = str_lines;
      execv("./sort", argv);
      return 0;
   } else if (p1 == 0 && p2 > 0 && p3 == 0) {
      // 2.dat
      sprintf(str_lines, "%d", lines / num_ps);
      strcpy(argv[0],"2.dat");
      argv[1] = str_lines;
      execv("./sort", argv);
      return 0;
   } else if (p1 == 0 && p2 > 0 && p3 > 0) {
      // 3.dat
      sprintf(str_lines, "%d", lines / num_ps);
      strcpy(argv[0],"3.dat");
      argv[1] = str_lines;
      execv("./sort", argv);
      return 0;
   } else if (p1 > 0 && p2 == 0 && p3 == 0) {
      // 4.dat
      sprintf(str_lines, "%d", lines / num_ps);
      strcpy(argv[0],"4.dat");
      argv[1] = str_lines;
      execv("./sort", argv);
      return 0;
   } else if (p1 > 0 && p2 == 0 && p3 > 0) {
      int p4 = fork();
      if (p4 == 0) {
         // 5.dat
         sprintf(str_lines, "%d", lines / num_ps);
         strcpy(argv[0],"5.dat");
         argv[1] = str_lines;
         execv("./sort", argv);
         return 0;
      } else {
         // 6.dat
         sprintf(str_lines, "%d", lines / num_ps);
         strcpy(argv[0],"6.dat");
         argv[1] = str_lines;
         execv("./sort", argv);
         return 0;
      }
   } else if (p1 > 0 && p2 > 0 && p3 == 0) {
      int p4 = fork();
      if (p4 == 0) {
         // 7.dat
         sprintf(str_lines, "%d", lines / num_ps);
         strcpy(argv[0],"7.dat");
         argv[1] = str_lines;
         execv("./sort", argv);
         return 0;
      } else {
         int p5 = fork();
         if (p5 == 0) {
            // 8.dat
            sprintf(str_lines, "%d", lines / num_ps);
            strcpy(argv[0],"8.dat");
            argv[1] = str_lines;
            execv("./sort", argv);
            return 0;
         } else {
            // 9.dat
            sprintf(str_lines, "%d", lines - 9 * lines / num_ps);
            strcpy(argv[0],"9.dat");
            argv[1] = str_lines;
            execv("./sort", argv);
            return 0;
         }
      }
   }
   while(wait(&pid) != -1);
	t = clock() - t;
	printf("Time elapsed to sort with %d ps: %f sec\n", num_ps, (double)t/CLOCKS_PER_SEC);
	merge("0.dat", lines / num_ps, "1.dat", lines / num_ps, "10.dat");
	merge("2.dat", lines / num_ps, "3.dat", lines / num_ps, "11.dat");
	merge("4.dat", lines / num_ps, "5.dat", lines / num_ps, "12.dat");
	merge("6.dat", lines / num_ps, "7.dat", lines / num_ps, "13.dat");
	merge("8.dat", lines / num_ps, "9.dat", lines - 9 * lines / num_ps, "14.dat");
	merge("10.dat", 2 * lines / num_ps, "11.dat", 2 * lines / num_ps, "15.dat");
	merge("12.dat", 2 * lines / num_ps, "13.dat", 2 * lines / num_ps, "16.dat");
	merge("15.dat", 4 * lines / num_ps, "16.dat", 4 * lines / num_ps, "17.dat");
	merge("14.dat", lines - 8 * lines / num_ps, "17.dat", lines / num_ps, "18.dat");
   system("mv ./*.dat ./ps10_data/");
}
