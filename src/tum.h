#ifndef TUM_H_
#define TUM_H_

#define READ_BUFFER_SIZE 256
#define PROC_MEMINFO "/proc/meminfo"
#define PROC_STAT "/proc/stat"

#define CPU_LOAD_USER 0
#define CPU_LOAD_NICE 1
#define CPU_LOAD_SYSTEM 2
#define CPU_LOAD_IDLE 3
#define CPU_LOAD_IOWAIT 4
#define CPU_LOAD_IRQ 5
#define CPU_LOAD_SOFTIRQ 6
#define CPU_LOAD_STEAL 7
#define CPU_LOAD_GUEST 8
#define CPU_LOAD_GUEST_NICE 9

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
int mem_get_measurement(char *desired_measurement, int *measurement_value, char *measurement_unit);

/* cpu_get_raw_stats - reads from /proc/stat and returns the values corresponding to the given cpu identifier
 *
 * cpu_identifier: the name of the cpu core to fetch stats from(e.g. "cpu0", "cpu")
 *
 * returns: a pointer to an int array with the length of 10, containing all available stats (see "man proc_stat" for more info)
 */
int* cpu_get_raw_stats(char *cpu_identifier);


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
 * returns: the value of the requested stat or -1 if an error occurs
 */
int cpu_get_system_load(int cpu_stat_identifier);

/* cpu_get_total_time - returns the sum of time, measured in USER_HZ, that the cpu has spent in total since boot
 *
 * returns: int
 */
int cpu_get_total_time();

/* cpu_get_busy_time - returns the amount of time, measured in USER_HZ, that the cpu has spent in the busy state in total since boot
 *
 * returns: int
 */
int cpu_get_busy_time();

#endif // TUM_H_

#ifdef TUM_IMPLEMENTATION

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
    char cpu_identifier_from_proc[8];

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
      if (sscanf(line_read, "%7s %d %d %d %d %d %d %d %d %d %d",
                 cpu_identifier_from_proc, &cpu_stat_extracted_values[0],
                 &cpu_stat_extracted_values[1], &cpu_stat_extracted_values[2],
                 &cpu_stat_extracted_values[3], &cpu_stat_extracted_values[4],
                 &cpu_stat_extracted_values[5], &cpu_stat_extracted_values[6],
                 &cpu_stat_extracted_values[7], &cpu_stat_extracted_values[8],
                 &cpu_stat_extracted_values[9]) != 11) {
        fprintf(stderr, "ERROR: could not parse cpu stat information retrieved "
                        "from the kernel!\n");
        fclose(cpu_stat);
        return NULL;
      }

        if (!strcmp(cpu_identifier, cpu_identifier_from_proc)) {
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

#endif
