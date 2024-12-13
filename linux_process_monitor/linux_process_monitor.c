#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

void display_process_info(long pid, long uptime);
void list_processes(long uptime);

// Function to get system uptime
long get_system_uptime() {
    FILE *file = fopen("/proc/uptime", "r");
    if (!file) {
        perror("fopen");
        return -1;
    }
    long uptime;
    fscanf(file, "%ld", &uptime);
    fclose(file);
    return uptime;
}

int main() {
    while (1) {
        long uptime = get_system_uptime();
        if (uptime != -1) {
            list_processes(uptime);
            sleep(5); // Update every 5 seconds
        }
    }
    return 0;
}

void list_processes(long uptime) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    printf("PID\tMemory (KB)\tCPU (%%)\n");
    printf("------------------------------\n");

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char *endptr;
            long pid = strtol(entry->d_name, &endptr, 10);
            if (*endptr == '\0') {
                // It's a valid PID
                display_process_info(pid, uptime);
            }
        }
    }

    closedir(dir);
}

void display_process_info(long pid, long uptime) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%ld/stat", pid);
    FILE *file = fopen(path, "r");
    if (!file) {
        perror("fopen");
        return;
    }

    // Read the contents of /proc/[pid]/stat
    char buffer[1024];
    long utime, stime, starttime;
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        sscanf(buffer, "%*d %*s %*c %*d %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %ld %ld %*d %*d %*d %*d %*u %*u %*d %*u %*d %*u %*u %*u %ld", &utime, &stime, &starttime);
    }
    fclose(file);

    snprintf(path, sizeof(path), "/proc/%ld/statm", pid);
    file = fopen(path, "r");
    if (!file) {
        perror("fopen");
        return;
    }

    // Read memory usage from /proc/[pid]/statm
    long mem_usage;
    if (fscanf(file, "%ld", &mem_usage) == 1) {
        mem_usage = mem_usage * getpagesize() / 1024; // Convert to KB
    }
    fclose(file);

    // Calculate CPU usage
    long total_time = utime + stime;
    long seconds = uptime - (starttime / sysconf(_SC_CLK_TCK));
    double cpu_usage = 100.0 * ((double) total_time / sysconf(_SC_CLK_TCK) / (double) seconds);

    printf("%ld\t%ld\t\t%.2f\n", pid, mem_usage, cpu_usage);
}
