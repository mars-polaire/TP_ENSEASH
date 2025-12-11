#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

/* ---------- Constants ---------- */
#define BUF_SIZE 1024
#define PROMPT "RS % "
#define WELCOME_MSG "Welcome to the RN Shell.\nTo quit, type 'exit'.\n"
/* ---------- Utility Functions ---------- */
ssize_t safe_write(const char *buf, size_t len);
void write_str(const char *s);
void trim_newline(char *s);
void display_welcome(void);
int is_exit_cmd(const char *s);
void display_goodbye(void);
void int_to_str(long v, char *out, size_t max);
void display_prompt_q4(int last_exit, int last_signal, int has_status);
void display_prompt_q5(int exit_status, int signal, int has_status, long exec_time_ms);
void write_int(int num);
#endif
