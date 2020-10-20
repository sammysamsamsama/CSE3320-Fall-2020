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

int main(void) {
   system("rm *.dat");
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
   }
   wait(NULL);
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
   pid = fork();
   if (pid == 0) {
      strcpy(argv[0], "0.dat");
      sprintf(argv[1], "%d", lines / num_ps);
      strcpy(argv[2], "1.dat");
      sprintf(argv[3], "%d", lines - lines / num_ps);
      strcpy(argv[4], "2.dat");
      execv("./merge", argv);
      return 0;
   }
   wait(NULL);
   t = clock() - t;
   printf("Time elapsed to sort with %d ps: %f sec\n", num_ps, (double)t/CLOCKS_PER_SEC);
   system("mv ./*.dat ./data/");
   // system("rm *.dat");

   // Instrument 4 ps
   memcpy(data2, data, lines * sizeof(int));
   num_ps = 4;
   for (int div = 0; div < num_ps; div++) {
      sprintf(filename, "%d.dat", div);
      file = fopen(filename, "w+");
      for (int i = div * lines / num_ps; i < (div + 1) * lines / num_ps; i++) {
         fprintf(file, "%d\n", data[i]);
      }
      fclose(file);
   }

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
         sprintf(str_lines, "%d", lines / num_ps);
         strcpy(argv[0],"2.dat");
         argv[1] = str_lines;
         execv("./sort", argv);
         return 0;
      }
      sprintf(str_lines, "%d", lines - 3 * lines / num_ps);
      strcpy(argv[0],"3.dat");
      argv[1] = str_lines;
      execv("./sort", argv);
      return 0;
   }
   // merge quarters into halves
   wait(NULL);
   pid = fork();
   if (pid == 0) {
      int p1 = fork();
      if (p1 == 0) {
         strcpy(argv[0], "0.dat");
         sprintf(argv[1], "%d", lines / num_ps);
         strcpy(argv[2], "1.dat");
         sprintf(argv[3], "%d", lines / num_ps);
         strcpy(argv[4], "4.dat");
         execv("./merge", argv);
         return 0;
      } else {
         strcpy(argv[0], "2.dat");
         sprintf(argv[1], "%d", lines / num_ps);
         strcpy(argv[2], "3.dat");
         sprintf(argv[3], "%d", lines - 3 * lines / num_ps);
         strcpy(argv[4], "5.dat");
         execv("./merge", argv);
         return 0;
      }
   }
   wait(NULL);
   // merge halves
   pid = fork();
   if (pid == 0) {
      strcpy(argv[0], "4.dat");
      sprintf(argv[1], "%d", lines / 2);
      strcpy(argv[2], "5.dat");
      sprintf(argv[3], "%d", lines - lines / 2);
      strcpy(argv[4], "6.dat");
      execv("./merge", argv);
      return 0;
   }
   wait(NULL);
   t = clock() - t;
   printf("Time elapsed to sort with %d ps: %f sec\n", num_ps, (double)t/CLOCKS_PER_SEC);
}
