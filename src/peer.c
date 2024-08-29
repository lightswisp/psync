#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <arpa/inet.h>
#include <peer.h>
#include <show.h>

bool peer_is_uniq_client(clients_t *clients, struct in_addr *ip) {
  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    if (memcmp(&clients->client[i].addr.sin_addr, ip, sizeof(struct in_addr)) == 0) {
      if (verbose)
        logger("client already exists. continue the loop\n");
      return false;
    }
  }
  if (verbose)
    logger("client is uniq, add him!\n");
  return true;
}

bool peer_get_free_slot(clients_t *clients, size_t *slot) {
  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    if (!clients->client[i].active) {
      *slot = i;
      return true;
    }
  }
  return false;
}

void peer_timer_cleanup(clients_t *clients) {
  if (verbose)
    logger("timer_cleanup check\n");
#define TIMER_LIMIT 10.0 // 10 seconds
  time_t now = time(NULL);
  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    if (clients->client[i].active &&
        floor(now - clients->client[i].time) > TIMER_LIMIT) {
      if (verbose)
        logger("removing client %ld\n", i);
      bzero(&clients->client[i], sizeof(client_t));
      clients->size--;
    }
  }
}

client_t *peer_exists(clients_t *clients, uint32_t peer_id) {
  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    if (clients->client[i].active &&
        clients->client[i].data.self_id == peer_id) {
      if (verbose)
        logger("connect em!\n");
      return &clients->client[i];
    }
  }
  return NULL;
}
