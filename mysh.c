// Contributors: Sebastian Almeida, Alex, Ryan, Nia
// COP 4600 Homework 2
// 10/27/2021

#define _GNU_SOURCE
#define _POSIX_SOURCE
#define PATH_MAX 32768
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

// PID Linked List
typedef struct node {
    pid_t pid;
    struct node* next;
} pid_list;

// Function Prototypes
char* readInput();
char** tokenize();
char*** addToHistory();

int numTokens = 0;
void moveToDir(char *directory)
{
  /* The code here in the comments will change directories but only once
     I promise I tried my best.
  // check for directory
  if (parsedInput[1] == NULL)
  {
    printf("**Error: no directory stated.\n**Correct input format: # movetodir <directory>\n");
  }
  else if (currentDir == NULL)
  {
    printf("**Error: unable to open directory.\n");
  }
  else
  {
    //if ((temp = opendir(parsedInput[1])) != NULL)
    if ((filepath = realpath(parsedInput[1], NULL)) != NULL)
    {
      // close current directories
      //closedir(temp);
      closedir(currentDir);
      // update filepath and current directory
      //filepath = realpath(parsedInput[1], NULL);
      currentDir = opendir(filepath);
      printf("Moved to directory: %s\n", parsedInput[1]);
    }
    else
    {
      printf("**Error: directory not found.\n");
    }
  }
  */
  char currdir[PATH_MAX];
  if (directory == NULL)
  {
    printf("**Error: no directory stated.\n**Correct input format: # movetodir <directory>\n");
  }
  else
  {
    strcpy(currdir,realpath(directory, NULL));
    if (chdir(directory) != 0)
    {
      printf("**Error: directory not found.\n");
      return;
    }
    printf("Moved to directory: %s\n", directory);
  }
}
// gets current directory || Estefania Sanchez
void whereAmI()
{
  char *filepath;
  char currdir[PATH_MAX];
  filepath = getcwd(currdir, sizeof(currdir));
  printf("Current Directory: %s\n", filepath);
}
// Function To Read Users Input As A Raw String
char *readInput(void)
{
  int bufsize = 1024;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;
  // Check If Malloc Succeded
  if (!buffer)
  {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }
  // Loop Through User Input
  while (1) {
    // Read a character
    c = getchar();
    // If we hit EOF, replace it with a null character and return.
    if (c == '\n')
    {
      buffer[position] = '\0';
      return buffer;
    }
    else
    {
      buffer[position] = c;
    }
    position++;
    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize)
    {
      bufsize += 1024;
      buffer = realloc(buffer, bufsize);
      // Check If Realloc Succeeded
      if (!buffer)
      {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

// Function To Split Raw Input Into Tokens On Spaces - Sebastian Almeida
char **tokenize(char *line)
{
  int bufsize = 128;
  int position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;
  // Check If Malloc Succeeded
  if (!tokens)
  {
    exit(EXIT_FAILURE);
  }
  token = strtok(line, " ");
  while (token != NULL)
  {
    tokens[position] = token;
    position++;
    if (position >= bufsize)
    {
      bufsize += 128;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      // Check If Realloc Succeeded
      if (!tokens)
      {
        exit(EXIT_FAILURE);
      }
    }
    token = strtok(NULL, " ");
  }
  tokens[position] = NULL;
  return tokens;
}


int main()
{
  // Initialize Variables
  char *rawInput;
  char **parsedInput;
  pid_list* pidList = NULL;
  int bufsize = 1024;
  int replay = 0;
  int replayIndex;
  char **history = malloc(bufsize * sizeof(char*));
  int i;
  int byebye = 0;
  int commandCounter = 0;
  // Loop For Printing Prompt And Then Reading User Input
  while(byebye == 0)
  {
    // Check If We Are Replaying
    if (replay == 1)
    {
      replay = 0;
      rawInput = history[replayIndex];
      parsedInput = tokenize(history[replayIndex]);
      printf("%s\n", history[replayIndex]);
      history[commandCounter] = history[replayIndex];
      printf("%s\n", history[commandCounter]);
    }
    else
    {
      // Print Prompt
      printf("# ");
      // Read In User Input
      rawInput = readInput();
      // Parse User Input
      parsedInput = tokenize(rawInput);
    }
    // Ensure That A Command Was Passed
    if (parsedInput[0] == NULL)
    {
      free(rawInput);
      free(parsedInput);
      continue;
    }
    else
    {
      history[commandCounter] = rawInput;
      commandCounter += 1;
    }
    // If-Else Command Statemnt // Sebastian Almeida
    // ByeBye Command
    if (strcmp(parsedInput[0], "byebye") == 0)
    {
      byebye = 1;
    }
    // History Command
    else if (strcmp(parsedInput[0], "history") == 0)
    {
      // Print History
      if (parsedInput[1] == NULL)
      {
        for (i = commandCounter - 1; i >= 0; i--)
        {
          printf("%d: %s\n", (commandCounter - i - 1), history[i]);
        }
      }
      // Clear History
      else if ((strcmp(parsedInput[1], "-c") == 0))
      {
        for (i = 0; i < commandCounter; i++)
        {
          free(history[i]);
          commandCounter = 0;
        }
      }
      // Unrecognized Command
      else
      {
        printf("Unrecognized Clear Flag\n");
      }
    }
    // Replay Command This Will Not Show In History - Sebastian Almeida
    else if (strcmp(parsedInput[0], "replay") == 0)
    {
      if (parsedInput[1] == NULL)
      {
        replay = 1;
        commandCounter -= 1;
        free(rawInput);
        replayIndex = commandCounter - 1;
      }
      else if (atoi(parsedInput[1]) > commandCounter - 2)
      {
        printf("Please Provide A Command Number In Range For Replay\n");
        commandCounter -= 1;
        free(rawInput);
      }
      else
      {
        replay = 1;
        commandCounter--;
        free(rawInput);
        replayIndex = commandCounter - atoi(parsedInput[1]) - 1;
      }
    }
    // Start Command - Ryan Doherty
    else if (strcmp(parsedInput[0], "start") == 0)
    {
      pid_t process = fork();
      if (process == -1){
          printf("Error Forking");
      } else if (process == 0) {
          // Running on child process
          // Exec takes a null terminated list of arguments
          char** args = malloc(numTokens * sizeof(char*));
          for (int i = 0; i < numTokens - 1; i++){
              args[i] = parsedInput[i + 1];
          }
          args[numTokens - 1] = NULL;
          int execError = execv(parsedInput[1], args);
          if (execError == -1){
              printf("Program doesn't exist\n");
          }
      } else {
          // if on parent process, wait for child to finish executing
          waitpid(process, NULL, 0);
      }
    }
    // Background Command - Ryan Doherty
    else if (strcmp(parsedInput[0], "background") == 0)
    {
      pid_t process = fork();
       if (process == -1){
           printf("Error Forking");
       } else if (process == 0) {
           // Running on child process
           // Exec takes a null terminated list of arguments
           char** args = malloc(numTokens * sizeof(char*));
           for (int i = 0; i < numTokens - 1; i++){
               args[i] = parsedInput[i + 1];
           }
           args[numTokens - 1] = NULL;
           int execError = execv(parsedInput[1], args);
           if (execError == -1){
               printf("Program doesn't exist\n");
               continue;
           }
       } else {
           // if on parent process, add PID to list
           printf("PID: %d\n", process);
           pid_list* new_node = malloc(sizeof(pid_list*));
           new_node->pid = process;
           new_node->next = NULL;
           if(pidList == NULL){
               pidList = new_node;
           } else {
               pid_list* temp_node = pidList;
               while (temp_node->next != NULL){
                   temp_node = temp_node->next;
               }
               temp_node->next = new_node;
           }
       }
    }
 // Dalek Command - alex koehler

    else if (strcmp(parsedInput[0], "dalek") == 0)
    {
      if(parsedInput[1] == NULL){
           printf("Please enter a PID to exterminate.\n");
           continue;
       }
       else{
           char* helper;
           pid_t victim;

           victim = strtol(parsedInput[1], &helper, 10);

           if (kill(victim, SIGKILL) == 0){
               printf("%d has been exterminated.\n", victim);
           }
           else{
               printf("%d was too strong. Extermination failed.\n", victim);
           }
       }
    }

    // DalekAll Command - alex koehler

    else if (strcmp(parsedInput[0], "dalekAll") == 0){
        if(pidList == NULL){
            printf("No programs to exterminate.\n");
        }
        else{
            pid_list * curr = pidList;

            while(curr != NULL){
                pid_t victim = curr->pid;
                if(kill(victim, 0) == 0){ // this for some reason isn't checking pro
                kill(victim, SIGKILL);
                printf("Exterminating %d.\n", victim);
                }
                curr = curr->next;
            }
        }
    }
      
    // Movetodir Command
    else if (strcmp(parsedInput[0], "movetodir") == 0)
    {
      memmove(parsedInput[1], parsedInput[1]+1, strlen(parsedInput[1]));
      moveToDir(parsedInput[1]);
    }
    // Whereami Command
    else if (strcmp(parsedInput[0], "whereami") == 0)
    {
      whereAmI();
    }
    // Unrecognized Command
    else
    {
      printf("Unrecognized Command\n");
    }
    // Free Input Arrays

    free(parsedInput);
  }
  // Make Sure No Memory Leaks Also Print To File Here
  return 0;
}
