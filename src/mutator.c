#ifndef MUTATOR_H
#define MUTATOR_H

#include <stdint.h>
#include <stddef.h>  

typedef enum {
    MUTATE_BIT_FLIP,       
    MUTATE_BOUNDARY_VALUE,  
    MUTATE_LENGTH_CORRUPT, 
    MUTATE_STRATEGY_COUNT   
                             
                             
                             
} mutation_strategy_t;

void mutate_packet(uint8_t *raw_data, size_t packet_len, mutation_strategy_t strategy);


void mutate_packet_random(uint8_t *raw_data, size_t packet_len);

void mutate_packet_random_logged(uint8_t *raw_data, size_t packet_len,
                                  char *desc_buf, size_t desc_buf_size);

#endif
