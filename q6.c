#define _POSIX_C_SOURCE 199309L  // Enable POSIX features including clock_gettime

#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>    // clock_gettime


int main(void) {
    char buf[BUF_SIZE];

    int last_exit = 0;
    int last_signal = 0;
    int has_status = 0;
    long exec_time_ms = 0;

    display_welcome();

    while (1) {
        display_prompt_q5(last_exit, last_signal, has_status, exec_time_ms);

        ssize_t r = read(STDIN_FILENO, buf, BUF_SIZE - 1);
        if (r <= 0) {
            // Ctrl+D or read error 
            display_goodbye();
            break;
        }

        buf[r] = '\0';
        trim_newline(buf);

        // empty line -> continue 
        if (buf[0] == '\0') {
            continue;
        }

        // exit command 
        if (strcmp(buf, "exit") == 0) {
            display_goodbye();
            break;
        }

        // Tokenize input into argv array 
        char *argv_exec[BUF_SIZE / 2 + 1]; // rough max number of args 
        int argc = 0;
        char *saveptr = NULL;
        char *tok = strtok_r(buf, " \t", &saveptr);
        while (tok != NULL && argc < (int)(BUF_SIZE / 2)) {
            argv_exec[argc++] = tok;
            tok = strtok_r(NULL, " \t", &saveptr);
        }
        argv_exec[argc] = NULL;

        if (argc == 0) {
            continue; 
        }

        pid_t pid = fork();
        if (pid < 0) {
            continue;
        }

        if (pid == 0) {
            // CHILD PROCESS 

            // internal command: segfault 
            if (strcmp(argv_exec[0], "segfault") == 0) {
                int *p = NULL;
                *p = 42; // trigger SIGSEGV 
                _exit(0); // unreachable 
            }

            // Execute command with arguments
            execvp(argv_exec[0], argv_exec);

            // execvp failed -> exit with 127 (convention for command not found) 
            _exit(127);
        } else {
            // PARENT PROCESS 
            struct timespec start_time, end_time;
            clock_gettime(CLOCK_MONOTONIC, &start_time);

            int status;
            waitpid(pid, &status, 0);

            clock_gettime(CLOCK_MONOTONIC, &end_time);

            has_status = 1;

            // compute execution time in ms 
            exec_time_ms = (end_time.tv_sec - start_time.tv_sec) * 1000;
            exec_time_ms += (end_time.tv_nsec - start_time.tv_nsec) / 1000000;

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



