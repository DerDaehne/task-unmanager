#include <stdio.h>

#define READ_BUFFER_SIZE 256
#define MEMINFO_FILE_PATH "/proc/meminfo"

int main() {
    char line_read[READ_BUFFER_SIZE];
    FILE *meminfo = fopen(MEMINFO_FILE_PATH, "r");

    if (meminfo == NULL) {
        printf("ERROR: can't open %s", MEMINFO_FILE_PATH);
        return 1;
    }


    while (fgets(line_read, READ_BUFFER_SIZE, meminfo)) {
        printf("Read %d Characters from meminfo:\n %s", READ_BUFFER_SIZE, line_read);
    }

    fclose(meminfo);
    return 0;
}
