/*******************************************************************************
 * @file        server.c
 * @brief       Provides functions sockets, epoll, and event loop.
 * @author      Josh Peters
 * @date        August 3, 2026
 * *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "server.h"

#define MAX_EVENTS 10

/*********************************
 * @brief Starts the Redis server.
 ********************************/
void server_start(void) {
    // Disable output buffering
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    printf("Logs from your program will appear here!\n");

    int server_fd = create_server_socket();
    if (server_fd == -1) {
        printf("create_server_socket: %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int epollfd = create_epoll(server_fd);
    if (epollfd == -1) {
        printf("create_epoll: %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    event_loop(epollfd, server_fd);
}

/************************************************************************
 * @brief Creates, configures, and begins listening on the server socket.
 ***********************************************************************/
static int create_server_socket(void) {
    int server_fd, reuse, connection_backlog;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        printf("Socket creation failed: %s...\n", strerror(errno));
        return -1;
    }

	// Since the tester restarts your program quite often, setting SO_REUSEADDR
	// ensures that we don't run into 'Address already in use' errors
    reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        printf("SO_REUSEADDR failed: %s \n", strerror(errno));
        return -1;
    }
    
    struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
        .sin_port = htons(6379),
        .sin_addr = { htonl(INADDR_ANY) },
    };

	if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
		printf("Bind failed: %s \n", strerror(errno));
		return -1;
	}

    connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0) {
        printf("Listen failed: %s \n", strerror(errno));
        return -1;
    }

    return server_fd;
}

/**********************************************************************
 * @brief Creates an epoll instance and registers the listening socket.
 *********************************************************************/
static int create_epoll(int server_fd) {
    struct epoll_event ev;
    int epollfd;

    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        printf("epoll_create1: %s \n", strerror(errno));
        return -1;
    }

    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        printf("epoll_ctl: %s \n", strerror(errno));
        return -1;
    }

    return epollfd;
}

/***********************************************
 * @brief Waits for and processes socket events.
 **********************************************/
static void event_loop(int epollfd, int server_fd) {
    int nfds;
    struct epoll_event events[MAX_EVENTS];

    for (;;) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            printf("epoll_wait: %s \n", strerror(errno));
            return;
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == server_fd) {
                accept_client(server_fd, epollfd);
            } else {
                handle_client(events[n].data.fd, epollfd);
            }
        }
    }
}

/**********************************************************
 * @brief Accepts a new client and registers it with epoll.
 *********************************************************/
static void accept_client(int server_fd, int epollfd) {
    int client_fd;
    socklen_t client_addr_len;
    struct sockaddr_in client_addr;
    struct epoll_event ev;

    printf("Waiting for a client to connect...\n");
    client_addr_len = sizeof(client_addr);

    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    printf("Client connected\n");
    if (client_fd == -1) {
        printf("accept: %s \n", strerror(errno));
        return;
    }

    ev.events = EPOLLIN;
    ev.data.fd = client_fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
        printf("epoll_ctl: %s \n", strerror(errno));
        return;
    }
}

/****************************************************
 * @brief Processes requests from a connected client.
 ***************************************************/
static void handle_client(int conn_sock, int epollfd) {
  char buffer[1024];
  const char *response = "+PONG\r\n"; // TODO: Parse the request and generate a RESP response.
  ssize_t bytes_read = recv(conn_sock, buffer, sizeof(buffer), 0);
  printf("%.*s\n", (int)bytes_read, buffer[0]);

  // If client disconnects
  if (bytes_read == 0) {
    cleanup_client(conn_sock, epollfd);
    return;
  }

  // If recv fails
  if (bytes_read < 0) {
    printf("recv: %s \n", strerror(errno));
    cleanup_client(conn_sock, epollfd);
    return;
  }

  // Process request by calling parse_resp from resp.c
  if (send(conn_sock, response, strlen(response), 0) == -1) {
    printf("send: %s \n" , strerror(errno));
    cleanup_client(conn_sock, epollfd);
  }
}

/***********************************************************
 * @brief Removes a client from epoll and closes its socket.
 **********************************************************/
static void cleanup_client(int conn_sock, int epollfd) {
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, conn_sock, NULL) == -1) {
        printf("epoll_ctl: %s \n", strerror(errno));
    }
    if (close(conn_sock) == -1) {
        printf("close: %s \n", strerror(errno));
    }
}