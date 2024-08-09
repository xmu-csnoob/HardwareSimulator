#include "bus.h"


void init_bus(BUS* bus){
    bus->device_count = 0;
    bus->address_start = 0;
    bus->address_end = 0;
}

void bus_add_device(BUS *bus, DEVICE *device) {
    bus->devices[bus->device_count++] = *device;
    bus->address_end = device->end_address;
}
void bus_read(BUS *bus, int address, BYTE *buffer, int size) {
    for (int i = 0; i < bus->device_count; ++i) {
        DEVICE *device = &bus->devices[i];
        if (address >= device->start_address && address <= device->end_address) {
            int offset = address - device->start_address;
            device->read(offset, buffer, size);
            return;
        }
    }
    printf("No device found at address %x\n", address);
}

void bus_write(BUS *bus, int address, const BYTE *buffer, int size) {
    for (int i = 0; i < bus->device_count; ++i) {
        DEVICE *device = &bus->devices[i];
        if (address >= device->start_address && address <= device->end_address) {
            int offset = address - device->start_address;
            device->write(offset, buffer, size);
            return;
        }
    }
    printf("No device found at address %x\n", address);
}