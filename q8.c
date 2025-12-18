#define _POSIX_C_SOURCE 199309L

#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    char buf[BUF_SIZE];               

    int last_exit = 0;                
    int last_signal = 0;              
    int has_status = 0;               
    long exec_time_ms = 0;            

    display_welcome();               

    while (1) {
        // show the prompt (shell name + status + time)
        display_prompt_q5(last_exit, last_signal, has_status, exec_time_ms);

        // read one line from stdin
        ssize_t r = read(STDIN_FILENO, buf, BUF_SIZE - 1);
        if (r <= 0) {
            // (Ctrl+D) or read error
            display_goodbye();
            break;
        }

        buf[r] = '\0';             
        trim_newline(buf);           // remove trailing newline/carriage return/spaces/tabs

        if (buf[0] == '\0')          // empty line -> show prompt again
            continue;

        if (strcmp(buf, "exit") == 0) { // internal "exit" command -> quit
            display_goodbye();
            break;
        }

        /* --- Tokenization: split the line into words separated by space/tab --- */
        char *argv[BUF_SIZE / 2 + 1]; // array of argument pointers 
        int argc = 0;
        char *saveptr = NULL;
        char *tok = strtok_r(buf, " \t", &saveptr); // first token
        while (tok != NULL && argc < (BUF_SIZE / 2)) {
            argv[argc++] = tok;                // store pointer to token in argv
            tok = strtok_r(NULL, " \t", &saveptr); // next token
        }
        argv[argc] = NULL;                     // execvp expects a NULL-terminated argv

        /* --- Search for a pipe '|' token among the arguments --- */
        int pipe_pos = -1;
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "|") == 0) {   // found a pipe
                pipe_pos = i;
                break;
            }
        }

        /* --- No pipe: simple single-command execution --- */
        if (pipe_pos == -1) {
            pid_t pid = fork();
            if (pid == 0) {
                // child: replace image with requested command
                execvp(argv[0], argv);
                // if execvp returns, it failed -> use convention exit code 127
                _exit(127);
            }
            // parent: measure time and wait for child to finish
            int status;
            struct timespec t1, t2;
            clock_gettime(CLOCK_MONOTONIC, &t1); // start time
            waitpid(pid, &status, 0);            // wait for child
            clock_gettime(CLOCK_MONOTONIC, &t2); // end time

            // compute elapsed time in milliseconds
            exec_time_ms =
                (t2.tv_sec - t1.tv_sec) * 1000 +
                (t2.tv_nsec - t1.tv_nsec) / 1000000;

            has_status = 1;                       // we now have a status to display
            if (WIFEXITED(status)) {              // child exited normally
                last_exit = WEXITSTATUS(status);
                last_signal = 0;
            } else {                              // child was terminated by a signal
                last_signal = WTERMSIG(status);
                last_exit = 0;
            }
            continue; // go back to prompt
        }

        /* --- Pipe present: build two command arrays cmd1 | cmd2 --- */
        argv[pipe_pos] = NULL;                    // split argv into two NULL-terminated arrays
        char **cmd1 = argv;                       // left command (before pipe)
        char **cmd2 = &argv[pipe_pos + 1];        // right command (after pipe)

        int fd[2];
        pipe(fd);  // create a pipe: fd[0] = read end, fd[1] = write end

        // measure total time for the pipeline
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        // fork first child: producer (left command)
        pid_t p1 = fork();
        if (p1 == 0) {
            // Child 1: redirect stdout to pipe write end
            dup2(fd[1], STDOUT_FILENO);   // duplicate fd[1] onto stdout
            close(fd[0]);                 // close unused read end
            close(fd[1]);                 // close original write end (dup2 duplicated it)
            execvp(cmd1[0], cmd1);        // execute left command
            _exit(127);                   // exec failed
        }

        // fork second child: consumer (right command)
        pid_t p2 = fork();
        if (p2 == 0) {
            // Child 2: redirect stdin from pipe read end
            dup2(fd[0], STDIN_FILENO);    // duplicate fd[0] onto stdin
            close(fd[1]);                 // close unused write end
            close(fd[0]);                 // close original read end
            execvp(cmd2[0], cmd2);        // execute right command
            _exit(127);                   // exec failed
        }

        /* --- Parent process: close pipe ends and wait for both children --- */
        close(fd[0]);
        close(fd[1]);

        int status2;
        waitpid(p1, NULL, 0);              // wait for left command (ignore its status)
        waitpid(p2, &status2, 0);          // wait for right command and get its status

        clock_gettime(CLOCK_MONOTONIC, &end); // end time

        // compute pipeline execution time in ms
        exec_time_ms =
            (end.tv_sec - start.tv_sec) * 1000 +
            (end.tv_nsec - start.tv_nsec) / 1000000;

        has_status = 1;
        if (WIFEXITED(status2)) {          // base status on right-hand command (common convention)
            last_exit = WEXITSTATUS(status2);
            last_signal = 0;
        } else {                           // right-hand command was terminated by a signal
            last_signal = WTERMSIG(status2);
            last_exit = 0;
        }
    }

    return 0;
}

