#include <stdio.h>
#include <unistd.h>
#include "tum.h"

int main() {
    int mem_total = 0;
    char mem_unit[16] = "";

    long USER_HZ = sysconf(_SC_CLK_TCK);

    if (mem_get_measurement("MemTotal:", &mem_total, mem_unit) != 0) {
        fprintf(stderr, "ERROR: could not get Memory Measurement!\n");
        return 1;
    }

    printf("This system has: %d %s of total memory\n", mem_total, mem_unit);
    printf("This system has a USER_HZ of: %ld Jiffies/s \n", USER_HZ);
    printf("This system has been busy with %d Jiffies in Userspace since boot\n", cpu_get_system_load(CPU_LOAD_USER));

    return 0;
}

