#pragma once

#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#define MAX_CLIENTS 20

typedef struct {
  uint32_t self_id;
  uint32_t peer_id;
} client_data_t;

typedef struct {
  uint16_t src_port;
  struct sockaddr_in dst_addr;
} peer_data_t;

typedef struct {
  bool active;
  struct sockaddr_in addr;
  socklen_t len;
  time_t time;
  client_data_t data;
} client_t;

typedef struct {
  size_t size;
  client_t client[MAX_CLIENTS]; 
} clients_t;
