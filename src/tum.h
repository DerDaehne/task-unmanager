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

int mem_get_measurement(char *desired_measurement, int *measurement_value, char *measurement_unit);
int cpu_get_system_load(int cpu_stat_identifier);

#endif // TUM_H_
