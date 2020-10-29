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
#include <pthread.h>
#include "data.h"

pthread_mutex_t lock;
typedef struct insertion_sort_input {
	t_data **nums;
	int start;
	int end;
	int size;
} insertion_sort_input;

void *insertion_sort(void *input) {
	t_data *nums = *(((insertion_sort_input*)input)->nums);
	int start = ((insertion_sort_input*)input)->start;
	int end = ((insertion_sort_input*)input)->end;
	int size = ((insertion_sort_input*)input)->size;
	int i, j;
	t_data num;
	for (i = start; i < end && i < size; i++) {
		pthread_mutex_lock(&lock);
      num = nums[i];
      for (j = i - 1; j >= 0 && j >= start && nums[j].latitude > num.latitude; j--) {
         nums[j + 1] = nums[j];
      }
      nums[j + 1] = num;
		pthread_mutex_unlock(&lock);
   }
	return NULL;
}

int sort_data(t_data *data, int num_threads, int lines) {
	pthread_t *thread_ids = (pthread_t*)malloc(lines * sizeof(pthread_t));
	int start, end;
	for (int i = 0; i < num_threads; i++) {
		if (i < num_threads - 1) {
			start = i * lines / num_threads;
			end = (i + 1) * lines / num_threads;
		} else {
			start = i * lines / num_threads;
			end = lines;
		}
		insertion_sort_input input = {&data, start, end, lines};
		pthread_create(&(thread_ids[i]), NULL, insertion_sort, (void *)&input);
	}
	for (int i = 0; i < num_threads; i++) {
		pthread_join(thread_ids[i], NULL);
	}
	// sleep(1);
	return 0;
}

// void merge(const char *f1, int size1, const char *f2, int size2, const char *d) {
// 	t_data *nums1 = (t_data*)malloc(size1 * sizeof(t_data));
//    t_data *nums2 = (t_data*)malloc(size2 * sizeof(t_data));
//    t_data *dest = (t_data*)malloc((size1 + size2) * sizeof(t_data));
//    FILE *file1 = fopen(f1, "r");
//    FILE *file2 = fopen(f2, "r");
//    FILE *file3;
// 	char line[256];
//
//    // read numbers from files to arrays
// 	int i = 0, size0 = size1 + size2;
// 	for (i = 0; i < size1; i++) {
// 		fgets(line, 256, file1);
//       char *tok = strtok(line, ",");
// 		strcpy(nums1[i].time, tok);
//       tok = strtok(NULL, ",");
// 		nums1[i].latitude = atof(tok);
//       tok = strtok(NULL, ",");
// 		nums1[i].longitude = atof(tok);
//       tok = strtok(NULL, ",");
// 		nums1[i].depth = atof(tok);
//       tok = strtok(NULL, ",");
// 		nums1[i].mag = atof(tok);
// 	}
//    fclose(file1);
// 	for (i = 0; i < size2; i++) {
// 		fgets(line, 256, file2);
//       char *tok = strtok(line, ",");
// 		strcpy(nums2[i].time, tok);
//       tok = strtok(NULL, ",");
// 		nums2[i].latitude = atof(tok);
//       tok = strtok(NULL, ",");
// 		nums2[i].longitude = atof(tok);
//       tok = strtok(NULL, ",");
// 		nums2[i].depth = atof(tok);
//       tok = strtok(NULL, ",");
// 		nums2[i].mag = atof(tok);
// 	}
//    fclose(file2);
//
//    // merge nums1 and nums2 into dest
//    i = 0;
//    if (nums1[size1 - 1].latitude <= nums2[0].latitude) {
//       while (i < size1) {
//          dest[i] = nums1[i++];
//       }
//       while (i < size0) {
//          dest[i] = nums2[i++];
//       }
//    } else {
//       int j = 0, k = 0;
//       while (i < size0) {
//          if (nums1[j].latitude <= nums2[k].latitude) {
//             dest[i++] = nums1[j++];
//          } else {
//             dest[i++] = nums2[k++];
//          }
//          if (j >= size1) {
//             while (i < size0) {
//                dest[i++] = nums2[k++];
//             }
// 				break;
//          } else if (k >= size2) {
//             while (i < size0) {
//                dest[i++] = nums1[j++];
//             }
// 				break;
//          }
//       }
//    }
//
//    // write numbers to file
// 	file3 = fopen(d, "w+");
//    for (int i = 0; i < size0; i++) {
//       fprintf(file3, "%s,%lf,%lf,%f,%f\n", dest[i].time, dest[i].latitude, dest[i].longitude, dest[i].depth, dest[i].mag);
//    }
//    fflush(file3);
//    fclose(file3);
// }

