#pragma once
#include <stdint.h>
#include <stdbool.h>

extern bool verbose;
void logger(const char* fmt, ...);
void show_help(const char *progname); 
void show_config(int server_family, uint16_t port, bool verbose, char* password);
