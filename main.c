#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dirent.h>
#include <windows.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define TRUE (1==1)
#define FALSE (1==0)
#define BUFFERMAX 1024
#define COLORSIZE 8

typedef enum {BLACK, BLUE, GREEN, CYAN, RED, MAGNETA,
     BROWN, LIGHTGRAY, WHITE = 15} MyColors;

/* THE FOLLOWING TWO FUNCTIONS "isArgs" AND "checkDirectoryExist" IS IN CASE
THIS PROGRAM IS RAN FROM THE COMMAND LINE IN UNIX */
//See if any arguments were supplied
int isArgs(int theArg){
  if(theArg == 1){
    printf("No argument was supplied\n");
    return FALSE;
  }
  return TRUE;
}

//check if the argument happens to be argv[1]
int checkDirectoryExist(int theArg){
  if(theArg < 2){
    printf("You need to say which directory\n");
    printf("Exiting... \n");
    return FALSE;
  }
  return TRUE;
}

/* function to set text color */
void color_text(MyColors color){
  if(color < 1 || color > LIGHTGRAY){
     color = WHITE;
  }
  HANDLE  hConsole;
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, color);
}

/* determines the file or directory with the greatest
   string length.  Used later for formatting reasons */
int getLargestFileLength(struct dirent *dr, DIR *d){
  char dName[BUFFERMAX];
  int longest = strlen(dr->d_name), current = 0;
  while((dr = readdir(d)) != NULL){
     current = strlen(dr->d_name);
     if(current > longest){
        longest = current;
     }
  }
  closedir(d);
  return longest;
}

void displayResultsFromFile(FILE *filePtr, const char* fileName){
   FILE *fp;
   fp = fopen(fileName, "r");
   if(fp == NULL){
      perror("Error, reading from file");
      exit(1);
   }

   char c;
   c = fgetc(filePtr);
   while(c != EOF){
      printf("%c", c);
      c = fgetc(fp);
   }

   fclose(fp);
}

int main (int argc, char **argv){
  char dirName[BUFFERMAX];
  int trueSuppliedDirectorySize = 0;
  DIR *directory;
  FILE *fptr;
  struct dirent *dirEntry;
  struct stat fileStats[BUFFERMAX];
  int fileCount = 0, dirCount = 0, normalFiles = 0, byteSize = 0;
  char timeAccessed[BUFFERMAX];
  char spacer = ' ';
  char* fileInDirName;

  //I need this unfortenetly if I want command line use to work
  setvbuf(stdout, NULL, _IONBF, 0);

  if(isArgs(argc) == FALSE){
    printf("Enter directory\n");
    scanf("%s", dirName);
    trueSuppliedDirectorySize = strlen(dirName);
  }else{
    strcpy(dirName, argv[1]);
  }

  directory = opendir(dirName);

  if(directory == NULL){
    printf("cannot open directory\nexiting now..\n");
    exit(1);
  }

  int greatestFileLen = getLargestFileLength(dirEntry, directory);
  directory = opendir(dirName);

  struct tm *tm;
  time_t myTime;

  char* path = malloc(strlen(dirName) + 1 + BUFFERMAX);
  strcpy(path, dirName);
  path[strlen(dirName)] = '/';
  int i, x;

  fptr = fopen("report2.txt", "w");
  if(fptr == NULL){
    perror("File could not be created");
    exit(1);
  }

  printf("\nIf you are on a Windows Machine, Cyan represents directories, Green represents files\n\n");

  printf("%-*s %10s                %-s\n",
         greatestFileLen+5, "File Name", "Modified Time", "File Size");
  fprintf(fptr, "%-*s %10s                %-s\n",
          greatestFileLen+5, "File Name", "Modified Time", "File Size");


  dirEntry = readdir(directory);
  while((dirEntry = readdir(directory)) != NULL){
    i = 0, x = 0;
    strcpy(path + strlen(dirName)+1, dirEntry->d_name);
    if(stat(path, &fileStats[i]) == -1){
        perror("Error: ");
        continue;
    }
    if(strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0){ continue;}
    (S_ISDIR(fileStats[i].st_mode)) ? dirCount++ : normalFiles++;
    myTime = fileStats[i].st_mtime;
    tm = localtime(&myTime);

    strftime(timeAccessed, sizeof(timeAccessed), "%b %d %Y %H:%M:%S", tm);
    fileInDirName = dirEntry->d_name;
    byteSize = fileStats[i].st_size;
    //if 0 means it is a directory, so don't print the zeros.
    if(S_ISDIR(fileStats[i].st_mode)){
        color_text(CYAN);
    }else{
        color_text(GREEN);
    }

    if(byteSize == 0){
       fprintf(fptr, "%-*s %10s          %c\n",
               greatestFileLen+5, fileInDirName, timeAccessed, byteSize, spacer);
       printf("%-*s %10s          %c\n",
              greatestFileLen+5, fileInDirName, timeAccessed, byteSize, spacer);
    }else{
       fprintf(fptr, "%-*s %10s          %-lu\n",
               greatestFileLen+5, fileInDirName, timeAccessed, byteSize);
       printf("%-*s %10s          %-lu\n",
              greatestFileLen+5, fileInDirName, timeAccessed, byteSize);
    }
    i++;
  }

  closedir(directory);
  color_text(LIGHTGRAY);
  printf("\ntotal directories: %d\n", dirCount);
  printf("total regular files: %d\n", normalFiles);

  fprintf(fptr, "\ntotal directories: %d\n", dirCount);
  fprintf(fptr, "total regular files: %d\n", normalFiles);
  fclose(fptr);

  //displayResultsFromFile(fptr, "report2.txt");
  fclose(fptr);
  free(path);

  char input;
  printf("\nPress enter to exit ");
  input = getch();
  if(input == '\n'){
        printf("\nexiting program...\n");
        exit(0);
  }

  return 0;
}

