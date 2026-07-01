#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <stddef.h>
#include "mavlink_types.h"


#define LOG_FILE "fuzzer_results.log"


int logger_init(const char *filename);


void logger_log_packet(int packet_num,
                       const char *mutation_desc,
                       const uint8_t *raw_data,
                       size_t packet_len,
                       int sent_ok);


void logger_log_summary(int total_sent, int total_failed);


void logger_close(void);

#endif 
