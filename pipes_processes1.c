// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

int main() 
{ 
    // We use two pipes 
    // First pipe to send input string from parent 
    // Second pipe to send concatenated string from child 
  
    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 
  
    char fixed_str[] = "howard.edu";
    char input_str[100]; 
    char concat_str[100];
    pid_t p; 
  
    if (pipe(fd1)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    if (pipe(fd2)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
  
    printf("Enter a string to concatenate: ");
    scanf("%s", input_str); 
    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed" ); 
        return 1; 
    } 
  
    // Parent process 
    else if (p > 0) 
    { 
        close(fd1[0]);  // Close reading end of pipes 
        close(fd2[1]);

        // Write input string and close writing end of first 
        // pipe. 
        write(fd1[1], input_str, strlen(input_str)+1); 
      
        // Wait for child to send a string 
        wait(NULL);

        //NEW: Receive second input string from child process
        char second_input[100];
        read(fd2[0], second_input, sizeof(second_input));
  
        // Concatenate the second inputted string with the first string
        strcat(input_str, fixed_str);
        strcat(input_str, second_input);

        // Print the final result
        printf("Output: %s\n", input_str); 

        close(fd2[0]); // Close writing end of pipe 1 and reading end of pipe 2 
        close(fd1[1]); 
    } 
  
    // child process 
    else
    { 
        close(fd1[1]);  // Close writing end of first pipes 
        close(fd2[0]); 
      
        // Read a string using first pipe 
        char second_input[100]; 
        read(fd1[0], concat_str, 100); 
  
        // Concatenate the fixed string with the input string
        strcat(concat_str, fixed_str);
  
        printf("Concatenated string: %s\n", concat_str);

        //Prompt user for second string
        printf("Enter a string to concatenate: ");
        scanf("%s", second_input);

        // NEW: Send input string to parent process using second process
        write(fd2[1], second_input, strlen(second_input) + 1);

        // Close both reading ends 
        close(fd1[0]); 
        close(fd2[1]); 

        exit(0); 
    } 
} 