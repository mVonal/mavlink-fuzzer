#ifndef MUTATOR_H
#define MUTATOR_H

#include <stdint.h>
#include <stddef.h>  // size_t icin

// Mutation stratejilerimiz -- her biri ayri bir teknigi temsil ediyor.
// Bunlari Grup 3 notlarindaki kavramlarla birebir eslestirdik.
typedef enum {
    MUTATE_BIT_FLIP,        // rastgele bir bitin degerini ters cevirir
    MUTATE_BOUNDARY_VALUE,  // rastgele bir byte'i bilinen sinir degerleriyle degistirir
    MUTATE_LENGTH_CORRUPT,  // len alanini kasitli olarak yanlislar
    MUTATE_STRATEGY_COUNT   // bu son eleman, kac tane strateji oldugunu saymak icin bir trick --
                             // C'de enum'lar otomatik 0'dan baslar, bu yuzden MUTATE_STRATEGY_COUNT
                             // = 3 olur (toplam strateji sayisi). Yeni strateji eklersek
                             // bu sayi otomatik guncellenir, elle degistirmemize gerek kalmaz.
} mutation_strategy_t;

// Bir paketi (raw_data) verilen strateji ile bozar.
// packet_len: paketin toplam byte uzunlugu (header+payload+checksum, hepsi dahil).
// Fonksiyon paketi YERINDE (in-place) degistirir, yani raw_data'nin
// kendisini bozar, yeni bir kopya dondurmez. Bu hem performansli hem basit.
void mutate_packet(uint8_t *raw_data, size_t packet_len, mutation_strategy_t strategy);

// Rastgele bir strateji secip uygulayan ust-duzey fonksiyon.
void mutate_packet_random(uint8_t *raw_data, size_t packet_len);

// Logger entegrasyonu icin: mutation aciklamasini desc_buf'a yazar.
void mutate_packet_random_logged(uint8_t *raw_data, size_t packet_len,
                                  char *desc_buf, size_t desc_buf_size);

#endif // MUTATOR_H
