/**
 * Usage: ./client server_hostname server_port client_count seconds_duration
 *
 * Creates a client that connects to `server_hostname`:`server_port`, sends a
 * scrambled rubik cube (a hash of it) and waits for the server to return the
 * moves which should be applied to the cube to solve it.
 *
 * 'client_count' threads are created to establish connections, and each one
 * connects indefinitely many times to the server, sending the cube and
 * verifying the response.
 *
 * When the main thread wants the others to stop, it signals so by the
 * arguments passed to them. This shutdown procedure is done
 * 'seconds_duration' seconds after the start of the program.
 *
 * The program then reports statistics of the run.
 *
 */

#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "rubik-optimal/src/hash.cpp"

#include "setdebug.h"

const int MAX_PAYLOAD_SIZE = 100;

void error(const char* msg) {
  perror(msg);
  exit(1);
}

// see: https://stackoverflow.com/questions/10192903/time-in-milliseconds-in-c
float timedifference_msec(struct timeval t0, struct timeval t1) {
  return (t1.tv_sec - t0.tv_sec) * 1000.0f +
         (t1.tv_usec - t0.tv_usec) / 1000.0f;
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

/**
 * Each client thread will connect to the server and establish
 * (one at a time) connections to it.
 *
 * On every successfully closed connection, 'request_count'
 * is incremented by the thread, using 'mutex' to avoid
 * sync problems.
 *
 * 'should_stop' is set by the main thread when it wants
 * the works to return.
 *
 */
struct connection_loop_arg_t {
  int server_port;
  char* server_hostname;
  int request_count;
  sem_t mutex;
  bool should_stop;
};

void* connection_loop(void* args) {
  int server_port = ((connection_loop_arg_t*)args)->server_port;
  char* server_hostname = ((connection_loop_arg_t*)args)->server_hostname;
  sem_t* mutex = &((connection_loop_arg_t*)args)->mutex;
  int* request_count = &((connection_loop_arg_t*)args)->request_count;
  bool* should_stop = &((connection_loop_arg_t*)args)->should_stop;
  struct sockaddr_in serv_addr;
  int sockfd;
  char* buffer;

  serv_addr = preconnection_setup(server_port, server_hostname);

  buffer = (char*)malloc(MAX_PAYLOAD_SIZE * sizeof(char));

  // build a cube to ask the server to solve.
  // this configuration takes 11 moves to solve.
  Permutation reference = Permutation::mult_vector(
      {CanonicalPermutation[U], CanonicalPermutation[R],
       CanonicalPermutation[Di], CanonicalPermutation[R2],
       CanonicalPermutation[F], CanonicalPermutation[Li],
       CanonicalPermutation[U], CanonicalPermutation[D2],
       CanonicalPermutation[Ri], CanonicalPermutation[F2],
       CanonicalPermutation[B]});

  while (true) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      error("ERROR opening socket");
    }

    // copy-constructor (means 'test' is a copy of 'reference')
    Permutation test = reference;

    string hash = Hash(test);
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

    if (recv(sockfd, buffer, MAX_PAYLOAD_SIZE, MSG_WAITALL) < 0) {
      error("ERROR on receive from socket");
    }

    printf("Client received %s\n", buffer);

    // check if the cube was correctly solved
    auto moves = parse_moves(buffer);
    for (int i = 0; i < moves.size(); i++) {
      test = Permutation::mult(test, moves[i]);
    }

    if (!Permutation::equals(test, Permutation::identity())) {
      error("ERROR cube was not solved correctly");
    }

    close(sockfd);

    sem_wait(mutex);
    *request_count += 1;
    sem_post(mutex);

    // stop if the main thread signaled so
    if (*should_stop) {
      free(buffer);
      return (void*)NULL;
    }
  }
}

int main(int argc, char* argv[]) {
  char* server_hostname;
  int server_port;
  int client_count;
  int requests_per_client;
  int duration_seconds;

  if (argc < 5) {
    fprintf(
        stderr,
        "usage %s server_hostname server_port client_count duration_seconds\n",
        argv[0]);
    exit(0);
  }
  server_hostname = argv[1];
  server_port = atoi(argv[2]);
  client_count = atoi(argv[3]);
  duration_seconds = atoi(argv[4]);

  pthread_t* threads = (pthread_t*)malloc(client_count * sizeof(pthread_t*));

  // setup args for worker threads
  connection_loop_arg_t args;
  args.server_port = server_port;
  args.server_hostname = server_hostname;
  args.request_count = 0;
  args.should_stop = false;
  // start with 0 to block workers
  sem_init(&args.mutex, 1, 0);

  for (int i = 0; i < client_count; i++) {
    pthread_create(&threads[i], NULL, connection_loop, &args);
  }

  struct timeval start;
  if (gettimeofday(&start, 0) != 0) {
    error("ERROR on acquire time");
  }

  // unlock to let workers run
  sem_post(&args.mutex);

  // capture data for about 'duration_seconds' seconds
  usleep(duration_seconds * 1000000);

  // signal workers to stop
  args.should_stop = true;

  for (int i = 0; i < client_count; i++) {
    pthread_join(threads[i], NULL);
  }
  free(threads);

  struct timeval end;
  if (gettimeofday(&end, 0) != 0) {
    error("ERROR on acquire time");
  }

  float elapsed = timedifference_msec(start, end);

  cout << "Ran for " << elapsed << " milliseconds;" << endl;
  cout << "Processed " << args.request_count << " requests;" << endl;

  return 0;
}