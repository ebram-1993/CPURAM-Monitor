#define _WIN32_WINNT 0x0601
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include "httplib.h"
#include <string>
#include <thread>
#include <chrono>

// ---- CPU usage helpers ----
ULONGLONG filetime_to_ull(const FILETIME& ft) {
    return (((ULONGLONG)ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
}

float get_cpu_usage() {
    static ULONGLONG last_idle_time = 0;
    static ULONGLONG last_kernel_time = 0;
    static ULONGLONG last_user_time = 0;

    FILETIME idle_time, kernel_time, user_time;
    GetSystemTimes(&idle_time, &kernel_time, &user_time);

    ULONGLONG curr_idle = filetime_to_ull(idle_time);
    ULONGLONG curr_kernel = filetime_to_ull(kernel_time);
    ULONGLONG curr_user = filetime_to_ull(user_time);

    ULONGLONG sys_idle = curr_idle - last_idle_time;
    ULONGLONG sys_kernel = curr_kernel - last_kernel_time;
    ULONGLONG sys_user = curr_user - last_user_time;
    ULONGLONG sys_total = sys_kernel + sys_user;

    last_idle_time = curr_idle;
    last_kernel_time = curr_kernel;
    last_user_time = curr_user;

    if (sys_total == 0) return 0.0f;

    float usage = 100.0f * (1.0f - ((float)sys_idle / sys_total));
    return usage;
}

// ---- Memory usage helper ----
float get_mem_usage() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(memInfo);
    GlobalMemoryStatusEx(&memInfo);

    DWORDLONG total = memInfo.ullTotalPhys;
    DWORDLONG free = memInfo.ullAvailPhys;

    return float(100.0 * (total - free) / total);
}

// ---- Main server ----
int main() {
    httplib::Server svr;

    svr.Get("/cpu", [](const httplib::Request&, httplib::Response& res) {
        float cpu = get_cpu_usage();
        res.set_content("{\"cpu_usage\":" + std::to_string(cpu) + "}", "application/json");
        });

    svr.Get("/memory", [](const httplib::Request&, httplib::Response& res) {
        float mem = get_mem_usage();
        res.set_content("{\"mem_usage\":" + std::to_string(mem) + "}", "application/json");
        });

    std::cout << "Server started on http://localhost:8080\n";
    svr.listen("localhost", 8080);
}