#include <stdio.h>
#include <string.h>
#include "mavlink_types.h"

// Bu fonksiyon, ham byte dizisini (raw_data) alir ve mavlink_v2_header_t
// struct'ina parse eder. Senin Python sniffer'inda elle yaptigin
// "data[7] | (data[8]<<8) | (data[9]<<16)" islemini burada C ile yapacagiz.
//
// Donus degeri: basarili olursa 0, magic byte yanlissa -1 doner.
int parse_mavlink_header(const uint8_t *raw_data, mavlink_v2_header_t *out_header) {

    // ADIM 1: Magic byte kontrolu -- gelen veri gercekten MAVLink v2 mi?
    if (raw_data[0] != MAVLINK_V2_MAGIC) {
        printf("HATA: Magic byte uyusmuyor. Beklenen: 0x%02X, Gelen: 0x%02X\n",
               MAVLINK_V2_MAGIC, raw_data[0]);
        return -1;
    }

    // ADIM 2: memcpy ile struct'i ham veriden doldurmak.
    // Struct'imiz packed ve byte-by-byte protokolle birebir eslesiyor
    // oldugu icin, ilk 10 byte'i dogrudan struct'in uzerine kopyalayabiliriz.
    // Bu, struct packing'in asil gucu -- elle her alani tek tek atamak
    // yerine tek bir memcpy ile tum header'i doldurabiliyoruz.
    memcpy(out_header, raw_data, MAVLINK_V2_HEADER_SIZE);

    return 0;
}

// msg_id alanimiz 3 byte'lik bir array (uint8_t[3]) oldugu icin,
// bunu okunabilir tek bir sayiya (uint32_t) cevirmemiz gerekiyor.
// Bu fonksiyon tam olarak senin Python'da yaptigin bit-shift islemini
// C tarafinda yapiyor.
uint32_t get_message_id(const mavlink_v2_header_t *header) {
    return header->msg_id[0]
         | (header->msg_id[1] << 8)
         | (header->msg_id[2] << 16);
}

// Header'i okunabilir formatta ekrana basan yardimci fonksiyon.
void print_header(const mavlink_v2_header_t *header) {
    printf("--- MAVLink v2 Header ---\n");
    printf("Magic:          0x%02X\n", header->magic);
    printf("Payload Length: %u\n", header->len);
    printf("Incompat Flags: 0x%02X\n", header->incompat_flags);
    printf("Compat Flags:   0x%02X\n", header->compat_flags);
    printf("Sequence:       %u\n", header->seq);
    printf("System ID:      %u\n", header->sys_id);
    printf("Component ID:   %u\n", header->comp_id);
    printf("Message ID:     %u\n", get_message_id(header));
    printf("-------------------------\n");
}
