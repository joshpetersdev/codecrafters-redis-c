/*******************************************************************************
 * @file        main.c
 * @brief       Program entry point for the Redis server.
 * @author      Josh Peters
 * @date        August 3, 2026
 *****************************************************************************/

#include "server.h"

int main() {
  server_start();
  return 0;
}

// #define MAX_EVENTS 10

// int main() {
// 	// Disable output buffering
// 	setbuf(stdout, NULL);
// 	setbuf(stderr, NULL);
	
// 	// You can use print statements as follows for debugging, they'll be visible when running tests.
// 	printf("Logs from your program will appear here!\n");

// 	int server_fd, client_addr_len;
// 	struct sockaddr_in client_addr;

// 	server_fd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (server_fd == -1) {
// 		printf("Socket creation failed: %s...\n", strerror(errno));
// 		return 1;
// 	}

// 	// Since the tester restarts your program quite often, setting SO_REUSEADDR
// 	// ensures that we don't run into 'Address already in use' errors
// 	int reuse = 1;
// 	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
// 		printf("SO_REUSEADDR failed: %s \n", strerror(errno));
// 		return 1;
// 	}

// 	struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
// 									 .sin_port = htons(6379),
// 									 .sin_addr = { htonl(INADDR_ANY) },
// 									};

// 	if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
// 		printf("Bind failed: %s \n", strerror(errno));
// 		return 1;
// 	}

// 	int connection_backlog = 5;
// 	if (listen(server_fd, connection_backlog) != 0) {
// 		printf("Listen failed: %s \n", strerror(errno));
// 		return 1;
// 	}

// 	printf("Waiting for a client to connect...\n");
// 	client_addr_len = sizeof(client_addr);

//   struct epoll_event ev, events[MAX_EVENTS];
//   int client_fd, nfds, epollfd;

//   // Create an epoll instance
//   epollfd = epoll_create1(0);
//   if (epollfd == -1) {
//     perror("epoll_create1");
//     exit(EXIT_FAILURE);
//   }

//   // Register file descriptor
//   ev.events = EPOLLIN;
//   ev.data.fd = server_fd;
//   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
//     perror("epoll_ctl: server_fd");
//     exit(EXIT_FAILURE);
//   }

//   // Wait for events
//   for (;;) {
//     nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
//     if (nfds == -1) {
//       perror("epoll_wait");
//       exit(EXIT_FAILURE);
//     }

//     for (int n = 0; n < nfds; ++n) {
//       if (events[n].data.fd == server_fd) {
//         client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
//         printf("Client connected\n");
//         if (client_fd == -1) {
//           perror("accept");
//           exit(EXIT_FAILURE);
//         }

//         // setnonblocking(client_fd);
//         ev.events = EPOLLIN;
//         ev.data.fd = client_fd;
//         if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
//           perror("epoll_ctl: client_fd");
//           exit(EXIT_FAILURE);
//         }
//       } else {
//         do_use_fd(events[n].data.fd);
//       }
//     }
//   }

// 	close(server_fd);

// 	return 0;
// }
