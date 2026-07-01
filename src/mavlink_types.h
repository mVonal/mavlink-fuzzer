#ifndef MAVLINK_TYPES_H
#define MAVLINK_TYPES_H

#include <stdint.h>   


#define MAVLINK_V2_MAGIC 0xFD

typedef struct __attribute__((packed)) {
    uint8_t  magic;          
    uint8_t  len;             
    uint8_t  incompat_flags;  
    uint8_t  compat_flags;   
    uint8_t  seq;             
    uint8_t  sys_id;          
    uint8_t  comp_id;        
    uint8_t  msg_id[3];       
} mavlink_v2_header_t;

#define MAVLINK_V2_HEADER_SIZE 10

#endif
