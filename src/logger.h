#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <stddef.h>
#include "mavlink_types.h"

// Log kayitlarinin yazilacagi dosya
#define LOG_FILE "fuzzer_results.log"

// Logger'i baslatir, log dosyasini olusturur/acar.
// Basarili olursa 0, hata durumunda -1 doner.
int logger_init(const char *filename);

// Gonderilen bir fuzz paketini, mutation bilgisiyle birlikte loglar.
// packet_num    : kacinci fuzz paketi oldugu
// mutation_desc : hangi mutation uygulandiginin aciklamasi (MUTATE logundan gelen string)
// raw_data      : gonderilen paketin ham byte'lari
// packet_len    : paketin uzunlugu
// sent_ok       : paketin basariyla gonderilip gonderilmedigi (1=evet, 0=hayir)
void logger_log_packet(int packet_num,
                       const char *mutation_desc,
                       const uint8_t *raw_data,
                       size_t packet_len,
                       int sent_ok);

// Fuzzing oturumunun ozet istatistiklerini loglar.
void logger_log_summary(int total_sent, int total_failed);

// Log dosyasini kapatir.
void logger_close(void);

#endif // LOGGER_H
