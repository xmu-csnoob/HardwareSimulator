#include "memory.h"
#include "cpu.h"
#include "disk.h"
#include "bus.h"
#include "msg.h"

// 注册内存设备到系统总线
void register_memory_device(BUS* bus);
// 注册磁盘设备到外设总线
void register_disk_device(BUS* bus);
// 生成磁盘文件
void generate_disks();
// 创建磁盘文件
void create_disk_file(const char *filename, const char *disk_name, size_t disk_size);
// 处理客户端请求
int wait_commands(CPU* cpu, BUS* system_bus, BUS* peripheral_bus);

int main(){
    // 初始化
    int lg = get_log_level();
    set_log_level(lg);

    // 注册磁盘
    char* disk_names[4] = {"src/hardwares/disks/disk0.disk", "src/hardwares/disks/disk1.disk", "src/hardwares/disks/disk2.disk", "src/hardwares/disks/disk3.disk"};
    for(int i = 0; i < 4; i++){
        register_disk(disk_names[i]);
    }

    CPU cpu;
    init_cpu(&cpu);
    init_memory();

    // 初始化系统总线
    BUS system_bus;
    init_bus(&system_bus);
    register_memory_device(&system_bus);
    _TRACE("Now system bus has %d devices, address range is %d -> %d", system_bus.device_count, system_bus.address_start, system_bus.address_end);

    // 初始化外设总线
    BUS peripheral_bus;
    init_bus(&peripheral_bus);
    register_disk_device(&peripheral_bus);
    _TRACE("Now peripheral bus has %d devices, address range is %d -> %d", peripheral_bus.device_count, peripheral_bus.address_start, peripheral_bus.address_end);

    for(int i = 0; i < peripheral_bus.device_count; i++){
        DEVICE device = peripheral_bus.devices[i];
        _TRACE("device %d: start from %d, end at %d", i, device.start_address, device.end_address);
    }

    // 处理客户端请求
    wait_commands(&cpu, &system_bus, &peripheral_bus);

    return 0;
}

int wait_commands(CPU* cpu, BUS* system_bus, BUS* peripheral_bus){
    int msg_id = msgget(REQUEST_QUEUE_KEY, 0666 | IPC_CREAT);
    if (msg_id != -1) {
        msgctl(msg_id, IPC_RMID, NULL);
    }

    msg_id = msgget(RESPONSE_QUEUE_KEY, 0666 | IPC_CREAT);
    if (msg_id != -1) {
        msgctl(msg_id, IPC_RMID, NULL);
    }

    int req_msg_id = msgget(REQUEST_QUEUE_KEY, IPC_CREAT | 0666);
    int res_msg_id = msgget(RESPONSE_QUEUE_KEY, IPC_CREAT | 0666);

    if (req_msg_id == -1 || res_msg_id == -1) {
        perror("msgget failed");
        exit(1);
    }

    while(1){
        request_msg req_msg;
        return_msg rtn_msg;

        if (msgrcv(req_msg_id, &req_msg, sizeof(req_msg) - sizeof(long), 0, 0) == -1) {
            perror("msgrcv failed");
            continue;
        }

        // 处理 CPU 指令消息
        if(req_msg.msg_type == CPU_MSG){
            _INFO("CPU Message: opcode is %d, operand1 is %d, operand2 is %d, operand3 is %d", req_msg.operation, req_msg.operand[0], req_msg.operand[1], req_msg.operand[2]);

            instruction i;
            i.opcode = req_msg.operation;
            i.operand1 = req_msg.operand[0];
            i.operand2 = req_msg.operand[1];
            i.operand3 = req_msg.operand[2];

            execute_instruction(cpu, &i);
            rtn_msg.code = OK;
            rtn_msg.size = 0;
        // 处理系统总线消息
        } else if(req_msg.msg_type == SYSTEM_BUS_MSG){
            _INFO("System Bus Message: address is %d, size is %d", req_msg.address, req_msg.size);

            if(req_msg.bus_op_type == BUS_READ){
                memset(rtn_msg.buffer, 0, sizeof(rtn_msg.buffer));
                req_msg.buffer[req_msg.size] = '\0';
                bus_read(peripheral_bus, req_msg.address, req_msg.buffer, req_msg.size);
                _TRACE("Read result is %s", req_msg.buffer);
                memcpy(&rtn_msg.buffer[0], req_msg.buffer, req_msg.size);
                rtn_msg.size = req_msg.size;
            } else {
                bus_write(system_bus, req_msg.address, req_msg.buffer, req_msg.size);
                rtn_msg.size = 0;
            }
            rtn_msg.code = OK;

        // 处理外设总线消息
        } else if(req_msg.msg_type == PERIPHERAL_BUS_MSG){
            _INFO("Peripheral Bus Message: address is %d, size is %d", req_msg.address, req_msg.size);

            if(req_msg.bus_op_type == BUS_READ){
                memset(rtn_msg.buffer, 0, sizeof(rtn_msg.buffer));
                req_msg.buffer[req_msg.size] = '\0';
                bus_read(peripheral_bus, req_msg.address, req_msg.buffer, req_msg.size);
                _TRACE("Read result is %s", req_msg.buffer);
                memcpy(&rtn_msg.buffer[0], req_msg.buffer, req_msg.size);
                rtn_msg.size = req_msg.size;
            } else {
                bus_write(peripheral_bus, req_msg.address, req_msg.buffer, req_msg.size);
                rtn_msg.size = 0;
            }
            rtn_msg.code = OK;
        }
        _INFO("return message : code is %s, data is %s, data_size is %d", rtn_msg.code == OK ? "OK" : "Not OK", rtn_msg.buffer, rtn_msg.size);
        // 发送返回消息
        if (msgsnd(res_msg_id, &rtn_msg, sizeof(rtn_msg) - sizeof(long), 0) == -1) {
            perror("msgsnd (return message) failed");
            continue;
        }
    }

    msgctl(req_msg_id, IPC_RMID, NULL);
    msgctl(res_msg_id, IPC_RMID, NULL);

    return 0;
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
        disk.start_address = bus->address_end;
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
