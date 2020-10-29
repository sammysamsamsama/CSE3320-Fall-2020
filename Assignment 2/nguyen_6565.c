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

// merge array in place
// first subarray is [start:mid]
// second subarray is [mid:end]
void merge(t_data *data, int start, int mid, int end) {
	// if it's already sorted, return
   if (data[mid - 1].latitude <= data[mid].latitude) {
      return;
   } else {
      int i, j, k;
		for (i = start, j = mid; i < mid && j < end; i++) {
			// if data[i] < data[j], leave it
         if (data[i].latitude <= data[j].latitude) {
			// else move data[i:j] right by 1 and data[i] = data[j]
			} else {
				t_data temp = data[j];
				for (k = j; k != i; k--) {
					data[k] = data[k - 1];
				}
				data[i] = temp;
				j++;
			}
      }
   }
}

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

	merge(data, 0, lines / num_threads, lines);

   // Instrument 4 threads
   num_threads = 4;
	memcpy(data, data_copy, lines * sizeof(t_data));

   t = time(NULL);
   sort_data(data, num_threads, lines);
	t = time(NULL) - t;
	printf("Time elapsed to sort with %d threads: %ld sec\n", num_threads, t);

	merge(data, 0, lines / num_threads, 2 * lines / num_threads);
	merge(data, 2 * lines / num_threads, 3 * lines / num_threads, lines);
	merge(data, 0, 2 * lines / num_threads, lines);

   // Instrument 10 threads
   num_threads = 10;
	memcpy(data, data_copy, lines * sizeof(t_data));

   t = time(NULL);
	sort_data(data, num_threads, lines);
	t = time(NULL) - t;
	printf("Time elapsed to sort with %d threads: %ld sec\n", num_threads, t);
	pthread_mutex_destroy(&lock);
	merge(data, 0, lines / num_threads, 2 * lines / num_threads);
	merge(data, 2 * lines / num_threads, 3 * lines / num_threads, 4 * lines / num_threads);
	merge(data, 4 * lines / num_threads, 5 * lines / num_threads, 6 * lines / num_threads);
	merge(data, 6 * lines / num_threads, 7 * lines / num_threads, 8 * lines / num_threads);
	merge(data, 8 * lines / num_threads, 9 * lines / num_threads, lines);
	merge(data, 0, 2 * lines / num_threads, 4 * lines / num_threads);
	merge(data, 4 * lines / num_threads, 6 * lines / num_threads, 8 * lines / num_threads);
	merge(data, 0, 4 * lines / num_threads, 8 * lines / num_threads);
	merge(data, 0, 8 * lines / num_threads, lines);
}
