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
#include "data.h"

void distribute_data(int num_ps, t_data data[], int lines) {
	FILE *file;
	char filename[10];
   for (int div = 0; div < num_ps; div++) {
      sprintf(filename, "%d.dat", div);
      file = fopen(filename, "w+");
      for (int i = div * lines / num_ps; i < lines && i < (div + 1) * lines / num_ps; i++) {
         fprintf(file, "%s,%lf,%lf,%f,%f\n", data[i].time, data[i].latitude, data[i].longitude, data[i].depth, data[i].mag);
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
	t_data *nums1 = (t_data*)malloc(size1 * sizeof(t_data));
   t_data *nums2 = (t_data*)malloc(size2 * sizeof(t_data));
   t_data *dest = (t_data*)malloc((size1 + size2) * sizeof(t_data));
   FILE *file1 = fopen(f1, "r");
   FILE *file2 = fopen(f2, "r");
   FILE *file3;
	char line[256];

   // read numbers from files to arrays
	int i = 0, size0 = size1 + size2;
	for (i = 0; i < size1; i++) {
		fgets(line, 256, file1);
      char *tok = strtok(line, ",");
		strcpy(nums1[i].time, tok);
      tok = strtok(NULL, ",");
		nums1[i].latitude = atof(tok);
      tok = strtok(NULL, ",");
		nums1[i].longitude = atof(tok);
      tok = strtok(NULL, ",");
		nums1[i].depth = atof(tok);
      tok = strtok(NULL, ",");
		nums1[i].mag = atof(tok);
	}
   fclose(file1);
	for (i = 0; i < size2; i++) {
		fgets(line, 256, file2);
      char *tok = strtok(line, ",");
		strcpy(nums2[i].time, tok);
      tok = strtok(NULL, ",");
		nums2[i].latitude = atof(tok);
      tok = strtok(NULL, ",");
		nums2[i].longitude = atof(tok);
      tok = strtok(NULL, ",");
		nums2[i].depth = atof(tok);
      tok = strtok(NULL, ",");
		nums2[i].mag = atof(tok);
	}
   fclose(file2);

   // merge nums1 and nums2 into dest
   i = 0;
   if (nums1[size1 - 1].latitude <= nums2[0].latitude) {
      while (i < size1) {
         dest[i] = nums1[i++];
      }
      while (i < size0) {
         dest[i] = nums2[i++];
      }
   } else {
      int j = 0, k = 0;
      while (i < size0) {
         if (nums1[j].latitude <= nums2[k].latitude) {
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
	file3 = fopen(d, "w+");
   for (int i = 0; i < size0; i++) {
      fprintf(file3, "%s,%lf,%lf,%f,%f\n", dest[i].time, dest[i].latitude, dest[i].longitude, dest[i].depth, dest[i].mag);
   }
   fflush(file3);
   fclose(file3);
}

int main(void) {
   FILE *file = fopen("all_month.csv", "r");
   t_data *data;
   char line[256];
   int num_ps, lines = 0;
	time_t t;

   while (fgets(line, 256, file)) {
      lines++;
   }
   lines -= 1; // the first line is not data
   data = (t_data*)malloc(lines * sizeof(t_data));

   fclose(file);

   file = fopen("all_month.csv", "r");
   fgets(line, 256, file); // ignore first line
   for (int i = 0; i < lines; i++) {
		fgets(line, 256, file);
      char *tok = strtok(line, ",");
		strcpy(data[i].time, tok);
      tok = strtok(NULL, ",");
		data[i].latitude = atof(tok);
      tok = strtok(NULL, ",");
		data[i].longitude = atof(tok);
      tok = strtok(NULL, ",");
		data[i].depth = atof(tok);
      tok = strtok(NULL, ",");
		data[i].mag = atof(tok);
   }
   fclose(file);

   // Instrument 1 ps
	num_ps = 1;
   distribute_data(num_ps, data, lines);

   t = time(NULL);
   sort_data(num_ps, lines);
   t = time(NULL) - t;
   printf("Time elapsed to sort with 1 ps: %ld sec\n", t);

   system("mv ./0.dat ./1ps.csv");

   // Instrument 2 ps
	num_ps = 2;
   distribute_data(num_ps, data, lines);

   t = time(NULL);
   sort_data(num_ps, lines);
	t = time(NULL) - t;

	printf("Time elapsed to sort with %d ps: %ld sec\n", num_ps, t);

	merge("0.dat", lines / num_ps, "1.dat", lines - lines / num_ps, "2.dat");
   system("mv ./2.dat ./2ps.csv");

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
	system("mv ./6.dat ./4ps.csv");

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
   system("mv ./18.dat ./10ps.csv");
	system("rm *.dat");
}
