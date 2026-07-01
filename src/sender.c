#include <stdio.h>
#include <string.h>
#include <unistd.h>         // close() icin
#include <arpa/inet.h>      // inet_addr(), htons() icin -- network byte order donusumleri
#include <sys/socket.h>     // socket(), sendto() icin
#include "sender.h"

// sender_init: UDP socket olusturur ve hedef adresi yapilandirir.
// Burada "connect" aslinda TCP'deki gibi el sikismasi yapmaz --
// UDP'de connect sadece "bu socket'ten gonderilen paketler su adrese gitsin"
// diye varsayilan hedefi kaydetmek icin kullanilir.
int sender_init(const char *ip, int port) {

    // ADIM 1: Socket olustur
    // AF_INET   = IPv4 adresleme ailesi
    // SOCK_DGRAM = datagram (yani UDP) -- SOCK_STREAM olsaydi TCP olurdu
    // 0         = protokol otomatik secilsin (UDP icin 0 dogru)
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("HATA: Socket olusturulamadi");
        return -1;
    }

    // ADIM 2: Hedef adresi yapilandir
    // sockaddr_in, IPv4 adresi + port bilgisini tutan C struct'i.
    // Bu struct sistem kutuphanesinde tanimli (arpa/inet.h).
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));  // once sifirla -- rastgele deger kalmasin
    dest_addr.sin_family      = AF_INET;
    dest_addr.sin_port        = htons(port);   // htons: host to network short -- byte order donusumu
    dest_addr.sin_addr.s_addr = inet_addr(ip); // string IP'yi binary'e cevir ("127.0.0.1" -> uint32_t)

    // ADIM 3: Socket'i hedefe "bagla" (UDP'de sadece varsayilan hedef kaydeder)
    if (connect(sock_fd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("HATA: Hedefe baglanamadi");
        close(sock_fd);
        return -1;
    }

    printf("[SENDER] UDP socket hazir -> %s:%d\n", ip, port);
    return sock_fd;
}

// sender_send: onceden init edilmis socket uzerinden paketi gonderir.
// connect() ile varsayilan hedef zaten ayarli oldugu icin
// send() kullanabiliyoruz (sendto()'ya adres vermek zorunda degiliz).
int sender_send(int sock_fd, const uint8_t *data, size_t len) {
    ssize_t sent = send(sock_fd, data, len, 0);
    if (sent < 0) {
        perror("HATA: Paket gonderilemedi");
        return -1;
    }
    return (int)sent;
}

// sender_close: sistem kaynagini serbest birak.
// Her acilan socket file descriptor kapatilmalidir --
// kapatilmazsa "file descriptor leak" olusur.
void sender_close(int sock_fd) {
    close(sock_fd);
    printf("[SENDER] Socket kapatildi.\n");
}
