#ifndef MSG_H
#define MSG_H

#include <sys/ipc.h>
#include <sys/msg.h>
#include "type.h"

#define REQUEST_QUEUE_KEY 0x1234
#define RESPONSE_QUEUE_KEY 0x5678

typedef enum{
    CPU_MSG,
    SYSTEM_BUS_MSG,
    PERIPHERAL_BUS_MSG
}message_type;

typedef enum{
    OK,
    ERROR,
    REJECTED
}message_state_code;

typedef enum{
    BUS_READ,
    BUS_WRITE
}bus_operation_type;

typedef struct{
    message_type msg_type;
    int operation;
    int* operand;
    bus_operation_type bus_op_type;
    int address;
    int size;
    BYTE buffer[256];
}request_msg;

typedef struct{
    message_state_code code;
    BYTE buffer[256];
    int size;
}return_msg;

#endif