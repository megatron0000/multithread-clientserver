/**
 * Usage: ./server server_port
 *
 * Creates a server listening on `server_port` that accepts payloads from
 * clients containing a hash of a rubik cube. The server finds the moves
 * necessary to solve the cube and sends them back to the client.
 */

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "rubik-optimal/src/hash.cpp"
#include "rubik-optimal/src/solve.cpp"

// value read from /proc/sys/net/core/somaxconn
const int MAX_CONNECTION_QUEUE = 128;
const int MAX_PAYLOAD_SIZE = 100;

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

void start_server(int server_port, PruningTable* pruning_table) {
  struct sockaddr_in serv_addr;
  int serversockfd;
  int clientsockfd;
  serv_addr = preconnection_setup(server_port);

  serversockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (serversockfd < 0) {
    error("ERROR opening socket");
  }

  if (bind(serversockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    error("ERROR on binding");
  }

  listen(serversockfd, MAX_CONNECTION_QUEUE);

  // the solver should be unique per thread (when threads are implemented)
  auto solver = CubeSolver(pruning_table);

  while (true) {
    clientsockfd = accept(serversockfd, NULL, 0);

    if (clientsockfd < 0) {
      error("ERROR on accept");
    } else {
      printf("Client connected\n");
      fflush(stdout);
    }

    // should add client to work-queue. Consumers (other threads) will handle
    // the connection themselves and close it
    char* buffer = (char*)malloc(MAX_PAYLOAD_SIZE * sizeof(char));
    // read one byte at a time, until receive a '\0'
    int bytes_read = 0;
    do {
      if (read(clientsockfd, buffer + bytes_read, 1) <= 0) {
        error("ERROR on read from socket");
      }
      bytes_read++;
    } while (buffer[bytes_read - 1] != '\0');

    // solve the received cube
    string hash = "";
    for (int i = 0; buffer[i] != '\0'; i++) {
      hash = hash + buffer[i];
    }

    auto scrambled_cube = Hash2Permutation(hash);
    auto solution = solver.solve(scrambled_cube);

    for (int i = 0; i < solution.move_names.length(); i++) {
      buffer[i] = solution.move_names[i];
    }
    buffer[solution.move_names.length()] = '\0';

    if (write(clientsockfd, buffer, MAX_PAYLOAD_SIZE) < 0) {
      error("ERROR writing to socket");
    }

    close(clientsockfd);
  }

  close(serversockfd);
}

int main(int argc, char* argv[]) {
  int server_port;

  if (argc < 2) {
    fprintf(stderr, "usage %s server_port\n", argv[0]);
    exit(0);
  }
  server_port = atoi(argv[1]);

  // create pruning table
  printf("Loading pruning table...\n");
  PruningTable table;
  table.allocate();
  table.load_from_file("pruning_table.bin");
  printf("Loaded pruning table\n");

  start_server(server_port, &table);
}