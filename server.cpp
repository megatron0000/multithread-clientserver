/**
 * Usage: ./server server_port worker_count
 *
 * Creates a server listening on `server_port` that accepts payloads from
 * clients containing a hash of a rubik cube. The server finds the moves
 * necessary to solve the cube and sends them back to the client.
 *
 * The server creates 'worker_count' threads to handle clients. Each one
 * loops connecting to a client, solving the rubik cube and
 * sending the response back to the client.
 */

#include <netinet/in.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "rubik-optimal/src/hash.cpp"
#include "rubik-optimal/src/solve.cpp"

#include "setdebug.h"

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

/**
 * The main thread produces client sockets to this queue.
 * Worker threads remove clients from the queue and process them
 */
struct queue_t {
  /**
   * A lead node which does not represent a client. Its sucessors do.
   */
  struct queue_item_t* lead;
  /**
   * Just like a lead node, but for the end of the queue
   */
  struct queue_item_t* lead_last;
  sem_t length;
  sem_t mutex;
};

struct queue_item_t {
  int clientsockfd;
  struct queue_item_t* next;
  struct queue_item_t* before;
};

/**
 * Arguments for handle_client_worker function
 */
struct worker_args {
  struct queue_t* client_queue;
  PruningTable* pruning_table;
};

/**
 * All worker threads need access to the client queue and
 * to the same pruning table (= 1 gigabyte)
 */
void* handle_client_worker(void* worker_args) {
  struct queue_t* queue = ((struct worker_args*)worker_args)->client_queue;
  PruningTable* table = ((struct worker_args*)worker_args)->pruning_table;

  auto solver = CubeSolver(table);

  char* buffer = (char*)malloc(MAX_PAYLOAD_SIZE * sizeof(char));

  while (true) {
    // wait for a client to arrive on the queue, then remove it
    sem_wait(&queue->length);
    sem_wait(&queue->mutex);
    struct queue_item_t* lead_last = queue->lead_last;
    struct queue_item_t* oldlast = lead_last->before;
    struct queue_item_t* newlast = oldlast->before;
    newlast->next = lead_last;
    lead_last->before = newlast;
    sem_post(&queue->mutex);
    int clientsockfd = oldlast->clientsockfd;
    free(oldlast);

    if (recv(clientsockfd, buffer, MAX_PAYLOAD_SIZE, MSG_WAITALL) < 0) {
      error("ERROR in receive from socket");
    }

    printf("Server received %s\n", buffer);

    // receive the cube. Take care with \0
    string hash = "";
    int count = 0;
    while (true) {
      if (buffer[count] == '\0') {
        break;
      }
      if (count == MAX_PAYLOAD_SIZE) {
        error("ERROR did not receive \\0 terminator");
      }
      hash = hash + buffer[count];
      count++;
    }

    // solve the received cube
    auto scrambled_cube = Hash2Permutation(hash);
    auto solution = solver.solve(scrambled_cube);

    // write the moves found for solution of the cube
    for (int i = 0; i < solution.move_names.length(); i++) {
      buffer[i] = solution.move_names[i];
    }
    buffer[solution.move_names.length()] = '\0';

    if (write(clientsockfd, buffer, MAX_PAYLOAD_SIZE) < 0) {
      error("ERROR writing to socket");
    }

    close(clientsockfd);
  }
}

void start_server(int server_port, int worker_count) {
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

  // create pruning table
  cout << "Loading pruning table..." << endl;
  PruningTable table;
  table.allocate();
  table.load_from_file("pruning_table.bin");
  cout << "Loaded pruning table. Listening for connections on " << server_port
       << endl;

  // create client queue
  struct queue_t queue;
  queue.lead = (queue_item_t*)malloc(sizeof(queue_item_t));
  queue.lead_last = (queue_item_t*)malloc(sizeof(queue_item_t));
  queue.lead->before = NULL;
  queue.lead->next = queue.lead_last;
  queue.lead_last->before = queue.lead;
  queue.lead_last->next = NULL;
  sem_init(&queue.length, 1, 0);
  sem_init(&queue.mutex, 1, 1);

  // create worker threads
  pthread_t* workers =
      (pthread_t*)malloc(worker_count * sizeof(pthread_t));
  struct worker_args args;
  args.client_queue = &queue;
  args.pruning_table = &table;
  for (int i = 0; i < worker_count; i++) {
    pthread_create(&workers[i], NULL, handle_client_worker, (void*)&args);
  }

  while (true) {
    clientsockfd = accept(serversockfd, NULL, 0);

    if (clientsockfd < 0) {
      error("ERROR on accept");
    } else {
      printf("Client connected\n");
    }

    // enqueue client (a worker will pick it up)
    sem_wait(&queue.mutex);
    struct queue_item_t* newclient =
        (struct queue_item_t*)malloc(sizeof(struct queue_item_t));
    struct queue_item_t* current_first = queue.lead->next;
    newclient->before = queue.lead;
    newclient->next = current_first;
    queue.lead->next = newclient;
    current_first->before = newclient;
    newclient->clientsockfd = clientsockfd;
    sem_post(&queue.mutex);
    sem_post(&queue.length);
  }

  // finalization code. Will never be reached
  close(serversockfd);
  for (int i = 0; i < worker_count; i++) {
    pthread_join(workers[i], NULL);
  }
}

int main(int argc, char* argv[]) {
  int server_port;
  int worker_count;

  if (argc < 3) {
    fprintf(stderr, "usage %s server_port worker_count\n", argv[0]);
    exit(0);
  }
  server_port = atoi(argv[1]);
  worker_count = atoi(argv[2]);

  start_server(server_port, worker_count);
}