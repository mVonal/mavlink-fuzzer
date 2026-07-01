#include <stdio.h>
#include <string.h>
#include <unistd.h>         
#include <arpa/inet.h>      
#include <sys/socket.h>    
#include "sender.h"


int sender_init(const char *ip, int port) {

    
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("HATA: Socket olusturulamadi");
        return -1;
    }

    
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr)); 
    dest_addr.sin_family      = AF_INET;
    dest_addr.sin_port        = htons(port);  
    dest_addr.sin_addr.s_addr = inet_addr(ip);

    
    if (connect(sock_fd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("HATA: Hedefe baglanamadi");
        close(sock_fd);
        return -1;
    }

    printf("[SENDER] UDP socket hazir -> %s:%d\n", ip, port);
    return sock_fd;
}

int sender_send(int sock_fd, const uint8_t *data, size_t len) {
    ssize_t sent = send(sock_fd, data, len, 0);
    if (sent < 0) {
        perror("HATA: Paket gonderilemedi");
        return -1;
    }
    return (int)sent;
}


void sender_close(int sock_fd) {
    close(sock_fd);
    printf("[SENDER] Socket kapatildi.\n");
}
