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
