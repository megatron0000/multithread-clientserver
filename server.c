/**
 * Usage: ./server server_port payload_size
 * 
 * Creates a server listening on `server_port` that accepts payloads from clients
 * and echoes the payload back to them.
 */

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// value read from /proc/sys/net/core/somaxconn
const int MAX_CONNECTION_QUEUE = 128;

void error(const char* msg) {
  perror(msg);
  exit(1);
}

struct sockaddr_in preconnection_setup(int server_port) {
  struct sockaddr_in serv_addr;

  bzero((char*)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(server_port);

  return serv_addr;
}

void start_server(int server_port, int payload_size) {
  struct sockaddr_in serv_addr;
  int serversockfd;
  struct sockaddr_in client_addr;
  int clientsockfd;
  socklen_t clientlength;

  serv_addr = preconnection_setup(server_port);

  serversockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (serversockfd < 0) {
    error("ERROR opening socket");
  }

  if (bind(serversockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    error("ERROR on binding");
  }

  listen(serversockfd, MAX_CONNECTION_QUEUE);

  while (1) {
    clientsockfd =
        accept(serversockfd, (struct sockaddr*)&client_addr, &clientlength);

    if (clientsockfd < 0) {
      error("ERROR on accept");
    } else {
      printf("Client connected\n");
    }

    // add client to work-queue. Consumers (other threads) will handle
    // the connection themselves and close it
    char* buffer = malloc(payload_size * sizeof(char));
    read(clientsockfd, buffer, payload_size);
    write(clientsockfd, buffer, payload_size);
    close(clientsockfd);
  }

  close(serversockfd);
}

int main(int argc, char* argv[]) {
  int server_port;
  int payload_size;

  if (argc < 3) {
    fprintf(stderr, "usage %s server_port payload_size\n", argv[0]);
    exit(0);
  }
  server_port = atoi(argv[1]);
  payload_size = atoi(argv[2]);

  start_server(server_port, payload_size);
}