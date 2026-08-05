/*******************************************************************************
 * @file        server.h
 * @brief       Declares the public interface for the Redis server.
 * @author      Josh Peters
 * @date        August 4, 2026
 *****************************************************************************/

#ifndef SERVER_H
#define SERVER_h

void server_start(void);
static int create_server_socket(void);
static int create_epoll(int server_fd);
static void event_loop(int epollfd, int server_fd);
static void accept_client(int server_fd, int epollfd);
static void handle_client(int conn_sock, int epollfd);
static void cleanup_client(int conn_sock, int epollfd);

#endif