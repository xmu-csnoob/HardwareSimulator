#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
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
    unsigned char buffer[256];
}request_msg;

typedef struct{
    message_state_code code;
    unsigned char buffer[256];
    int size;
}return_msg;

int main() {
    // 打开请求队列和响应队列
    int req_msg_id = msgget(REQUEST_QUEUE_KEY, 0666 | IPC_CREAT);
    int res_msg_id = msgget(RESPONSE_QUEUE_KEY, 0666 | IPC_CREAT);

    if (req_msg_id == -1 || res_msg_id == -1) {
        perror("msgget failed");
        exit(1);
    }

    // 准备请求消息
    request_msg req_msg;
    req_msg.msg_type = PERIPHERAL_BUS_MSG;
    req_msg.bus_op_type = BUS_READ;
    req_msg.address = 0;
    req_msg.size = 1;
    memset(req_msg.buffer, 0, req_msg.size);

    // 发送请求消息
    if (msgsnd(req_msg_id, &req_msg, sizeof(req_msg) - sizeof(long), 0) == -1) {
        perror("msgsnd failed");
        exit(1);
    }

    printf("客户端发送请求消息\n");

    // 接收返回消息
    return_msg rtn_msg;
    if (msgrcv(res_msg_id, &rtn_msg, sizeof(rtn_msg) - sizeof(long), 0, 0) == -1) {
        perror("msgrcv failed");
        exit(1);
    }
    if(rtn_msg.size != 0)
        printf("客户端接收到返回消息，状态码 = %d, 数据 = %s\n", rtn_msg.code, rtn_msg.buffer);
    else
        printf("客户端接收到返回消息，状态码 = %d\n", rtn_msg.code);
    return 0;
}
