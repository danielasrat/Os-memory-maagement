#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 128

typedef struct {
    int id;
    int size;
} Process;

typedef struct {
    int memory[MEMORY_SIZE];
    int free[MEMORY_SIZE][2];
    int free_count;
    int firstIndex[MEMORY_SIZE];
} Memory;

void initializeMemory(Memory *mem) {
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        mem->memory[i] = -1;
    }
    mem->free[0][0] = 0;
    mem->free[0][1] = MEMORY_SIZE;
    mem->free_count = 1;
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        mem->firstIndex[i] = -1;
    }
}

void allocate(Memory *mem, Process *process) {
    if (mem->firstIndex[process->id] != -1) {
        printf("Process with id %d is already in the memory!\n", process->id);
        return;
    }
    for (int i = 0; i < mem->free_count; ++i) {
        int dif = mem->free[i][1] - mem->free[i][0];
        if (dif >= process->size) {
            mem->firstIndex[process->id] = mem->free[i][0];
            int initial = mem->free[i][0];
            mem->free[i][0] += process->size;
            if (mem->free[i][0] == mem->free[i][1]) {
                for (int j = i; j < mem->free_count - 1; ++j) {
                    mem->free[j][0] = mem->free[j + 1][0];
                    mem->free[j][1] = mem->free[j + 1][1];
                }
                --mem->free_count;
            }
            for (int j = initial; j < initial + process->size; ++j) {
                mem->memory[j] = process->id;
            }
            return;
        }
    }
    printf("Not enough space\n");
}

void removeProcess(Memory *mem, Process *process) {
    if (mem->firstIndex[process->id] == -1) {
        printf("Process %d not found\n", process->id);
        return;
    }
    int start = mem->firstIndex[process->id];
    int end = start + process->size;

    for (int i = 0; i < process->size; ++i) {
        mem->memory[start + i] = -1;
    }

    mem->firstIndex[process->id] = -1;

    int merged = 0;
    for (int i = 0; i < mem->free_count; ++i) {
        if (mem->free[i][1] == start) {
            mem->free[i][1] = end;
            merged = 1;
            if (i < mem->free_count - 1 && mem->free[i + 1][0] == end) {
                mem->free[i][1] = mem->free[i + 1][1];
                for (int j = i + 1; j < mem->free_count - 1; ++j) {
                    mem->free[j][0] = mem->free[j + 1][0];
                    mem->free[j][1] = mem->free[j + 1][1];
                }
                --mem->free_count;
            }
            break;
        }
    }
    if (!merged) {
        for (int i = 0; i < mem->free_count; ++i) {
            if (mem->free[i][0] == end) {
                mem->free[i][0] = start;
                merged = 1;
                break;
            }
        }
    }
    if (!merged) {
        mem->free[mem->free_count][0] = start;
        mem->free[mem->free_count][1] = end;
        ++mem->free_count;
    }
    for (int i = 0; i < mem->free_count - 1; ++i) {
        for (int j = i + 1; j < mem->free_count; ++j) {
            if (mem->free[i][0] > mem->free[j][0]) {
                int temp0 = mem->free[i][0];
                int temp1 = mem->free[i][1];
                mem->free[i][0] = mem->free[j][0];
                mem->free[i][1] = mem->free[j][1];
                mem->free[j][0] = temp0;
                mem->free[j][1] = temp1;
            }
        }
    }
}

int main() {
    Memory memory;
    initializeMemory(&memory);

    Process p1 = {1, 5};
    Process p2 = {2, 5};
    Process p3 = {3, 15};
    Process p4 = {4, 15};
    Process p5 = {5, 3};

    allocate(&memory, &p1);
    allocate(&memory, &p3);
    removeProcess(&memory, &p1);
    removeProcess(&memory, &p2);
    allocate(&memory, &p4);
    allocate(&memory, &p5);

    printf("Memory:\n");
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        printf("%d ", memory.memory[i]);
    }
    printf("\nFree spaces:\n");
    for (int i = 0; i < memory.free_count; ++i) {
        printf("[%d, %d] ", memory.free[i][0], memory.free[i][1]);
    }
    printf("\nFirst Index:\n");
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        if (memory.firstIndex[i] != -1) {
            printf("%d: %d ", i, memory.firstIndex[i]);
        }
    }
    printf("\n");

    return 0;
}

