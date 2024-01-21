#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

// Function to trim leading and trailing whitespaces from a string
char* trim(char* str) {
    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == '\0') {
        return str;
    }

    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }

    *(end + 1) = '\0';

    return str;
}

// Function to execute the Program Under Test (Java program)
void executePUT(char *putFile, char *testCaseFile) {
    int inPipe[2]; // Pipe for writing input to the PUT
    int outPipe[2]; // Pipe for reading output from the PUT

    if (pipe(inPipe) == -1 || pipe(outPipe) == -1) {
        perror("Pipe creation failed");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) {
        // Child process (PUT)
        close(inPipe[1]);
        close(outPipe[0]);

        // Redirect input and output to pipes
        dup2(inPipe[0], STDIN_FILENO);
        dup2(outPipe[1], STDOUT_FILENO);

        // Execute the Java program
        execl("/usr/bin/java", "java", putFile, NULL);

        // This line will only execute if the execlp fails
        perror("Execution of Program Under Test failed");
        exit(1); 
    } else {
        // Parent process (test tool)
        close(inPipe[0]);
        close(outPipe[1]);

        FILE *testCase = fopen(testCaseFile, "r");
        if (testCase == NULL) {
            perror("Failed to open test case file");
            exit(1);
        }

        char line[1000];
        char expected[1000];
        while (fgets(line, sizeof(line), testCase) != NULL) {
            if (line[0] == '#') {
                // Separator between test cases
                // Restart the PUT for the next test case
                close(inPipe[1]);
                close(outPipe[0]);
                if (pipe(inPipe) == -1 || pipe(outPipe) == -1) {
                    perror("Pipe creation failed");
                    exit(1);
                }
                if (pid == -1) {
                    perror("Fork failed");
                    exit(1);
                }
                if (pid == 0) {
                    // Child process (PUT)
                    close(inPipe[1]);
                    close(outPipe[0]);
                    dup2(inPipe[0], STDIN_FILENO);
                    dup2(outPipe[1], STDOUT_FILENO);
                    execl("/usr/bin/java", "java", putFile, NULL);
                    perror("Execution of Program Under Test failed");
                    exit(1);
                }
                close(inPipe[0]);
                close(outPipe[1]);
            } else if (line[0] != '*') {
                // Input line
                write(inPipe[1], line, strlen(line) - 1);
            } else {
                // Output line
                char output[1000];
                int bytesRead = read(outPipe[0], output, sizeof(output));
                if (bytesRead > 0) {
                    output[bytesRead] = '\0';
                    line[strlen(line) - 1] = '\0'; // Remove trailing newline
                    char* trimmedOutput = trim(output);
                    char* trimmedExpected = trim(line + 1);
                    if (strstr(trimmedOutput, trimmedExpected) != NULL){
                        printf("Test passed: %s\n", trimmedExpected);
                    } else {
                        printf("Test failed: %s\n", trimmedExpected);
                    }
                }
            }
        }

        fclose(testCase);
        wait(NULL); // Wait for the last child process to exit
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <Program Under Test> <Test Case File>\n", argv[0]);
        return 1;
    }

    char *putFile = argv[1];
    char *testCaseFile = argv[2];

    executePUT(putFile, testCaseFile);

    return 0;
}
