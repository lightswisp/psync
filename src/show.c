#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>

void logger(const char* fmt, ...){
  va_list arglist;
#define BUF_LEN 26
  char time_buffer[BUF_LEN];
  time_t cur_time = time(NULL);
  struct tm* tm_info;
  tm_info = localtime(&cur_time);
  strftime(time_buffer, BUF_LEN, "%Y-%m-%d %H:%M:%S", tm_info);

  printf("[%s] ", time_buffer);
  va_start(arglist, fmt);
  vprintf(fmt, arglist);
  va_end(arglist);
}

void show_help(const char *progname) {
  printf("rendez-vous server\n\n"
         "usage: %s [-4v] [-p port] [-P password]\n",
         progname);
  exit(0);
}

void show_config(int server_family, uint16_t port, bool verbose, char* password){
  password = strlen(password) == 0 ? "(empty)" : password;
  const char *top = "---------------------------------";
  uint8_t padding = 10; 
  printf("selected options: \n"
         "+%.*s+\n"
         "| server_family | %*d |\n"
         "| port          | %*u |\n"
         "| verbose       | %*u |\n"
         "| password      | %*s |\n"
         "+%.*s+\n",
         padding+18, top, padding, server_family, padding, port, padding, verbose, padding,
         password, padding+18, top);
}
