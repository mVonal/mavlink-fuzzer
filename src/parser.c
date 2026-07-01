#include <stdio.h>
#include <string.h>
#include "mavlink_types.h"


int parse_mavlink_header(const uint8_t *raw_data, mavlink_v2_header_t *out_header) {

   
    if (raw_data[0] != MAVLINK_V2_MAGIC) {
        printf("HATA: Magic byte uyusmuyor. Beklenen: 0x%02X, Gelen: 0x%02X\n",
               MAVLINK_V2_MAGIC, raw_data[0]);
        return -1;
    }


    memcpy(out_header, raw_data, MAVLINK_V2_HEADER_SIZE);

    return 0;
}


uint32_t get_message_id(const mavlink_v2_header_t *header) {
    return header->msg_id[0]
         | (header->msg_id[1] << 8)
         | (header->msg_id[2] << 16);
}

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
