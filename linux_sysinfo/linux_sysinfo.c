#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <sys/sysinfo.h>

int main() {
    struct utsname sys_info;
    struct sysinfo mem_info;
    char hostname[256];

    // Get system name, release, version, and architecture
    if (uname(&sys_info) == 0) {
        printf("System Information:\n");
        printf("  System Name: %s\n", sys_info.sysname);
        printf("  Node Name: %s\n", sys_info.nodename);
        printf("  Release: %s\n", sys_info.release);
        printf("  Version: %s\n", sys_info.version);
        printf("  Machine: %s\n", sys_info.machine);
    } else {
        perror("uname");
    }

    // Get hostname
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        printf("  Hostname: %s\n", hostname);
    } else {
        perror("gethostname");
    }

    // Get uptime and memory information
    if (sysinfo(&mem_info) == 0) {
        printf("Memory and Uptime Information:\n");
        printf("  Uptime: %ld seconds\n", mem_info.uptime);
        printf("  Total RAM: %ld MB\n", mem_info.totalram / 1024 / 1024);
        printf("  Free RAM: %ld MB\n", mem_info.freeram / 1024 / 1024);
        printf("  Shared RAM: %ld MB\n", mem_info.sharedram / 1024 / 1024);
        printf("  Buffered RAM: %ld MB\n", mem_info.bufferram / 1024 / 1024);
        printf("  Total Swap: %ld MB\n", mem_info.totalswap / 1024 / 1024);
        printf("  Free Swap: %ld MB\n", mem_info.freeswap / 1024 / 1024);
        printf("  Process Count: %d\n", mem_info.procs);
    } else {
        perror("sysinfo");
    }

    // Get processor type
    FILE *cpu_info = fopen("/proc/cpuinfo", "r");
    if (cpu_info) {
        char line[256];
        printf("Processor Information:\n");
        while (fgets(line, sizeof(line), cpu_info)) {
            if (strncmp(line, "model name", 10) == 0) {
                printf("  %s", line + 13); // Skip "model name : "
                break;
            }
        }
        fclose(cpu_info);
    } else {
        perror("/proc/cpuinfo");
    }

    return 0;
}