int main(void) {
   FILE *file = fopen("all_month.csv", "r");
   t_data *data, *data_copy;
   char line[256];
   int num_threads, lines = 0;
	time_t t;
	if (pthread_mutex_init(&lock, NULL) != 0) {
		printf("mutex init failed\n");
		return 1;
	}

   while (fgets(line, 256, file)) {
      lines++;
   }
   lines -= 1; // the first line is not data
   data = (t_data*)malloc(lines * sizeof(t_data));
   data_copy = (t_data*)malloc(lines * sizeof(t_data));

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
	memcpy(data_copy, data, lines * sizeof(t_data));

   // Instrument 1 thread
	num_threads = 1;

   t = time(NULL);
   sort_data(data, num_threads, lines);
   t = time(NULL) - t;
   printf("Time elapsed to sort with 1 thread: %ld sec\n", t);

   // Instrument 2 threads
	num_threads = 2;
	memcpy(data, data_copy, lines * sizeof(t_data));

   t = time(NULL);
   sort_data(data, num_threads, lines);
	t = time(NULL) - t;
	printf("Time elapsed to sort with %d threads: %ld sec\n", num_threads, t);

	// merge("0.dat", lines / num_threads, "1.dat", lines - lines / num_threads, "2.dat");

   // Instrument 4 threads
   num_threads = 4;
	memcpy(data, data_copy, lines * sizeof(t_data));

   t = time(NULL);
   sort_data(data, num_threads, lines);
	t = time(NULL) - t;
	printf("Time elapsed to sort with %d threads: %ld sec\n", num_threads, t);

	// merge("0.dat", lines / num_threads, "1.dat", lines / num_threads, "4.dat");
	// merge("2.dat", lines / num_threads, "3.dat", lines - (3 * lines / num_threads), "5.dat");
	// merge("4.dat", 2 * (lines / num_threads), "5.dat", lines - 2 * (lines / num_threads), "6.dat");

   // Instrument 10 threads
   num_threads = 10;
	memcpy(data, data_copy, lines * sizeof(t_data));

   t = time(NULL);
	sort_data(data, num_threads, lines);
	t = time(NULL) - t;
	printf("Time elapsed to sort with %d threads: %ld sec\n", num_threads, t);
	pthread_mutex_destroy(&lock);
	// merge("0.dat", lines / num_threads, "1.dat", lines / num_threads, "10.dat");
	// merge("2.dat", lines / num_threads, "3.dat", lines / num_threads, "11.dat");
	// merge("4.dat", lines / num_threads, "5.dat", lines / num_threads, "12.dat");
	// merge("6.dat", lines / num_threads, "7.dat", lines / num_threads, "13.dat");
	// merge("8.dat", lines / num_threads, "9.dat", lines - (9 * lines / num_threads), "14.dat");
	// merge("10.dat", 2 * (lines / num_threads), "11.dat", 2 * (lines / num_threads), "15.dat");
	// merge("12.dat", 2 * (lines / num_threads), "13.dat", 2 * (lines / num_threads), "16.dat");
	// merge("15.dat", 4 * (lines / num_threads), "16.dat", 4 * (lines / num_threads), "17.dat");
	// merge("14.dat", lines - (9 * lines / num_threads) + lines / num_threads, "17.dat", 8 * (lines / num_threads), "18.dat");
}
