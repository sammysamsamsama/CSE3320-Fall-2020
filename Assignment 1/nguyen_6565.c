/* Samson Nguyen
 * 1001496565
 * CSE 3320 Assignment 1
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>

// linked list node for directory and file name management
struct Node {
   char filename[NAME_MAX];
   int size;
   int time;
   struct Node* next;
   struct Node* last;
};

// push new node in after last_node
void push(struct Node* last_node, char *new_filename, int size, int time) {
   // insert data if empty head node
   if (strlen(last_node->filename) == 0) {
      strcpy(last_node->filename, new_filename);
      last_node->size = size;
      last_node->time = time;
      return;
   }
   struct Node* new_node = malloc(sizeof(struct Node));
   strcpy(new_node->filename, new_filename);
   new_node->size = size;
   new_node->time = time;
   if (last_node->next != NULL) {
      last_node->next->last = new_node; // next node <- new node
   }
   new_node->next = last_node->next; // new node -> next node
   last_node->next = new_node; // last node -> new node
   new_node->last = last_node; // new node <- last node
}

int compare_alpha(const void* a, const void* b) {
   return strcmp(((const struct Node*)a)->filename, ((const struct Node*)b)->filename);
}

int compare_size(const void* a, const void* b) {
   int a_size = ((const struct Node*)a)->size;
   int b_size = ((const struct Node*)b)->size;
   if (a_size > b_size) {
      return 1;
   } else if (a_size < b_size) {
      return -1;
   } else {
      return 0;
   }
}

int compare_date(const void* a, const void* b) {
   int a_time = ((const struct Node*)a)->time;
   int b_time = ((const struct Node*)b)->time;
   if (a_time > b_time) {
      return 1;
   } else if (a_time < b_time) {
      return -1;
   } else {
      return 0;
   }
}

int main(void) {
   pid_t child;
   DIR * dir;  /* pointer to directory structure */
   struct dirent * dir_ent; /* pointer to directory entries */
   struct stat fileinfo; /* contains info like size and date */
   int i, c, k;  /* misc variables for looping */
   /* fixed length buffers? susceptible to buffer overflows */
   char cwd[2048], s[PATH_MAX], cmd[PATH_MAX], cmd2[PATH_MAX], input[4096];
   int sort_mode = 0; // 0 = alpha, 1 = size, 2 = date
   time_t t; /* time structure */
   int display_limit = 8; /* limit on files/directories displayed */

   while (1) {
      // system call for time in secs from the beginning of creation, 1/1/1970
      t = time( NULL );

      // get cwd into string s, max 200 chars?
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
         printf("\nCurrent Directory: %s \n", cwd); // display cwd
         printf("Time: %s\n", ctime( &t )); // display current time
      } else {
         perror("getcwd() failed");
         return -1;
      }

      // open cwd
      dir = opendir( cwd );
      if (dir == NULL) { // if failed to open then dir == NULL
         printf("opendir(\"%s\") failed:", cwd);
         perror(""); // print error and exit
         return -1;
      }

      // print directories
      // head and tail of linked list
      // tail will always be empty
      struct Node* head = calloc(1, sizeof(struct Node));
      struct Node* tail = calloc(1, sizeof(struct Node));
      head->next = tail;
      tail->last = head;
      c = 0; // counter variable
      printf("Directories:");
      // read all directories into linked list
      // sys call read from dir into dir_ent
      while ((dir_ent = readdir(dir))){
         // if d_type is a DT_DIR, add to list
         if ((dir_ent->d_type) & DT_DIR) {
            stat(dir_ent->d_name, &fileinfo);
            push(tail->last, dir_ent->d_name, fileinfo.st_size, fileinfo.st_atime);
            c++;
         }
      }

      // compress linked list to array
      struct Node directories[c]; // array of Nodes
      c = 0;
      struct Node* current_node = head;
      while (current_node != tail) {
         // allocate space for file name
         strcpy(directories[c].filename, current_node->filename);
         directories[c].size = current_node->size;
         directories[c].time = current_node->time;
         current_node = current_node->next;
         free(current_node->last);
         c++;
      }
      free(current_node);

      // sort filenames
      switch (sort_mode) {
         case 1:
            qsort(directories, c, sizeof(struct Node), compare_size);
            break;
         case 2:
            qsort(directories, c, sizeof(struct Node), compare_date);
            break;
         default:
            qsort(directories, c, sizeof(struct Node), compare_alpha);
      }

      // print the array of directories
      int first_tab_flag = 0;
      i = 0;
      while (i >= 0) {
         if (first_tab_flag == 0) {
            printf("\t%d. %s\n", i, directories[i].filename);
            first_tab_flag = 1;
         } else {
            if (i < c) {
               printf("\t\t%d. %s\n", i, directories[i].filename);
            }
            if ((i + 1) % display_limit == 0 || i >= c) {
               if (c < display_limit) {
                  break;
               }
               printf("Press 'N' for the next %d, ", display_limit);
               printf("'B' for the last %d, ", display_limit);
               printf("or 'Z' to continue.\n");
               printf("> ");
               fgets(input, sizeof(input), stdin);
               if (input[strlen(input) - 1] != '\n') {
                  char c;
                  while ((c = getchar()) != '\n' && c != EOF);
               }
               switch(tolower(input[0])) {
                  case 'n':
                     // print next 5
                     if (i < c - 1) {
                        break;
                     } else {
                        // go back or to 0
                        i = i < display_limit ? 0 : i - (i % display_limit) - 1;
                     }
                     break;
                  case 'b':
                     if (i < display_limit) {
                        // will be corrected by i++ and restart at i = 0
                        i = -1;
                     } else {
                        // subtract the difference from the last multiple of
                        // display_limit, then subtract display_limit + 1
                        i = i - (i % display_limit) - (display_limit + 1);
                     }
                     break;
                  case 'z':
                     // after i++, i = -1 < 0 so break out of while loop
                     i = -2;
                     break;
                  default:
                     printf("Command '%c' not recognized.\n", input[0]);
               }
            }
         }
         i++;
      }

      closedir( dir ); // could fail?
      printf( "\n" );

      // open . directory again
      // (rewind dir for reading)
      dir = opendir( "." );
      if (dir == NULL) {
         perror("opendir(\".\") failed");
         return -1;
      }

      // print files
      // head and tail of linked list
      // tail will always be empty
      head = calloc(1, sizeof(struct Node));
      tail = calloc(1, sizeof(struct Node));
      head->next = tail;
      tail->last = head;
      c = 0; // reset counter (of filenames read)
      printf("Files:");
      // read all file names into linked list
      while ((dir_ent = readdir(dir))){ // reread dir into dir_ent
         if (((dir_ent->d_type) & DT_REG)) { // if d_type is DT_REG (reg file)
            // add to filename linked list
            stat(dir_ent->d_name, &fileinfo);
            push(tail->last, dir_ent->d_name, fileinfo.st_size, fileinfo.st_atime);
            c++;
         }
      }

      // compress linked list to array
      struct Node files[c]; // array of Nodes
      c = 0;
      current_node = head;
      while (current_node != tail) {
         // allocate space for file name
         strcpy(files[c].filename, current_node->filename);
         files[c].size = current_node->size;
         files[c].time = current_node->time;
         current_node = current_node->next;
         free(current_node->last);
         c++;
      }
      free(current_node);

      // sort filenames
      switch (sort_mode) {
         case 1:
            qsort(files, c, sizeof(struct Node), compare_size);
            break;
         case 2:
            qsort(files, c, sizeof(struct Node), compare_date);
            break;
         default:
            qsort(files, c, sizeof(struct Node), compare_alpha);
      }

      // print the array of filenames
      i = 0;
      while (i >= 0) {
         if (i < c) {
            printf("\t\t%d. %s\n", i, files[i].filename);
         }
         if ((i + 1) % display_limit == 0 || i >= c) {
            if (c < display_limit) {
               break;
            }
            printf("Press 'N' for the next %d, ", display_limit);
            printf("'B' for the last %d, ", display_limit);
            printf("or 'Z' to continue.\n");
            printf("> ");
            fgets(input, sizeof(input), stdin);
            if (input[strlen(input) - 1] != '\n') {
               char c;
               while ((c = getchar()) != '\n' && c != EOF);
            }
            switch(tolower(input[0])) {
               case 'n':
                  // print next 5
                  if (i < c - 1) {
                     break;
                  } else {
                     // go back or to 0
                     i = i < display_limit ? 0 : i - (i % display_limit) - 1;
                  }
                  break;
               case 'b':
                  if (i < display_limit) {
                     // will be corrected by i++ and restart at i = 0
                     i = -1;
                  } else {
                     i = i - (i % display_limit) - (display_limit + 1);
                  }
                  break;
               case 'z':
                  i = -2;
                  break;
               default:
                  printf("Command '%c' not recognized.\n", input[0]);
            }
         }
         i++;
      }

      closedir( dir );
      printf( "\n" );

      // print operations
      printf("Operations:\tD Display\n");
      printf("\t\tE Edit\n");
      printf("\t\tR Run\n");
      printf("\t\tC Change Directory\n");
      printf("\t\tS Sort Directory listing\n");
      printf("\t\tM Move to Directory\n");
      printf("\t\tR Remove file\n");
      printf("\t\tQ Quit\n");
      printf("\n> ");

      char failure[4096];
      fgets(input, sizeof(input), stdin);
      if (input[strlen(input) - 1] != '\n') {
         char c;
         while ((c = getchar()) != '\n' && c != EOF);
      }
      c = input[0];
      // tolower so 'Q' == 'q'
      switch (tolower(c)) {
         case 'q': exit(0); /* quit */
         case 'd':
            while (1) {
               printf("Display what?\n");
               printf("> ");
               fgets(s, sizeof(s), stdin);
               if (s[strlen(s) - 1] != '\n') {
                  char c;
                  while ((c = getchar()) != '\n' && c != EOF);
                  printf("Invalid: input is too long.\n");
               } else {
                  break;
               }
            }
            strcpy(cmd, "cat ");
            strcat(cmd, s);
            cmd[strlen(cmd) - 1] = 0; // get rid of trailing \n
            if (system( cmd ) != 0) {
               strcpy(failure, "system(");
               strcat(failure, cmd);
               strcat(failure, ") failed");
               perror(failure);
               chdir(s);
            }
            break;
         case 'e':
            while (1) {
               printf( "Edit what?\n" );
               printf("> ");
               fgets(s, sizeof(s), stdin);
               if (s[strlen(s) - 1] != '\n') {
                  char c;
                  while ((c = getchar()) != '\n' && c != EOF);
                  printf("Invalid: input is too long.\n");
               } else {
                  break;
               }
            }
            strcpy( cmd, "pico "); // pico editor to cmd string
            strcat( cmd, s ); // e parameter to cmd string
            system( cmd ); // copies shell and runs cmd (bad fork & exec)
            break;
         case 'r':
            while (1) {
               printf("Run (0) or Remove (1)?\n");
               printf("> ");
               fgets(s, sizeof(s), stdin);
               if (s[strlen(s) - 1] != '\n') {
                  char c;
                  while ((c = getchar()) != '\n' && c != EOF);
                  printf("Invalid: input is too long.\n");
               } else {
                  break;
               }
            }
            c = s[0];
            switch (tolower(c)) {
               case '0':
                  while (1) {
                     printf( "Run what?\n" );
                     printf("> ");
                     fgets(cmd, sizeof(cmd), stdin);
                     if (cmd[strlen(cmd) - 1] != '\n') {
                        char c;
                        while ((c = getchar()) != '\n' && c != EOF);
                        printf("Invalid: input is too long.\n");
                     } else {
                        break;
                     }
                  }
                  cmd[strlen(cmd) - 1] = 0; // get rid of trailing \n
                  if (system( cmd ) != 0) {
                     strcpy(failure, "system(");
                     strcat(failure, cmd);
                     strcat(failure, ") failed: ");
                     perror(failure);
                     chdir(s);
                  }
                  break;
               case '1':
                  while (1) {
                     printf( "Remove what?\n" );
                     printf("> ");
                     fgets(cmd, sizeof(cmd), stdin);
                     if (cmd[strlen(cmd) - 1] != '\n') {
                        char c;
                        while ((c = getchar()) != '\n' && c != EOF);
                        printf("Invalid: input is too long.\n");
                     } else {
                        break;
                     }
                  }
                  getcwd(s, 200);
                  cmd[strlen(cmd) - 1] = 0; // get rid of trailing \n
                  // sys call change directory
                  if (remove( cmd ) != 0) {
                     strcpy(failure, "remove(");
                     strcat(failure, cmd);
                     strcat(failure, ") failed");
                     perror(failure);
                     chdir(s);
                  }
                  break;
               default:
                  printf("Command %c not recognized", input[0]);
            }
            break;
         case 'c':
            while (1) {
               printf( "Change to?\n" );
               printf("> ");
               fgets(cmd, sizeof(cmd), stdin);
               if (cmd[strlen(cmd) - 1] != '\n') {
                  char c;
                  while ((c = getchar()) != '\n' && c != EOF);
                  printf("Invalid: input is too long.\n");
               } else {
                  break;
               }
            }
            getcwd(s, 200);
            cmd[strlen(cmd) - 1] = 0; // get rid of trailing \n
            // sys call change directory
            if (chdir( cmd ) != 0) {
               strcpy(failure, "chdir(");
               strcat(failure, cmd);
               strcat(failure, ") failed");
               perror(failure);
               chdir(s);
            }
            break;
         case 's':
            while (1) {
               printf("Sort how?\n0 for alphabetical, 1 for size, 2 for date.\n");
               printf("> ");
               fgets(cmd, sizeof(cmd), stdin);
               if (cmd[strlen(cmd) - 1] != '\n') {
                  char c;
                  while ((c = getchar()) != '\n' && c != EOF);
                  printf("Invalid: input is too long.\n");
               } else {
                  break;
               }
            }
            switch (cmd[0]) {
               case '0':
                  sort_mode = 0;
                  break;
               case '1':
                  sort_mode = 1;
                  break;
               case '2':
                  sort_mode = 2;
                  break;
               default:
                  printf("Sort mode %c not recognized.\n", cmd[0]);
                  printf("Sorting alphabetically.\n");
            }
            break;
         case 'm':
            while (1) {
               printf("Move what?\n");
               printf("> ");
               fgets(cmd, sizeof(s), stdin);
               if (cmd[strlen(cmd) - 1] != '\n') {
                  char c;
                  while ((c = getchar()) != '\n' && c != EOF);
                  printf("Invalid: input is too long.\n");
               } else {
                  cmd[strlen(cmd) - 1] = 0; // get rid of trailing \n
                  break;
               }
            }
            while (1) {
               printf("To where?\n");
               printf("> ");
               fgets(cmd2, sizeof(s), stdin);
               if (cmd2[strlen(cmd2) - 1] != '\n') {
                  char c;
                  while ((c = getchar()) != '\n' && c != EOF);
                  printf("Invalid: input is too long.\n");
               } else {
                  cmd2[strlen(cmd2) - 1] = 0;
                  break;
               }
            }
            if (rename( cmd, cmd2 ) != 0) {
               strcpy(failure, "rename(");
               strcat(failure, cmd);
               strcat(failure, ", ");
               strcat(failure, cmd2);
               strcat(failure, ") failed");
               perror(failure);
               chdir(s);
            }
            break;
         default:
            printf("Command %c not recognized", input[0]);
      }
   }
}
