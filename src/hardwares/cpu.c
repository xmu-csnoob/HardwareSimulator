#include "cpu.h"

void init_cpu(CPU* cpu) {
    for (int i = 0; i < REGISTERS_NUM; i++) {
        cpu->registers[i] = 0;
    }
    cpu->pc = 0;
    cpu->ir = 0;
    cpu->zf = 0;
    cpu->sf = 0;
    cpu->state = CPU_RUNNING;
}

void execute_instruction(CPU* cpu, instruction* inst) {
    _TRACE("-------------cpu execute start-----------");
    _TRACE("pc = %d", cpu->pc);

    switch (inst->opcode) {
        case NOP:
            break;
        case LOAD:
            _TRACE("LOAD");
            cpu->registers[inst->operand1] = inst->operand2;
            break;
        case ADD:
            _TRACE("ADD");
            cpu->registers[inst->operand1] = cpu->registers[inst->operand2] + cpu->registers[inst->operand3];
            break;
        case SUB:
            _TRACE("SUB");
            cpu->registers[inst->operand1] = cpu->registers[inst->operand2] - cpu->registers[inst->operand3];
            break;
        case CMP:
            _TRACE("CMP");
            cpu->zf = (cpu->registers[inst->operand1] == cpu->registers[inst->operand2]) ? 1 : 0;
            cpu->sf = (cpu->registers[inst->operand1] < cpu->registers[inst->operand2]) ? 1 : 0;
            break;
        case JE:
            _TRACE("JE");
            if (cpu->zf == 1) {
                cpu->pc = inst->operand1;
                _TRACE("pc = %d", cpu->pc);
                return;
            }
            break;
        case JNE:
            _TRACE("JNE");
            if (cpu->zf != 1) {
                cpu->pc = inst->operand1;
                _TRACE("pc = %d", cpu->pc);
                return;
            }
            break;
        case JL:
            _TRACE("JL");
            if (cpu->sf == 1){
                cpu->pc = inst->operand1;
                _TRACE("pc = %d", cpu->pc);
                return;
            }
            break;
        case JG:
            _TRACE("JG");
            if (cpu->sf == 0){
                cpu->pc = inst->operand1;
                _TRACE("pc = %d", cpu->pc);
                return;
            }
            break;
        case JZ:
            _TRACE("JZ");
            if(cpu->zf == 1){
                cpu->pc = inst->operand1;
                _TRACE("pc = %d", cpu->pc);
                return;
            }
            break;
        case JNZ:
            _TRACE("JNZ");
            if(cpu->zf != 1){
                cpu->pc = inst->operand1;
                _TRACE("pc = %d", cpu->pc);
                return;
            }
            break;
        case JLZ:
            _TRACE("JLZ");
            if(cpu->zf == 1 && cpu->sf == 1){
                cpu->pc = inst->operand1;
                _TRACE("pc = %d", cpu->pc);
                return;
            }
            break;
        case JUMP:
            _TRACE("JUMP");
            _TRACE("pc = %d", cpu->pc);
            cpu->pc = inst->operand1;
            return;
        case HALT:
            _TRACE("HALT");
            cpu->state = CPU_HALTED;
            return;
        default:
            break;
    }
    cpu->pc++;
    _TRACE("pc = %d", cpu->pc);
    _TRACE("-------------cpu execute end-----------");
}

void print_cpu_state(CPU *cpu) {
    _TRACE("-------------CPU State Start--------------");
    _TRACE("PC: %d", cpu->pc);
    _TRACE("IR: %d", cpu->ir);
    _TRACE("ZF: %d", cpu->zf);
    _TRACE("SF: %d", cpu->sf);
    for (int i = 0; i < REGISTERS_NUM; i++) {
        _TRACE("R%d: %d", i, cpu->registers[i]);
    }
    _TRACE("State: %s", cpu->state == CPU_RUNNING ? "RUNNING" : "HALTED");
    _TRACE("-------------CPU State End--------------");
}