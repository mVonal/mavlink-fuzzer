#include <stdio.h>
#include <time.h>
#include <string.h>
#include "logger.h"

// Log dosyasinin global handle'i.
// static: bu degisken sadece bu .c dosyasi icinde gorunur,
// disaridan dogrudan erisim kapatilmis olur -- iyi C pratigi.
static FILE *log_fp = NULL;

// Anlik zamani "2025-07-01 14:32:01" formatinda bir string olarak dondurur.
// static buffer kullaniyor -- thread-safe degil ama bizim single-threaded
// fuzzer'imiz icin tamamen yeterli.
static const char *current_timestamp(void) {
    static char buf[32];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
    return buf;
}

int logger_init(const char *filename) {
    log_fp = fopen(filename, "w");  // "w": her calistirmada taze log dosyasi
    if (log_fp == NULL) {
        perror("HATA: Log dosyasi acilamadi");
        return -1;
    }

    // Log dosyasinin basina oturum bilgisi yaz
    fprintf(log_fp, "========================================\n");
    fprintf(log_fp, "MAVLink Fuzzer - Log\n");
    fprintf(log_fp, "Tarih: %s\n", current_timestamp());
    fprintf(log_fp, "Hedef: 127.0.0.1:5501 (ArduPilot SITL)\n");
    fprintf(log_fp, "========================================\n\n");

    // Stdout'a da bildir
    printf("[LOGGER] Log dosyasi acildi: %s\n", filename);
    return 0;
}

void logger_log_packet(int packet_num,
                       const char *mutation_desc,
                       const uint8_t *raw_data,
                       size_t packet_len,
                       int sent_ok) {
    if (log_fp == NULL) return;

    fprintf(log_fp, "[%s] FUZZ #%d\n", current_timestamp(), packet_num);
    fprintf(log_fp, "  Mutation : %s\n", mutation_desc);
    fprintf(log_fp, "  Durum    : %s\n", sent_ok ? "GONDERILDI" : "BASARISIZ");

    // Ham byte'lari hex olarak yaz -- bu, bulgu tekrarlanabilirliginin kaniti
    // Bir "anomali" bulundugunda, bu log satirindan tam paketin byte'larini
    // alip tekrar gonderebilirsin (reproducibility).
    fprintf(log_fp, "  Hex      : ");
    for (size_t i = 0; i < packet_len; i++) {
        fprintf(log_fp, "%02x ", raw_data[i]);
    }
    fprintf(log_fp, "\n\n");

    // Her paketten sonra flush -- program crash olsa bile log kaybolmasin.
    // Bu embedded/guvensiz ortamlarda kritik bir pratik.
    fflush(log_fp);
}

void logger_log_summary(int total_sent, int total_failed) {
    if (log_fp == NULL) return;

    fprintf(log_fp, "========================================\n");
    fprintf(log_fp, "OZET - %s\n", current_timestamp());
    fprintf(log_fp, "  Toplam gonderilen : %d\n", total_sent);
    fprintf(log_fp, "  Basarisiz         : %d\n", total_failed);
    fprintf(log_fp, "  Basarili          : %d\n", total_sent - total_failed);
    fprintf(log_fp, "========================================\n");

    // Stdout'a da ozet yaz
    printf("[LOGGER] Ozet: %d gonderildi, %d basarisiz.\n",
           total_sent, total_failed);
}

void logger_close(void) {
    if (log_fp != NULL) {
        fclose(log_fp);
        log_fp = NULL;
        printf("[LOGGER] Log dosyasi kapatildi.\n");
    }
}
