#include "utils.h"
#include <string.h>
#include <stdlib.h>

int main(void) {
    char buf[BUF_SIZE];
    int last_exit = 0;
    int last_signal = 0;
    int has_status = 0;
    
    display_welcome();
    
    while (1) {
        display_prompt_q4(last_exit, last_signal, has_status);
        
        ssize_t r = read(STDIN_FILENO, buf, BUF_SIZE - 1);
        if (r <= 0) {
            display_goodbye();
            break;
        }
        
        buf[r] = '\0';
        trim_newline(buf);
        
        if (buf[0] == '\0') {
            continue;
        }
        
        if (strcmp(buf, "exit") == 0) {
            display_goodbye();
            break;
        }
        
        pid_t pid = fork();
        if (pid == 0) {
            // Simple internal command: segfault
            if (strcmp(buf, "segfault") == 0) {
                int *p = NULL;
                *p = 42;  // SIGSEGV (signal 11)
                return 0;
            }
            
            // Normal commands
            char *argv[] = {buf, NULL};
            execvp(buf, argv);
            
            // If execvp fails
            _exit(127);
        } else if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
            
            has_status = 1;
            
            if (WIFEXITED(status)) {
                last_exit = WEXITSTATUS(status);
                last_signal = 0;
            } else if (WIFSIGNALED(status)) {
                last_signal = WTERMSIG(status);
                last_exit = 0;
            }
        }
    }
    
    return 0;
}
