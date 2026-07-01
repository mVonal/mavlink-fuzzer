#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "mavlink_types.h"
#include "mutator.h"
#include "sender.h"
#include "logger.h"

int parse_mavlink_header(const uint8_t *raw_data, mavlink_v2_header_t *out_header);
uint32_t get_message_id(const mavlink_v2_header_t *header);
void print_header(const mavlink_v2_header_t *header);
char g_last_mutation_desc[256];

int main() {
    srand(time(NULL));
    uint8_t seed_packet[] = {
        0xfd, 0x09, 0x00, 0x00, 0xe4, 0x01, 0x01,
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x51, 0x03, 0x96, 0x43
    };
    size_t packet_len = sizeof(seed_packet);
    int fuzz_count = 50; 
    int total_failed = 0;

    printf("=== MAVLink Fuzzer v0.1 ===\n");
    printf("Hedef : %s:%d\n", SITL_IP, SITL_PORT);
    printf("Seed  : %zu byte HEARTBEAT\n", packet_len);
    printf("Sayi  : %d paket\n\n", fuzz_count);

    
    if (logger_init(LOG_FILE) < 0) {
        return 1;
    }

    
    int sock = sender_init(SITL_IP, SITL_PORT);
    if (sock < 0) {
        logger_close();
        return 1;
    }

    
    printf("--- Orijinal paket gonderiliyor ---\n");
    int sent = sender_send(sock, seed_packet, packet_len);
    logger_log_packet(0, "ORIJINAL (seed)", seed_packet, packet_len, sent > 0);
    if (sent > 0) printf("Orijinal paket gonderildi.\n\n");

    usleep(300000);

    
    uint8_t mutated[sizeof(seed_packet)];
    mavlink_v2_header_t header;

    for (int i = 1; i <= fuzz_count; i++) {
        memcpy(mutated, seed_packet, packet_len);
        mutate_packet_random_logged(mutated, packet_len, g_last_mutation_desc,
                                    sizeof(g_last_mutation_desc));

        printf("--- FUZZ #%d | %s ---\n", i, g_last_mutation_desc);
        
        if (parse_mavlink_header(mutated, &header) == 0) {
            print_header(&header);
        } else {
            printf("(Magic byte bozuldu, parser reddetti)\n");
            strncat(g_last_mutation_desc, " [MAGIC BOZULDU]",
                    sizeof(g_last_mutation_desc) - strlen(g_last_mutation_desc) - 1);
        }

        sent = sender_send(sock, mutated, packet_len);
        int success = (sent > 0);
        if (!success) total_failed++;

        printf("Durum: %s\n\n", success ? "GONDERILDI" : "BASARISIZ");

        
        logger_log_packet(i, g_last_mutation_desc, mutated, packet_len, success);

        usleep(100000);  
    }

    
    logger_log_summary(fuzz_count, total_failed);
    printf("=== Fuzzing tamamlandi. %d/%d basarili. ===\n",
           fuzz_count - total_failed, fuzz_count);

    sender_close(sock);
    logger_close();

    return 0;
}
