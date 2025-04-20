#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define STR_SEP " \t\n"

void cleanup_background_processes()
{

    // Check for and clean up terminated background processes
    pid_t pid = waitpid(-1, NULL, WNOHANG);
    while (pid > 0)
    {
        printf("%d\n", pid);
        fflush(stdout); // Flush stdout to ensure output appears immediately
        pid = waitpid(-1, NULL, WNOHANG);
    }
}

int main()
{
    char line[1024];
    char *args[100];
    while (1)
    {
        cleanup_background_processes(); // Check for completed background processes
        printf(">> ");
        fflush(stdout); // Flush stdout to ensure prompt appears immediately
        // Read input line
        if (fgets(line, 1024, stdin) == NULL)
            break;

        // Tokenize input
        args[0] = strtok(line, STR_SEP);
        int i = 0;
        while (args[i] != NULL)
        {
            ++i;
            args[i] = strtok(NULL, STR_SEP);
        }
        if (i == 0)
            continue; // empty line

        int is_background = 0; // false
        if (strcmp(args[i - 1], "&") == 0)
        {
            is_background = 1;
            args[i - 1] = NULL; // remove '&' from args
        }

        // Fork and execute
        pid = fork();
        if (pid < 0)
        {
            perror("fork error");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else
            waitpid(pid, NULL, 0);
    }

    return 0;
}