#include "utils.h"
#include <string.h>  // For strcmp

int main(void) {
    char buf[BUF_SIZE];
    
    display_welcome();
    
    while (1) {
        write_str(PROMPT);
        
        ssize_t r = read(STDIN_FILENO, buf, BUF_SIZE - 1);
        if (r <= 0) {
            // Ctrl+D detected 
            display_goodbye();
            break;
        }
        
        buf[r] = '\0';
        trim_newline(buf);
        
        if (buf[0] == '\0') {
            continue;
        }
        
        // Check if it's the "exit" command
        if (strcmp(buf, "exit") == 0) {
            display_goodbye();
            break;
        }
        
        // Command execution
        pid_t pid = fork();
        if (pid == 0) {
            char *argv[] = {buf, NULL};
            execvp(buf, argv);
            _exit(127);
        } else if (pid > 0) {
            wait(NULL);
        }
    }
    
    return 0;
}
