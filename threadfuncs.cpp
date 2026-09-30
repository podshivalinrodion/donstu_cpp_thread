#include "threadfuncs.h"

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/syscall.h>
#include <chrono>
#include <thread>

// global log file and mutex — определены ровно один раз
std::ofstream logFile;
std::mutex    logMutex;

void writeLine(const std::string& msg) {
  std::lock_guard<std::mutex> lock(logMutex);
  logFile << msg;
  logFile.flush();
}

pid_t getThreadID() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

void about() {
  std::cout << "std::thread example\n";
}

void funcThread(const ThreadArgs& args) {
  for (int i = 0; i < COUNT_ITERATIONS; ++i) {
    std::ostringstream oss;

    oss << "[tag = " << args.tag
        << "] pid = "  << ::getpid()
        << " ppid = "  << ::getppid()
        << " tid = "   << getThreadID()
        << " iter = "  << i
        << "\n";

    writeLine(oss.str());

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
