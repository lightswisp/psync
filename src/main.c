#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>  
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <structs.h>
#include <show.h>
#include <peer.h>

bool verbose; //global

static void check_config(int server_family, uint16_t port, bool verbose, char* password){
  if (server_family != AF_INET) {
    fprintf(stderr, "error: AF_INET must be selected!\n");
    exit(EXIT_FAILURE);
  }
  if (port < 1024) {
    fprintf(stderr, "error: reserved port was selected!\n");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[]){
  if (argc < 2)
    show_help(argv[0]);

  int opt, server_family;
  uint16_t port;
#define PWD_LIMIT 10 
  char password[PWD_LIMIT];

  while ((opt = getopt(argc, argv, "4vp:P:")) != -1) {
    switch (opt) {
    case '4':
      server_family = AF_INET;
      break;
    case 'v':
      verbose = true;
      break;
    case 'p':
      port = atoi(optarg);
      break;
    case 'P':
      strncpy(password, optarg, sizeof(password));
      password[sizeof(password) - 1] = '\0';
      break;
    }
  }

  show_config(server_family, port, verbose, password); 
  check_config(server_family, port, verbose, password);

  int sockfd;
  struct sockaddr_in servaddr, clientaddr;
  if ((sockfd = socket(server_family, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&clientaddr, 0, sizeof(clientaddr));

  servaddr.sin_family = server_family;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  clients_t clients;
  bzero(&clients, sizeof(clients));

#define ADD_CLIENT(buf, saddr, clientlen)                                      \
  if (clients.size < MAX_CLIENTS) {                                            \
    size_t slot;                                                               \
    if (!get_free_slot(&clients, &slot)) {                                     \
      if (verbose)                                                             \
        logger("client limit is reached!\n");                                  \
      continue;                                                                \
    }                                                                          \
    clients.client[slot].active = true;                                        \
    clients.client[slot].data = buf;                                           \
    clients.client[slot].addr = saddr;                                         \
    clients.client[slot].time = time(NULL);                                    \
    clients.client[slot].len = clientlen;                                      \
    clients.size++;                                                            \
  }

#define PRINT_CLIENTS                                                          \
  if (verbose)                                                                 \
    logger("clients we have connected: %lu\n", clients.size);                  \
  for (size_t i = 0; i < MAX_CLIENTS; i++) {                                   \
    if (clients.client[i].active) {                                            \
      char source_ip[INET_ADDRSTRLEN];                                         \
      inet_ntop(server_family, &(clients.client[i].addr.sin_addr), source_ip,  \
                INET_ADDRSTRLEN);                                              \
      if (verbose)                                                             \
        logger("client<%lu>: ip=%s self_id=0x%x peer_id=0x%x\n", i, source_ip, \
               clients.client[i].data.self_id,                                 \
               clients.client[i].data.peer_id);                                \
    }                                                                          \
  }

  client_data_t buf;
  socklen_t len = sizeof(clientaddr);
  char source_ip[INET_ADDRSTRLEN];
  uint16_t source_port;

  logger("listening on port: %u\n", port);

  for (;;) {
    bzero(&buf, sizeof(buf));
    recvfrom(sockfd, (void *)&buf, sizeof(buf), MSG_WAITALL,
             (struct sockaddr *)&clientaddr, &len);

    source_port = htons(clientaddr.sin_port);
    inet_ntop(server_family, &(clientaddr.sin_addr), source_ip, INET_ADDRSTRLEN);

    if (verbose)
      logger("received 0x%x 0x%x from: %s:%d\n", buf.self_id, buf.peer_id, source_ip, source_port);

    /* perform the peer lookup */
    client_t* other_peer;
    if ( (other_peer = peer_exists(&clients, buf.peer_id)) != NULL) {

      // maybe there is a small chance that two ports are gonna be the same?
      uint16_t port1 = (rand() % (65535 + 1 - 1024)) + 1024; 
      uint16_t port2 = (rand() % (65535 + 1 - 1024)) + 1024;

      //peer1 is the one that just connected
      if (verbose)
        logger("peer1 port: %u | peer2 port: %u\n", port1, port2);

      peer_data_t peer1 = {.src_port = port1, .dst_addr = clientaddr};
      peer_data_t peer2 = {.src_port = port2, .dst_addr = other_peer->addr};

      sendto(sockfd, &peer1, sizeof(peer_data_t), 0,
             (struct sockaddr *)(&other_peer->addr), other_peer->len);
      sendto(sockfd, &peer2, sizeof(peer_data_t), 0,
             (struct sockaddr *)&clientaddr, len);

      if (verbose)
        logger("sent %ld bytes\n", sizeof(peer_data_t));

      /* cleanup */
      bzero(other_peer, sizeof(client_t));
      clients.size--;
      continue;
    }

    /* remove old connections */
    timer_cleanup(&clients);

    /* skip if not unique */
    if (!uniq_client(&clients, &(clientaddr.sin_addr)))
      continue;

    ADD_CLIENT(buf, clientaddr, len)
    PRINT_CLIENTS
  }
  return 0;
}
