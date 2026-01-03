#include "tum.h"
#include <stdio.h>
#include <string.h>


/*
 * get_meminfo_measurement - Read a measurement from /proc/meminfo
 *
 * - desired_measurement: Measurement name terminated with ":" (e.g. "MemTotal:")
 * - measurement_value: Output parameter for the value
 * - measurement_unit: output buffer for unit (needs 16+ bytes)
 *
 * Returns: 0 on success
 *         -1 if file can't be opened
 *          1 if measurement is not found
 */
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

/*
 * get_cpu_load_system - reads the total (summed over all cpu cores) system load from /proc/stat
 *
 * cpu_stat_identifier: the specific stat to be retrieved, one of:
 *     - CPU_LOAD_USER
 *     - CPU_LOAD_NICE
 *     - CPU_LOAD_SYSTEM
 *     - CPU_LOAD_IDLE
 *     - CPU_LOAD_IOWAIT
 *     - CPU_LOAD_IRQ
 *     - CPU_LOAD_SOFTIRQ
 *     - CPU_LOAD_STEAL
 *     - CPU_LOAD_GUEST
 *     - CPU_LOAD_GUEST_NICE
 *
 * Returns: the value of the requested stat or -1 if an error occurs
 */
int cpu_get_system_load(int cpu_stat_identifier) {

    if ((cpu_stat_identifier < 0) || (cpu_stat_identifier > 9)) {
        fprintf(stderr, "ERROR: requested cpu_stat_identifier is undefined!\n");
        return -1;
    }

    int cpu_stat_read_values[10];
    char cpu_string[4];

    char line_read[READ_BUFFER_SIZE];
    FILE *cpu_stat = fopen(PROC_STAT, "r");

    if (cpu_stat == NULL) {
        fprintf(stderr, "ERROR: can't open %s\n", PROC_STAT);
        return -1;
    }

    if (fgets(line_read, READ_BUFFER_SIZE, cpu_stat) == NULL) {
        fprintf(stderr, "ERROR: %s is unreadable or empty!\n", PROC_STAT);
        fclose(cpu_stat);
        return -1;
    }

    if (11 != sscanf(line_read, "%3s %d %d %d %d %d %d %d %d %d %d", cpu_string,
                     &cpu_stat_read_values[0], &cpu_stat_read_values[1],
                     &cpu_stat_read_values[2], &cpu_stat_read_values[3],
                     &cpu_stat_read_values[4], &cpu_stat_read_values[5],
                     &cpu_stat_read_values[6], &cpu_stat_read_values[7],
                     &cpu_stat_read_values[8], &cpu_stat_read_values[9])) {
      fprintf(stderr, "ERROR: could not parse cpu stat information retrieved from the kernel!\n");
      fclose(cpu_stat);
      return -1;
    }

    fclose(cpu_stat);
    return cpu_stat_read_values[cpu_stat_identifier];
}
