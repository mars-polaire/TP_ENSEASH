#include "utils.h"

int main(void) {
    // Buffer to store the input
    char buf[BUF_SIZE];
    
    display_welcome();
    
    // Infinite loop for shell prompt
    while (1) {
        write_str(PROMPT);
        
        // Read user input from standard input
        ssize_t r = read(STDIN_FILENO, buf, BUF_SIZE - 1);
        
        buf[r] = '\0';
        
        trim_newline(buf);
        
        // (just pressing Enter)
        if (buf[0] == '\0') {
            continue;
        }
        
        // Fork a new process to execute the command
        pid_t pid = fork();
        
        if (pid == 0) {
            // Child process: execute the command
            char *argv[] = {buf, NULL};
            execvp(buf, argv);
            
            // command not found, exit 
            _exit(127);
        } else if (pid > 0) {
            // Parent process: wait for child to complete
            wait(NULL);
        }
        // After command execution, return to prompt 
    }
    
    return 0;
}
