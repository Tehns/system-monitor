#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/sysinfo.h>
#include <unistd.h>
#endif

void getCpuUsage() {
#ifdef _WIN32
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        std::cout << "CPU usage information is platform-specific for Windows." << std::endl;
    }
    else {
        std::cerr << "Failed to retrieve CPU usage information." << std::endl;
    }
#else
    std::ifstream cpuFile("/proc/stat");
    if (!cpuFile.is_open()) {
        std::cerr << "Failed to open /proc/stat for CPU information." << std::endl;
        return;
    }

    std::string line;
    getline(cpuFile, line);
    std::istringstream ss(line);
    std::string cpu;
    unsigned long user, nice, system, idle;
    ss >> cpu >> user >> nice >> system >> idle;

    unsigned long total = user + nice + system + idle;
    unsigned long usage = total - idle;

    std::cout << "CPU Usage: " << (usage * 100.0 / total) << "%" << std::endl;
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
        std::cout << "Total Memory: " << totalMemory << " MB\n";
        std::cout << "Free Memory: " << freeMemory << " MB\n";
        std::cout << "Used Memory: " << (totalMemory - freeMemory) << " MB\n";
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

int main() {
    while (true) {
        std::cout << "System Monitoring Information:" << std::endl;
        std::cout << "-----------------------------" << std::endl;
        getCpuUsage();
        getMemoryUsage();
        getTemperature();
        std::cout << "-----------------------------" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
