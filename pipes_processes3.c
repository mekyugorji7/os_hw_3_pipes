#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <pattern>\n", argv[0]);
        return 1;
    }

    int pipefd1[2]; // Pipe between cat and grep
    int pipefd2[2]; // Pipe between grep and sort
    pid_t pid1, pid2;

    // Create the first pipe
    if (pipe(pipefd1) == -1) {
        perror("pipe1 failed");
        exit(1);
    }

    // Fork the first child to run "cat scores"
    if ((pid1 = fork()) == -1) {
        perror("fork1 failed");
        exit(1);
    }

    if (pid1 == 0) {
        // First child process: execute "cat scores"
        
        // Redirect stdout to pipe1's write end
        dup2(pipefd1[1], STDOUT_FILENO);
        
        // Close unused pipe ends
        close(pipefd1[0]);
        close(pipefd1[1]);

        // Execute "cat scores"
        execlp("cat", "cat", "scores", NULL);
        perror("execlp cat failed");
        exit(1);
    }

    // Create the second pipe
    if (pipe(pipefd2) == -1) {
        perror("pipe2 failed");
        exit(1);
    }

    // Fork the second child to run "grep <pattern>"
    if ((pid2 = fork()) == -1) {
        perror("fork2 failed");
        exit(1);
    }

    if (pid2 == 0) {
        // Second child process: execute "grep <pattern>"

        // Redirect stdin to pipe1's read end
        dup2(pipefd1[0], STDIN_FILENO);
        // Redirect stdout to pipe2's write end
        dup2(pipefd2[1], STDOUT_FILENO);
        
        // Close unused pipe ends
        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);

        // Execute "grep <pattern>"
        execlp("grep", "grep", argv[1], NULL);
        perror("execlp grep failed");
        exit(1);
    }

    // Parent process: execute "sort"
    
    // Close unused pipe ends
    close(pipefd1[0]);
    close(pipefd1[1]);
    
    // Redirect stdin to pipe2's read end
    dup2(pipefd2[0], STDIN_FILENO);
    
    // Close unused end of the second pipe
    close(pipefd2[1]);

    // Execute "sort"
    execlp("sort", "sort", NULL);
    perror("execlp sort failed");
    exit(1);
}
