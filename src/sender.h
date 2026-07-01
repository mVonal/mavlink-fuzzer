#ifndef SENDER_H
#define SENDER_H

#include <stdint.h>
#include <stddef.h>

// SITL'in MAVLink paketlerini dinledigi adres ve port.
// MAVProxy'nin --out parametresinde gordugumuz deger: 127.0.0.1:14550
// Biz bu porta paket GONDERECEGIZ (MAVProxy oradan dinliyor).
#define SITL_IP   "127.0.0.1"
#define SITL_PORT 5501

// UDP socket'i olusturur ve hedef adrese baglar.
// Basarili olursa socket file descriptor'i doner, hata durumunda -1.
int sender_init(const char *ip, int port);

// Hazirlanmis (ve muhtemelen bozulmus) paketi UDP uzerinden gonderir.
// Basarili olursa gonderilen byte sayisini, hata durumunda -1 doner.
int sender_send(int sock_fd, const uint8_t *data, size_t len);

// Socket'i kapatir, kaynaklari serbest birakir.
void sender_close(int sock_fd);

#endif // SENDER_H
