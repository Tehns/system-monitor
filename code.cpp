#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#ifdef _WIN32
#include <windows.h>
#include <pdh.h>
#pragma comment(lib, "pdh.lib")
#else
#include <sys/sysinfo.h>
#include <unistd.h>
#endif

void getCpuUsage() {
#ifdef _WIN32
    PDH_HQUERY cpuQuery;
    PDH_HCOUNTER cpuTotal;
    PdhOpenQuery(NULL, NULL, &cpuQuery);
    PdhAddCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
    PdhCollectQueryData(cpuQuery);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Delay to collect data
    PdhCollectQueryData(cpuQuery);

    PDH_FMT_COUNTERVALUE counterVal;
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
    std::cout << "CPU Usage: " << counterVal.doubleValue << "%" << std::endl;
    PdhCloseQuery(cpuQuery);
#else
    static unsigned long long prevTotal = 0, prevIdle = 0;
    std::ifstream cpuFile("/proc/stat");
    if (!cpuFile.is_open()) {
        std::cerr << "Failed to open /proc/stat for CPU information." << std::endl;
        return;
    }

    std::string line;
    getline(cpuFile, line);
    std::istringstream ss(line);
    std::string cpu;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal;
    unsigned long long idleTime = idle + iowait;

    unsigned long long totalDiff = total - prevTotal;
    unsigned long long idleDiff = idleTime - prevIdle;

    double usage = 100.0 * (totalDiff - idleDiff) / totalDiff;
    std::cout << "CPU Usage: " << usage << "%" << std::endl;

    prevTotal = total;
    prevIdle = idleTime;
#endif
}

void getMemoryUsage() {
#ifdef _WIN32
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memStatus)) {
        std::cout << "Memory Usage: " << memStatus.dwMemoryLoad << "%" << std::endl;
    }
    else {
        std::cerr << "Failed to retrieve memory usage information." << std::endl;
    }
#else
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        unsigned long totalMemory = info.totalram / (1024 * 1024);
        unsigned long freeMemory = info.freeram / (1024 * 1024);
        unsigned long usedMemory = totalMemory - freeMemory;
        std::cout << "Total Memory: " << totalMemory << " MB\n";
        std::cout << "Free Memory: " << freeMemory << " MB\n";
        std::cout << "Used Memory: " << usedMemory << " MB\n";
    }
    else {
        std::cerr << "Failed to retrieve memory usage information." << std::endl;
    }
#endif
}

void getTemperature() {
#ifdef _WIN32
    std::cout << "Temperature monitoring is platform-specific and not implemented for Windows in this example." << std::endl;
#else
    std::ifstream tempFile("/sys/class/thermal/thermal_zone0/temp");
    if (!tempFile.is_open()) {
        std::cerr << "Failed to open temperature sensor file." << std::endl;
        return;
    }

    double temperature;
    tempFile >> temperature;
    std::cout << "CPU Temperature: " << (temperature / 1000.0) << " °C" << std::endl;
#endif
}

void getDiskUsage() {
#ifdef _WIN32
    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
    if (GetDiskFreeSpaceEx(NULL, &freeBytesAvailable, &totalBytes, &totalFreeBytes)) {
        unsigned long long total = totalBytes.QuadPart / (1024 * 1024);
        unsigned long long free = totalFreeBytes.QuadPart / (1024 * 1024);
        unsigned long long used = total - free;
        std::cout << "Total Disk Space: " << total << " MB\n";
        std::cout << "Used Disk Space: " << used << " MB\n";
        std::cout << "Free Disk Space: " << free << " MB\n";
    }
    else {
        std::cerr << "Failed to retrieve disk usage information." << std::endl;
    }
#else
    struct statvfs stat;
    if (statvfs("/", &stat) == 0) {
        unsigned long total = (stat.f_blocks * stat.f_frsize) / (1024 * 1024);
        unsigned long free = (stat.f_bfree * stat.f_frsize) / (1024 * 1024);
        unsigned long used = total - free;
        std::cout << "Total Disk Space: " << total << " MB\n";
        std::cout << "Used Disk Space: " << used << " MB\n";
        std::cout << "Free Disk Space: " << free << " MB\n";
    }
    else {
        std::cerr << "Failed to retrieve disk usage information." << std::endl;
    }
#endif
}

void getNetworkUsage() {
#ifdef _WIN32
    std::cout << "Network monitoring is platform-specific and not implemented for Windows in this example." << std::endl;
#else
    std::ifstream netFile("/proc/net/dev");
    if (!netFile.is_open()) {
        std::cerr << "Failed to open /proc/net/dev for network information." << std::endl;
        return;
    }

    std::string line;
    while (getline(netFile, line)) {
        if (line.find(":") != std::string::npos) {
            std::cout << line << std::endl;
        }
    }
#endif
}

int main() {
    while (true) {
        std::cout << "System Monitoring Information:" << std::endl;
        std::cout << "-----------------------------" << std::endl;
        getCpuUsage();
        getMemoryUsage();
        getTemperature();
        getDiskUsage();
        getNetworkUsage();
        std::cout << "-----------------------------" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
