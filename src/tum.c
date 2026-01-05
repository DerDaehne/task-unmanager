#include "tum.h"
#include <stdio.h>
#include <string.h>


int mem_get_measurement(char *desired_measurement, int *measurement_value, char *measurement_unit) {
    char line_read[READ_BUFFER_SIZE];
    FILE *meminfo = fopen(PROC_MEMINFO, "r");

    if (meminfo == NULL) {
        fprintf(stderr, "ERROR: can't open %s\n", PROC_MEMINFO);
        return -1;
    }

    char meminfo_measurement[64] = "";
    int meminfo_value = 0;
    char meminfo_unit[16] = "";

    while (fgets(line_read, READ_BUFFER_SIZE, meminfo)) {
        if (sscanf(line_read, "%63s %d %15s", meminfo_measurement,  &meminfo_value, meminfo_unit ) < 3) {
            continue;
        }

        if (!strcmp(desired_measurement, meminfo_measurement)) {
            *measurement_value = meminfo_value;
            strncpy(measurement_unit, meminfo_unit, 16);
            fclose(meminfo);
            return 0;
        }
    }

    fclose(meminfo);
    return 1;

}

int* cpu_get_raw_stats(char *cpu_identifier) {
    static int cpu_stat_extracted_values[10];
    char cpu_identifier_string_read[8];

    char line_read[READ_BUFFER_SIZE];
    FILE *cpu_stat = fopen(PROC_STAT, "r");

    if (cpu_stat == NULL) {
        fprintf(stderr, "ERROR: can't open %s\n", PROC_STAT);
        return NULL;
    }

    if (fgets(line_read, READ_BUFFER_SIZE, cpu_stat) == NULL) {
        fprintf(stderr, "ERROR: %s is unreadable or empty!\n", PROC_STAT);
        fclose(cpu_stat);
        return NULL;
    }

    do {
        if (11 != sscanf(line_read, "%7s %d %d %d %d %d %d %d %d %d %d", cpu_identifier_string_read,
                        &cpu_stat_extracted_values[0], &cpu_stat_extracted_values[1],
                        &cpu_stat_extracted_values[2], &cpu_stat_extracted_values[3],
                        &cpu_stat_extracted_values[4], &cpu_stat_extracted_values[5],
                        &cpu_stat_extracted_values[6], &cpu_stat_extracted_values[7],
                        &cpu_stat_extracted_values[8], &cpu_stat_extracted_values[9])) {
            fprintf(stderr, "ERROR: could not parse cpu stat information retrieved from the kernel!\n");
            fclose(cpu_stat);
            return NULL;
        }

        if (!strcmp(cpu_identifier, cpu_identifier_string_read)) {
            break;
        }
    }while (fgets(line_read, READ_BUFFER_SIZE, cpu_stat));


    fclose(cpu_stat);
    return cpu_stat_extracted_values;

}

int cpu_get_system_load(int cpu_stat_identifier) {

    if ((cpu_stat_identifier < 0) || (cpu_stat_identifier > 9)) {
        fprintf(stderr, "ERROR: requested cpu_stat_identifier is undefined!\n");
        return -1;
    }

    int *cpu_load = cpu_get_raw_stats("cpu");

    return cpu_load[cpu_stat_identifier];
}


int cpu_get_total_time() {

    int *cpu_stat_read_values = cpu_get_raw_stats("cpu");

    int load_sum = 0;
    for (int i = 0; i < 10; i++) {
        load_sum += cpu_stat_read_values[i];

    }
    return load_sum;
}

int cpu_get_busy_time() {
    int *cpu_stat_read_values = cpu_get_raw_stats("cpu");

    int busy_sum = cpu_stat_read_values[CPU_LOAD_USER] +
                   cpu_stat_read_values[CPU_LOAD_NICE] +
                   cpu_stat_read_values[CPU_LOAD_SYSTEM] +
                   cpu_stat_read_values[CPU_LOAD_IRQ] +
                   cpu_stat_read_values[CPU_LOAD_SOFTIRQ] +
                   cpu_stat_read_values[CPU_LOAD_STEAL] +
                   cpu_stat_read_values[CPU_LOAD_GUEST] +
                   cpu_stat_read_values[CPU_LOAD_GUEST_NICE];

    return busy_sum;
}
