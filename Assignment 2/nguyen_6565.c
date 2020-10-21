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

typedef struct t_data {
	char time[25];
	double latitude;
	double longitude;
	float depth;
	float mag;
} t_data;

void distribute_data(int num_ps, double data[], int lines) {
	FILE *file;
	char filename[10];
   for (int div = 0; div < num_ps; div++) {
      sprintf(filename, "%d.dat", div);
      file = fopen(filename, "w+");
      for (int i = div * lines / num_ps; i < lines && i < (div + 1) * lines / num_ps; i++) {
         fprintf(file, "%f\n", data[i]);
      }
      fclose(file);
   }
}

int sort_data(int num_ps, int lines) {
	char filename[10];
	char str_lines[10];
	char *argv[3] = {filename, str_lines, NULL};
	for (int i = 0; i < num_ps; i++) {
		int p1 = fork();
		if (p1 == 0) {
			if (i < num_ps - 1) {
				sprintf(argv[1], "%d", lines / num_ps);
		      sprintf(argv[0], "%d.dat", i);
		      execv("./sort", argv);
		      return 0;
			} else {
				sprintf(argv[1], "%d", lines - (num_ps - 1) * lines / num_ps);
            sprintf(argv[0], "%d.dat", i);
            execv("./sort", argv);
            return 0;
			}
		}
	}
	while(wait(NULL) != -1);
	return 0;
}

void merge(const char *f1, int size1, const char *f2, int size2, const char *d) {
	double *nums1 = (double*)malloc(size1 * sizeof(double));
   double *nums2 = (double*)malloc(size2 * sizeof(double));
   double *dest = (double*)malloc((size1 + size2) * sizeof(double));
   FILE *file1 = fopen(f1, "r");
   FILE *file2 = fopen(f2, "r");
   FILE *file3 = fopen(d, "w+");
	char line[256];
	double temp = 0;

   // read numbers from files to arrays
	int i = 0, size0 = size1 + size2;
	for (i = 0; i <= size1; i++) {
		fscanf(file1, "%lf", &(nums1[i]));
	}
   fclose(file1);
	for (i = 0; i <= size2; i++) {
		fscanf(file2, "%lf", &(nums2[i]));
	}
   fclose(file2);

   // merge nums1 and nums2 into dest
   i = 0;
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
            while (i < size0) {
               dest[i++] = nums2[k++];
            }
				break;
         } else if (k >= size2) {
            while (i < size0) {
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
   int num_ps, lines = 0;
	time_t t;

   while (fgets(line, 256, file)) {
      lines++;
   }
   lines -= 1; // the first line is not data
   data = (double*)malloc(lines * sizeof(double));

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
	num_ps = 1;
   distribute_data(num_ps, data, lines);

	char filename[10];
   char str_lines[10];
	strcpy(filename, "0.dat");
   sprintf(str_lines, "%d", lines);
   char *argv[6] = {filename, str_lines, NULL, NULL, NULL, NULL};
   argv[2] = (char*)malloc(50);
   argv[3] = (char*)malloc(50);
   argv[4] = (char*)malloc(50);

   t = time(NULL);
   sort_data(num_ps, lines);
   t = time(NULL) - t;
   printf("Time elapsed to sort with 1 ps: %ld sec\n", t);
   system("mv ./*.dat ./ps1_data/");

   // Instrument 2 ps
	num_ps = 2;
   distribute_data(num_ps, data, lines);

   t = time(NULL);
   sort_data(num_ps, lines);
	t = time(NULL) - t;
	printf("Time elapsed to sort with %d ps: %ld sec\n", num_ps, t);
	merge("0.dat", lines / num_ps, "1.dat", lines - lines / num_ps, "2.dat");
   system("mv ./*.dat ./ps2_data/");

   // Instrument 4 ps
   num_ps = 4;
   distribute_data(num_ps, data, lines);

   t = time(NULL);
   sort_data(num_ps, lines);
	t = time(NULL) - t;
	printf("Time elapsed to sort with %d ps: %ld sec\n", num_ps, t);
	merge("0.dat", lines / num_ps, "1.dat", lines / num_ps, "4.dat");
	merge("2.dat", lines / num_ps, "3.dat", lines - (3 * lines / num_ps), "5.dat");
	merge("4.dat", 2 * (lines / num_ps), "5.dat", lines - 2 * (lines / num_ps), "6.dat");
	system("mv ./*.dat ./ps4_data/");

   // Instrument 10 ps
   num_ps = 10;
   distribute_data(num_ps, data, lines);

   t = time(NULL);
	sort_data(num_ps, lines);
	t = time(NULL) - t;
	printf("Time elapsed to sort with %d ps: %ld sec\n", num_ps, t);
	merge("0.dat", lines / num_ps, "1.dat", lines / num_ps, "10.dat");
	merge("2.dat", lines / num_ps, "3.dat", lines / num_ps, "11.dat");
	merge("4.dat", lines / num_ps, "5.dat", lines / num_ps, "12.dat");
	merge("6.dat", lines / num_ps, "7.dat", lines / num_ps, "13.dat");
	merge("8.dat", lines / num_ps, "9.dat", lines - (9 * lines / num_ps), "14.dat");
	merge("10.dat", 2 * (lines / num_ps), "11.dat", 2 * (lines / num_ps), "15.dat");
	merge("12.dat", 2 * (lines / num_ps), "13.dat", 2 * (lines / num_ps), "16.dat");
	merge("15.dat", 4 * (lines / num_ps), "16.dat", 4 * (lines / num_ps), "17.dat");
	merge("14.dat", lines - (9 * lines / num_ps) + lines / num_ps, "17.dat", 8 * (lines / num_ps), "18.dat");
   system("mv ./*.dat ./ps10_data/");
}
