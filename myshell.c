/* Samson Nguyen
 * 1001496565
 * CSE 3320 Assignment 1
 */
/*  Some example code and prototype -
contains many, many problems: should check for return values
(especially system calls), handle errors, not use fixed paths,
handle parameters, put comments, watch out for buffer overflows,
security problems, use environment variables, etc.
*/

#include <sys/types.h>
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
   char filename[PATH_MAX];
   struct Node* next;
   struct Node* last;
};

// push new node in after last_node
void push(struct Node* last_node, char *new_filename) {
   // insert data if empty head node
   if (strlen(last_node->filename) == 0) {
      strcpy(last_node->filename, new_filename);
      return;
   }
   struct Node* new_node = malloc(sizeof(struct Node));
   strcpy(new_node->filename, new_filename);
   if (last_node->next != NULL) {
      last_node->next->last = new_node; // next node <- new node
   }
   new_node->next = last_node->next; // new node -> next node
   last_node->next = new_node; // last node -> new node
   new_node->last = last_node; // new node <- last node
}

int main(void) {
   pid_t child;
   DIR * dir;  /* pointer to directory structure */
   struct dirent * dir_ent; /* pointer to directory entries */
   int i, c, k;  /* misc variables for looping */
   /* fixed length buffers? susceptible to buffer overflows */
   char cwd[PATH_MAX], s[256], cmd[256], input[256];
   time_t t; /* time structure */

   while (1) {
      // system call for time in secs from the beginning of creation, 1/1/1970
      t = time( NULL );

      // get cwd into string s, max 200 chars?
      // TODO: Check for getcwd errors
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
            push(tail->last, dir_ent->d_name);
            c++;
         }
      }

      // compress linked list to array
      char** filenames = calloc(c, sizeof(char*)); // array of pointers to strings
      c = 0;
      struct Node* current_node = head;
      while (current_node != tail) {
         // allocate space for file name
         filenames[c] = calloc(strlen(current_node->filename), sizeof(char));
         strcpy(filenames[c], current_node->filename);
         c++;
         current_node = current_node->next;
         free(current_node->last);
      }
      free(current_node);

      // print the array of directories
      int first_tab_flag = 0;
      i = 0;
      while (i >= 0) {
         if (first_tab_flag == 0) {
            printf("\t%d. %s\n", i, filenames[i]);
            first_tab_flag = 1;
         } else {
            if (i < c) {
               printf("\t\t%d. %s\n", i, filenames[i]);
            }
            if ((i + 1) % 5 == 0 || i >= c) {
               if (c < 5) {
                  break;
               }
               printf("Press 'N' for the next 5, 'B' for the last 5, or 'Z' to continue.\n");
               printf("> ");
               fgets(input, 256, stdin);
               switch(tolower(input[0])) {
                  case 'n':
                     // print next 5
                     if (i < c - 1) {
                        break;
                     } else {
                        // go back or to 0
                        i = i < 5 ? 0 : i - (i % 5) - 1;
                     }
                     break;
                  case 'b':
                     if (i < 5) {
                        i = -1;
                     } else {
                        i = i - (i % 5) - 6;
                     }
                     break;
                  case 'z':
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
            push(tail->last, dir_ent->d_name);
            // printf("\t\t%d. %s\n", c, tail->last->filename);
            c++;
         }
      }

      // compress linked list to array
      filenames = realloc(filenames, c * sizeof(char*));; // array of pointers to strings
      c = 0;
      current_node = head;
      while (current_node != tail) {
         // allocate space for file name
         filenames[c] = calloc(strlen(current_node->filename), sizeof(char));
         strcpy(filenames[c], current_node->filename);
         c++;
         current_node = current_node->next;
         free(current_node->last);
      }
      free(current_node);

      // print the array of filenames
      i = 0;
      while (i >= 0) {
         if (i < c) {
            printf("\t\t%d. %s\n", i, filenames[i]);
         }
         if ((i + 1) % 5 == 0 || i >= c) {
            if (c < 5) {
               break;
            }
            printf("Press 'N' for the next 5, 'B' for the last 5, or 'Z' to continue.\n");
            printf("> ");
            fgets(input, 256, stdin);
            switch(tolower(input[0])) {
               case 'n':
                  // print next 5
                  if (i < c - 1) {
                     break;
                  } else {
                     // go back or to 0
                     i = i < 5 ? 0 : i - (i % 5) - 1;
                  }
                  break;
               case 'b':
                  if (i < 5) {
                     i = -1;
                  } else {
                     i = i - (i % 5) - 6;
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

      char failure[256];
      fgets(input, 256, stdin);
      c = input[0];
      // tolower so 'Q' == 'q'
      switch (tolower(c)) {
         case 'q': exit(0); /* quit */
         case 'e': printf( "Edit what?: " );
            scanf( "%s", s );
            strcpy( cmd, "pico "); // pico editor to cmd string
            strcat( cmd, s ); // e parameter to cmd string
            system( cmd ); // copies shell and runs cmd (bad fork & exec)
            break;
         case 'r': printf( "Run what?: " );
            fgets(cmd, 256, stdin);
            system( cmd );
            break;
         case 'c': printf( "Change To?: " );
            getcwd(s, 200);
            fgets(cmd, 256, stdin);
            cmd[strlen(cmd) - 1] = 0; // get rid of trailing \n
            if (chdir( cmd ) != 0) {
               strcpy(failure, "chdir(");
               strcat(failure, cmd);
               strcat(failure, ") failed");
               perror(failure);
               chdir(s);
            } // sys call change cwd // what if dir doesn't exist? permission?
            break;
         default:
            printf("Command %c not recognized", input[0]);
            break;
      }
   }
}
