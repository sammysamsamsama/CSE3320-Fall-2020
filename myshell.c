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

int main(void) {
   pid_t child;
   DIR * dir;  /* pointer to directory structure */
   struct dirent * dir_ent; /* pointer to directory entries */
   int i, c, k;  /* misc variables for looping */
   char s[256], cmd[256];  /* fixed length buffers? susceptible to buffer overflows */
   char input[256];
   time_t t; /* time structure */

   while (1) {
      // system call for time in secs from the beginning of creation, 1/1/1970
      t = time( NULL );

      // get cwd into string s, max 200 chars?
      // TODO: Check for getcwd errors
      getcwd(s, 200);
      printf( "\nCurrent Directory: %s \n", s); // display cwd
      printf( "Time: %s\n", ctime( &t )); // display current time

      // open cwd
      dir = opendir( s );
      if (dir == NULL) { // if failed to open then dir == NULL
         printf("opendir(\"%s\") failed:",s);
         perror(""); // print error and exit
         return -1;
      }

      // print directories
      c = 0; // counter variable
      printf("Directories:");
      // sys call read from dir into dir_ent
      int first_tab_flag = 0;
      while ((dir_ent = readdir(dir))){
         // if d_type is a DT_DIR, print the directory
         if ((dir_ent->d_type) & DT_DIR) {
            // print every directory name
            if (first_tab_flag == 1) {
               printf( "\t\t%d. %s\n", c++, dir_ent->d_name);
            } else {
               printf( "\t%d. %s\n", c++, dir_ent->d_name);
               first_tab_flag = 1;
            }
         }
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
      c = 0; // reset counter
      printf("Files:");
      while ((dir_ent = readdir(dir))){ // reread dir into dir_ent
         if (((dir_ent->d_type) & DT_REG)) { // if d_type is DT_REG (reg file)
            printf( "\t\t%d. %s\n", c++, dir_ent->d_name);
         }
         if ( c > 0 && ( c % 5 ) == 0 ) { // print only 5 filenames at a time
            printf( "Hit N for Next\n" );
            k = getchar( ); getchar( );
         }
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
            strcpy(failure, "Command '");
            strcat(failure, &input[0]);
            strcat(failure, "' not recognized.");
            printf("%s", failure);
            break;
      }
   }
}
