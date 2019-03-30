/**
 * Usage: ./client server_hostname server_port client_count payload_size
 * 
 * Creates a client that connects to `server_hostname`:`server_port`, sends a payload
 * (`payload_size` bytes), expects the server to echo the payload back and tests
 * if payload==response
 * 
 * TODO: Create multiple threads to spawn `client_count` number of
 * clients to stress server
 */

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void error(const char* msg) {
  perror(msg);
  exit(0);
}

struct sockaddr_in preconnection_setup(int server_port, char* server_hostname) {
  struct hostent* server_host;
  struct sockaddr_in serv_addr;

  server_host = gethostbyname(server_hostname);

  bzero((char*)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char*)server_host->h_addr, (char*)&serv_addr.sin_addr.s_addr,
        server_host->h_length);
  serv_addr.sin_port = htons(server_port);

  return serv_addr;
}

void connection_loop(int server_port, char* server_hostname, int payload_size) {
  struct sockaddr_in serv_addr;
  int sockfd;
  char* payload_buffer;
  char* response_buffer;

  serv_addr = preconnection_setup(server_port, server_hostname);

  payload_buffer = malloc(payload_size * sizeof(char));
  response_buffer = malloc(payload_size * sizeof(char));

  while (1) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      error("ERROR opening socket");
    }

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
      error("ERROR connecting");

    if (write(sockfd, payload_buffer, strlen(payload_buffer)) < 0) {
      error("ERROR writing to socket");
    }

    if (read(sockfd, response_buffer, payload_size) < 0) {
      error("ERROR reading from socket");
    }

    for(int i = 0; i < payload_size; i++) {
      if (payload_buffer[i] != response_buffer[i]) {
        error("ERROR payload does not match server response");
      }
    }

    close(sockfd);
    return;
  }
}

int main(int argc, char* argv[]) {
  char* server_hostname;
  int server_port;
  int client_count;
  int payload_size;
  
  if (argc < 5) {
    fprintf(stderr, "usage %s server_hostname server_port client_count payload_size\n",
            argv[0]);
    exit(0);
  }
  server_hostname = argv[1];
  server_port = atoi(argv[2]);
  client_count = atoi(argv[3]);
  payload_size = atoi(argv[4]);

  connection_loop(server_port, server_hostname, payload_size);
}