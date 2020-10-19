/* Samson Nguyen
 * 1001496565
 * CSE 3320 Assignment 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
   // system("rm *.dat");
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


   // Instrument 1 ps
   memcpy(data2, data, lines * sizeof(int));
   char filename[6];
   sprintf(filename, "%d.dat", 0);
   file = fopen(filename, "w+");
   // write numbers to file
   for (int i = 0; i < lines; i++) {
      fprintf(file, "%d\n", data[i]);
   }
   fclose(file);

   char str_lines[50];
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
   } else {
      wait(NULL);
   }
   t = clock() - t;
   printf("Time elapsed to sort with 1 ps: %f sec\n", (double)t/CLOCKS_PER_SEC);

   // Instrument 2 ps
   memcpy(data2, data, lines * sizeof(int));
   int num_ps = 2;
   for (int div = 0; div < num_ps; div++) {
      sprintf(filename, "%d.dat", div);
      file = fopen(filename, "w+");
      for (int i = div * lines / num_ps; i < (div + 1) * lines / num_ps; i++) {
         fprintf(file, "%d\n", data[i]);
      }
      fclose(file);
   }

   t = clock();
   pid = fork();
   if (pid == 0) {
      // child process
      int p1 = fork();
      if (p1 > 0) {
         // child-parent
         int p2 = fork();
         if (p2 == 0) {
            sprintf(str_lines, "%d", lines / 2);
            strcpy(argv[0],"0.dat");
            argv[1] = str_lines;
            execv("./sort", argv);
         }
      } else {
         // child-child
         sprintf(str_lines, "%d", lines - lines / 2);
         strcpy(argv[0],"1.dat");
         argv[1] = str_lines;
         execv("./sort", argv);
         return 0;
      }
      wait(NULL);
      strcpy(argv[0], "0.dat");
      sprintf(argv[1], "%d", lines / 2);
      strcpy(argv[2], "1.dat");
      sprintf(argv[3], "%d", lines - lines / 2);
      strcpy(argv[4], "2.dat");
      execv("./merge", argv);
      return 0;
   }
   t = clock() - t;
   // printf("%d %d", lines/2, lines - lines/2);
   printf("Time elapsed to sort with 2 ps: %f sec\n", (double)t/CLOCKS_PER_SEC);
}
