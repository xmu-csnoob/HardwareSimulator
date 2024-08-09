#include <string.h>  // 为了使用 memcpy

#define MEMORY_SIZE 1024  // 示例内存大小
typedef unsigned char BYTE;

BYTE memory[MEMORY_SIZE];  // 模拟的内存空间

void init_memory() {
    memset(memory, 0x00, MEMORY_SIZE);  // 将内存初始化为 0
}

void read_memory(int address, BYTE* buffer, int size) {
    if (address < 0 || address + size > MEMORY_SIZE) {
        return;
    }
    memcpy(buffer, &memory[address], size);  // 从内存复制到 buffer
}

void write_memory(int address, const BYTE *buffer, int size) {
    if (address < 0 || address + size > MEMORY_SIZE) {
        return;  // 地址越界处理
    }
    memcpy(&memory[address], buffer, size);  // 从 buffer 复制到内存
}
