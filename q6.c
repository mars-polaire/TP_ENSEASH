#define _POSIX_C_SOURCE 200809L  /* AJOUTE CETTE LIGNE */

#include "utils.h"

void parse_line_with_redirects(char *line, char **argv, char **infile, char **outfile, int max_args) {
    int argc = 0;
    *infile = NULL;
    *outfile = NULL;
    
    char *p = line;
    
    // Skip leading spaces
    while (*p == ' ' || *p == '\t') p++;
    
    // Parse tokens
    while (*p && argc < max_args - 1) {
        // If we find a redirect
        if (*p == '<') {
            p++; // Skip '<'
            while (*p == ' ' || *p == '\t') p++; // Skip spaces
            *infile = p; // Set input file
            // Move to end of filename
            while (*p && *p != ' ' && *p != '\t' && *p != '>' && *p != '<') p++;
        } 
        else if (*p == '>') {
            p++; // Skip '>'
            while (*p == ' ' || *p == '\t') p++; // Skip spaces
            *outfile = p; // Set output file
            // Move to end of filename
            while (*p && *p != ' ' && *p != '\t' && *p != '>' && *p != '<') p++;
        }
        else {
            // Normal argument
            argv[argc++] = p;
            // Move to end of argument
            while (*p && *p != ' ' && *p != '\t' && *p != '>' && *p != '<') p++;
        }
        
        // If we're at a space, terminate current token and continue
        if (*p == ' ' || *p == '\t') {
            *p = '\0';
            p++;
            while (*p == ' ' || *p == '\t') p++; // Skip multiple spaces
        }
        // If we're at a redirect character, don't terminate, just continue loop
        else if (*p == '<' || *p == '>') {
            // Do nothing, let next iteration handle it
        }
        // End of string
        else if (*p == '\0') {
            // Terminate last token if needed
            break;
        }
    }
    
    argv[argc] = NULL;
    
    // Terminate the filenames if they exist
    if (*infile) {
        char *end = *infile;
        while (*end && *end != ' ' && *end != '\t' && *end != '\0') end++;
        *end = '\0';
    }
    if (*outfile) {
        char *end = *outfile;
        while (*end && *end != ' ' && *end != '\t' && *end != '\0') end++;
        *end = '\0';
    }
}

int main(void) {
    char buf[BUF_SIZE];
    char *argv[MAX_ARGS];
    char *infile = NULL, *outfile = NULL;
    int last_exit = 0, last_signal = 0, has_status = 0;
    long last_time_ms = 0;
    
    display_welcome();
    
    while (1) {
        // Prompt
        write_str("enseash ");
        if (has_status) {
            write_str("[");
            if (last_signal != 0) {
                write_str("sign:");
                char s[16];
                int_to_str(last_signal, s, sizeof(s));
                write_str(s);
            } else {
                write_str("exit:");
                char s[16];
                int_to_str(last_exit, s, sizeof(s));
                write_str(s);
            }
            write_str("|");
            char t[32];
            int_to_str(last_time_ms, t, sizeof(t));
            write_str(t);
            write_str("ms] ");
        }
        write_str("% ");
        
        // Read input
        ssize_t r = read(STDIN_FILENO, buf, BUF_SIZE - 1);
        if (r <= 0) {
            write_str("\n");
            display_goodbye();
            break;
        }
        
        buf[r] = '\0';
        trim_newline(buf);
        
        if (buf[0] == '\0') continue;
        if (is_exit_cmd(buf)) {
            display_goodbye();
            break;
        }
        
        // Reset pointers
        infile = NULL;
        outfile = NULL;
        
        // Parse with redirects
        parse_line_with_redirects(buf, argv, &infile, &outfile, MAX_ARGS);
        
        if (argv[0] == NULL) continue;
        
        struct timespec t1, t2;
        clock_gettime(CLOCK_MONOTONIC, &t1);
        
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            // Handle input redirection
            if (infile != NULL) {
                int fd = open(infile, O_RDONLY);
                if (fd < 0) {
                    write_str("Error: cannot open input file '");
                    write_str(infile);
                    write_str("'\n");
                    _exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            
            // Handle output redirection
            if (outfile != NULL) {
                int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) {
                    write_str("Error: cannot open output file '");
                    write_str(outfile);
                    write_str("'\n");
                    _exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            
            // Execute command
            execvp(argv[0], argv);
            
            // If exec fails
            write_str("Error executing: ");
            write_str(argv[0]);
            write_str("\n");
            _exit(127);
        } 
        else if (pid > 0) {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
            clock_gettime(CLOCK_MONOTONIC, &t2);
            
            // Calculate time
            long sec = t2.tv_sec - t1.tv_sec;
            long nsec = t2.tv_nsec - t1.tv_nsec;
            if (nsec < 0) {
                nsec += 1000000000L;
                sec--;
            }
            last_time_ms = sec * 1000 + nsec / 1000000;
            
            // Update status
            has_status = 1;
            if (WIFEXITED(status)) {
                last_exit = WEXITSTATUS(status);
                last_signal = 0;
            } else if (WIFSIGNALED(status)) {
                last_signal = WTERMSIG(status);
                last_exit = 0;
            }
        }
        else {
            write_str("fork failed\n");
        }
    }
    
    return 0;
}
