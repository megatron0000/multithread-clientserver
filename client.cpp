/**
 * Usage: ./client server_hostname server_port client_count
 *
 * Creates a client that connects to `server_hostname`:`server_port`, sends a
 * scrambled rubik cube (a hash of it) and waits for the server to return the
 * moves which should be applied to the cube to solve it
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

#include "rubik-optimal/src/hash.cpp"

const int MAX_PAYLOAD_SIZE = 100;

void error(const char* msg) {
  perror(msg);
  printf("\n");
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

vector<Permutation> parse_moves(char* moves) {
  vector<Permutation> result;
  for (int i = 0; moves[i] != '\0'; i++) {
    int index = 0;
    if (moves[i] == ' ') {
      continue;
    }
    if (moves[i] == ';') {
      i++;
      break;
    }
    for (int m = 0; m < 6; m++) {
      if (CanonicalPermutationName[m][0] == moves[i]) {
        index = m;
        break;
      }
    }
    if (moves[i + 1] != '\0') {
      switch (moves[i + 1]) {
        case 'i':
          i++;
          index += 12;
          break;
        case '2':
          i++;
          index += 6;
          break;
        default:
          break;
      }
    }
    result.push_back(CanonicalPermutation[index]);
  }
  return result;
}

void connection_loop(int server_port, char* server_hostname) {
  struct sockaddr_in serv_addr;
  int sockfd;
  char* buffer;

  serv_addr = preconnection_setup(server_port, server_hostname);

  buffer = (char*)malloc(MAX_PAYLOAD_SIZE * sizeof(char));

  // limit to a few requests just for now
  int count = 10;
  while (count > 0) {
    count--;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      error("ERROR opening socket");
    }

    // build a cube to ask the server to solve
    Permutation p = Permutation::mult_vector({CanonicalPermutation[U],
                                              CanonicalPermutation[D],
                                              CanonicalPermutation[R]});
    string hash = Hash(p);
    for (int i = 0; i < hash.length(); i++) {
      buffer[i] = hash[i];
    }
    buffer[hash.length()] = '\0';

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      error("ERROR connecting");
    }

    if (write(sockfd, buffer, MAX_PAYLOAD_SIZE) < 0) {
      error("ERROR writing to socket");
    }

    // read one byte at a time, until receive a '\0'
    int bytes_read = 0;
    do {
      if (read(sockfd, buffer + bytes_read, 1) <= 0) {
        error("ERROR on read from socket");
      }
      bytes_read++;
    } while (buffer[bytes_read - 1] != '\0');

    printf("Client received %s\n", buffer);

    // check if the cube was correctly solved
    auto moves = parse_moves(buffer);
    for (int i = 0; i < moves.size(); i++) {
      p = Permutation::mult(p, moves[i]);
    }

    if (!Permutation::equals(p, Permutation::identity())) {
      error("ERROR cube was not solved correctly");
    }

    close(sockfd);
  }
}

int main(int argc, char* argv[]) {
  char* server_hostname;
  int server_port;
  int client_count;

  if (argc < 4) {
    fprintf(stderr, "usage %s server_hostname server_port client_count\n",
            argv[0]);
    exit(0);
  }
  server_hostname = argv[1];
  server_port = atoi(argv[2]);
  client_count = atoi(argv[3]);

  connection_loop(server_port, server_hostname);
}