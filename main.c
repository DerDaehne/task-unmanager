#include <stdio.h>
#include <string.h>

#define READ_BUFFER_SIZE 256
#define PROC_MEMINFO "/proc/meminfo"

int get_meminfo_measurement(char *desired_measurement, int *measurement_value, char *measurement_unit);

int main() {
    int mem_total = 0;
    char mem_unit[16];

    if (get_meminfo_measurement("MemTotal:", &mem_total, mem_unit) != 0) {
        fprintf(stderr, "ERROR: could not get Memory Measurement!\n");
        return 1;
    }

    printf("This system has: %d %s of total memory\n", mem_total, mem_unit);

    return 0;
}

/*
 * get_meminfo_measurement - Read a measurement from /proc/meminfo
 *
 * desired_measurement: Measurement name terminated with ":" (e.g. "MemTotal:")
 * measurement_value: Output parameter for the value
 * measurement_unit: output buffer for unit (needs 16+ bytes)
 *
 * Returns: 0 on success
 *         -1 if file can't be opened
 *          1 if measurement is not found
 */
int get_meminfo_measurement(char *desired_measurement, int *measurement_value, char *measurement_unit) {
    char line_read[READ_BUFFER_SIZE];
    FILE *meminfo = fopen(PROC_MEMINFO, "r");

    if (meminfo == NULL) {
        fprintf(stderr, "ERROR: can't open %s\n", PROC_MEMINFO);
        return -1;
    }

    char meminfo_measurement[64];
    int meminfo_value = 0;
    char meminfo_unit[16];

    while (fgets(line_read, READ_BUFFER_SIZE, meminfo)) {
        if (sscanf(line_read, "%63s %d %15s", meminfo_measurement,  &meminfo_value, meminfo_unit ) < 3) {
            continue;
        }

        if (!strcmp(desired_measurement, meminfo_measurement)) {
            *measurement_value = meminfo_value;
            strcpy(measurement_unit, meminfo_unit);
            fclose(meminfo);
            return 0;
        }
    }

    fclose(meminfo);
    return 1;

}
