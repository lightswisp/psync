#include <sys/socket.h>
#include <stdint.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include "structs.h"

bool peer_is_uniq_client(clients_t *clients, struct in_addr *ip);
bool peer_get_free_slot(clients_t *clients, size_t *slot);
void peer_timer_cleanup(clients_t *clients);
client_t *peer_exists(clients_t *clients, uint32_t peer_id);
