#ifndef BUS_H
#define BUS_H
#include "memory.h"

#define MAX_DEVICES 10

typedef struct{
    int device_id;
    int start_address;
    int end_address;
    void (*read)(int address, BYTE *buffer, int size);
    void (*write)(int address, const BYTE *buffer, int size);
} DEVICE;

typedef struct{
    DEVICE devices[MAX_DEVICES];
    int device_count;
    int address_start;
    int address_end;
} BUS;


void init_bus(BUS* bus);
void bus_add_device(BUS* bus, DEVICE* device);
void bus_read(BUS* bus, int address, BYTE *buffer, int size);
void bus_write(BUS * bus, int address, const BYTE* buffer, int size);

#endif