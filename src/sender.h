#ifndef SENDER_H
#define SENDER_H

#include <stdint.h>
#include <stddef.h>

#define SITL_IP   "127.0.0.1"
#define SITL_PORT 5501


int sender_init(const char *ip, int port);


int sender_send(int sock_fd, const uint8_t *data, size_t len);


void sender_close(int sock_fd);

#endif 
