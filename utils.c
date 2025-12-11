#include "utils.h"

/* Writes data to standard output safely*/
ssize_t safe_write(const char *buf, size_t len) {
    size_t done = 0;
    while (done < len) {
        ssize_t w = write(STDOUT_FILENO, buf + done, len - done);
        if (w < 0) return w;
        done += (size_t)w;
    }
    return (ssize_t)done;
}

/* Writes a null-terminated string to standard output*/
void write_str(const char *s) {
    safe_write(s, strnlen(s, BUF_SIZE));
}

/* Removes trailing newline, carriage return, spaces and tabs*/
void trim_newline(char *s) {
    size_t len = strnlen(s, BUF_SIZE);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r' || s[len - 1] == ' ' || s[len - 1] == '\t')) {
        s[len - 1] = '\0';
        len--;
    }
}

/* Displays the shell welcome message*/
void display_welcome(void) {
    write_str(WELCOME_MSG);
}

/* Checks if a string is the "exit" command*/
int is_exit_cmd(const char *s) {
    return (strnlen(s, BUF_SIZE) == 4 && strncmp(s, "exit", 4) == 0);
}

/* Displays the shell exit message*/
void display_goodbye(void) {
    write_str("Goodbyeeeee.\n");
}

/* Converts a long integer to a string*/
void int_to_str(long v, char *out, size_t max) {
    if (max == 0) return;
    
    char tmp[32];
    size_t i = 0, j = 0;
    
    if (v == 0) {
        tmp[j++] = '0';
    } else {
        long n = v;
        while (n > 0 && j < sizeof(tmp)) {
            tmp[j++] = (char)('0' + (n % 10));
            n /= 10;
        }
    }
    
    while (j > 0 && i < max - 1) {
        out[i++] = tmp[--j];
    }
    out[i] = '\0';
}

/* Displays the shell prompt with status information*/
void display_prompt(int last_exit, int last_signal, int has_status) {
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
        write_str("] ");
    }
    
    write_str("% ");
}

/* Displays the shell prompt with status and execution time*/
void display_prompt_q5(int exit_status, int signal, int has_status, long exec_time_ms) {
    write_str("enseash");
    
    if (has_status) {
        write_str(" [");
        if (signal == 0) {
            write_str("exit:");
            write_int(exit_status);
        } else {
            write_str("sign:");
            write_int(signal);
        }
        write_str("|");
        write_int(exec_time_ms);
        write_str("ms]");
    }
    
    write_str(" % ");
}

/* Writes an integer to standard output without using printf*/
void write_int(int num) {
    char buffer[20];  
    int i = 0;
    
    if (num < 0) {
        write(STDOUT_FILENO, "-", 1);
        num = -num;
    }
    
    if (num == 0) {
        write(STDOUT_FILENO, "0", 1);
        return;
    }
    
    while (num > 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }
    
    while (i > 0) {
        write(STDOUT_FILENO, &buffer[--i], 1);
    }
}
