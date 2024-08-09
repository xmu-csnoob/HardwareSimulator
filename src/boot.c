#include "memory.h"
#include "cpu.h"
#include "disk.h"
#include "bus.h"

void register_memory_device(BUS* bus);
void register_disk_device(BUS* bus);
void generate_disks();
void create_disk_file(const char *filename, const char *disk_name, size_t disk_size);

int main(){
    // generate_disks();
    char* disk_names[4] = {"src/hardwares/disks/disk0.disk", "src/hardwares/disks/disk1.disk", "src/hardwares/disks/disk2.disk", "src/hardwares/disks/disk3.disk"};
    for(int i = 0; i < 4; i++){
        register_disk(disk_names[i]);
    }
    CPU cpu;
    init_cpu(&cpu);
    init_memory();
    BUS bus;
    init_bus(&bus);
    register_memory_device(&bus);
    _TRACE("Now bus has %d devices, address range is %d -> %d", bus.device_count, bus.address_start, bus.address_end);
    register_disk_device(&bus);
    _TRACE("Now bus has %d devices, address range is %d -> %d", bus.device_count, bus.address_start, bus.address_end);

    
}

void register_memory_device(BUS* bus){
    DEVICE memory;
    memory.start_address = bus->address_end;
    memory.end_address = memory.start_address + MEMORY_SIZE - 1;
    memory.read = read_memory;
    memory.write = write_memory;
    bus_add_device(bus, &memory);
}

void register_disk_device(BUS* bus){
    for(int i = 0; i < MAX_DISKS; i++){
        disk* d = &physical_disks[i];
        DEVICE disk;
        disk.start_address = bus->address_end + 1;
        disk.end_address = disk.start_address + d->size - 1;
        disk.read = read_disk;
        disk.write = write_disk;
        bus_add_device(bus, &disk);
    }
}

void create_disk_file(const char *filename, const char *disk_name, size_t disk_size) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to create file");
        return;
    }
    // 写入磁盘元数据
    fprintf(file, "Disk Name: %s\n", disk_name);
    fprintf(file, "Disk Size: %zu bytes\n", disk_size);
    // 写入二进制数据
    for (size_t i = 0; i < disk_size; i++) {
        fprintf(file, "00 "); // 每个字节用 "00 " 表示
        if ((i + 1) % 16 == 0) {
            fprintf(file, "\n"); // 每行 16 个字节
        }
    }
    fclose(file);
    _INFO("Disk file '%s' created successfully.", filename);
}

void generate_disks(){
    create_disk_file("src/hardwares/disks/disk0.disk", "Samsung 990 pro", 256);
    create_disk_file("src/hardwares/disks/disk1.disk", "Samsung 990 pro", 512);
    create_disk_file("src/hardwares/disks/disk2.disk", "Samsung 990 pro", 1024);
    create_disk_file("src/hardwares/disks/disk3.disk", "Samsung 990 pro", 2048);
}