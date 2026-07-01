#ifndef MAVLINK_TYPES_H
#define MAVLINK_TYPES_H

#include <stdint.h>   // uint8_t, uint16_t, uint32_t icin -- fixed-width integer'lar burada tanimli

// MAVLink v2 magic byte -- her v2 paketinin basladigi sabit deger
#define MAVLINK_V2_MAGIC 0xFD

// MAVLink v2 header'i -- Grup 1'de cikardigin anatomiyle BIREBIR eslesiyor:
// magic - len - incompat_flags - compat_flags - seq - sys_id - comp_id - msg_id(3 byte)
//
// ONEMLI: __attribute__((packed)) kullaniyoruz cunku derleyici normalde
// struct alanlari arasina hizalama (alignment) icin bos byte ekleyebilir.
// Bizim struct'imizin boyutu, protokolun byte-by-byte tanimiyla BIREBIR
// eslesmek zorunda -- yoksa parse ettigimizde yanlis offsetlerden okuruz.
typedef struct __attribute__((packed)) {
    uint8_t  magic;          // 0xFD sabiti -- paketin baslangic isareti
    uint8_t  len;             // payload uzunlugu (byte cinsinden)
    uint8_t  incompat_flags;  // uyumsuz flag'ler (ornegin signing zorunlu mu)
    uint8_t  compat_flags;    // uyumlu flag'ler (eski parser'lar yoksayabilir)
    uint8_t  seq;             // sequence number -- paket kaybi takibi icin
    uint8_t  sys_id;          // gonderen sistemin ID'si
    uint8_t  comp_id;         // gonderen sistem icindeki bilesenin ID'si
    uint8_t  msg_id[3];       // mesaj ID'si -- 3 byte (24 bit), little-endian
} mavlink_v2_header_t;

// Bu struct'in gercekten kac byte oldugunu derleme zamaninda kontrol etmek
// icin kullanacagiz. MAVLink v2 header'i tam olarak 10 byte olmali.
#define MAVLINK_V2_HEADER_SIZE 10

#endif // MAVLINK_TYPES_H
