#ifndef MEMORY_H
#define MEMORY_H
#define MEMORY_SIZE 1024
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "type.h"

extern BYTE memory[MEMORY_SIZE];

#define MEMORY_START_ADDRESS 0
#define MEMORY_END_ADDRESS MEMORY_SIZE - 1

void init_memory();

void read_memory(int address, BYTE* buffer, int size);

void write_memory(int address, const BYTE *buffer, int size);

#endif