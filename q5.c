#define _POSIX_C_SOURCE 199309L  // Enable POSIX features including clock_gettime 

#include "utils.h"     
#include <string.h>    
#include <stdlib.h>    
#include <time.h>  // Time functions for clock_gettime 

int main(void) {
  
    char buf[BUF_SIZE];
    
    int last_exit = 0;     
    int last_signal = 0;  
    int has_status = 0;   
    long exec_time_ms = 0;  // Execution time of last command in milliseconds 
    
    display_welcome();
    
    while (1) {

        display_prompt_q5(last_exit, last_signal, has_status, exec_time_ms);
        
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
            /* CHILD PROCESS */
            
             // Simple internal command: segfault
            if (strcmp(buf, "segfault") == 0) {
                int *p = NULL;
                *p = 42;  // SIGSEGV (signal 11)
                return 0; 
            }
            
            char *argv[] = {buf, NULL};
            
            execvp(buf, argv);
            
            _exit(127);  
            
        } else if (pid > 0) {
            /* PARENT PROCESS */
            
            struct timespec start_time, end_time;
            
            // Record start time before waiting for child 
            clock_gettime(CLOCK_MONOTONIC, &start_time);
            
            int status;
            waitpid(pid, &status, 0);
            
            // Record end time after child finishes
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            
            has_status = 1;
            
            // Calculate execution time in milliseconds 
            exec_time_ms = (end_time.tv_sec - start_time.tv_sec) * 1000;
            exec_time_ms += (end_time.tv_nsec - start_time.tv_nsec) / 1000000;
            
            // Determine how the child terminated 
            if (WIFEXITED(status)) {
                // Child terminated normally with an exit code 
                last_exit = WEXITSTATUS(status);
                last_signal = 0;
            } else if (WIFSIGNALED(status)) {
                // Child was terminated by a signal 
                last_signal = WTERMSIG(status);
                last_exit = 0;
            }
        }
    }
    
    return 0; 
}
