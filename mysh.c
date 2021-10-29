// Contributors: Sebastian Almeida, Alex, Ryan Doherty
// COP 4600 Homework 2
// 10/27/2021

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

// Global to keep track of number of tokens
int numTokens = 0;

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

// Function To Split Raw Input Into Tokens On Spaces
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
  numTokens = position;
  return tokens;
}


int main()
{
  // Initialize Variables
  char *rawInput;
  char **parsedInput;
  char ***history;
  pid_list* pidList = NULL;
  int *argc;
  int byebye = 0;

  // Loop For Printing Prompt And Then Reading User Input
  while(byebye == 0)
  {
    // Print Prompt
    printf("# ");
    // Read In User Input
    rawInput = readInput();
    // Parse User Input
    parsedInput = tokenize(rawInput);
    // Ensure That A Command Was Passed
    if (parsedInput[0] == NULL)
    {
      free(rawInput);
      free(parsedInput);
      continue;
    }
    // If-Else Command Statement
    // ByeBye Command
    if (strcmp(parsedInput[0], "byebye") == 0)
    {
      byebye = 1;
    }
    // History Command
    else if (strcmp(parsedInput[0], "history") == 0)
    {
    }
    // Replay Command
    else if (strcmp(parsedInput[0], "replay") == 0)
    {
    }
    // Start Command
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
    // Background Command
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
    // Dalek Command
    else if (strcmp(parsedInput[0], "dalek") == 0)
    {
    }
    // Movetodir Command
    else if (strcmp(parsedInput[0], "movetodir") == 0)
    {
    }
    // Whereami Command
    else if (strcmp(parsedInput[0], "whereami") == 0)
    {
    }
    // Unrecognized Command
    else
    {
      printf("Unrecognized Command\n");
    }
    // Free Input Arrays
    free(rawInput);
    free(parsedInput);
  }
  return 0;
}
