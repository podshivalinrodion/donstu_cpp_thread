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

// shared counter — атомарный (задание 20)
std::atomic<int> counter{0};

bool writeLine(const std::string& msg) {
  std::lock_guard<std::mutex> lock(logMutex);
  logFile << msg;
  logFile.flush();
  return static_cast<bool>(logFile);
}

pid_t getThreadID() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

void about() {
  std::cout << "std::thread example\n";
}

void funcThread(const ThreadArgs& args) {
  // каждый поток накручивает счётчик 100000 раз
  for (int i = 0; i < 100000; ++i) {
    ++counter;
  }

  for (int i = 0; i < COUNT_ITERATIONS; ++i) {
    std::ostringstream oss;

    oss << "[tag = " << args.tag
        << "] pid = "  << ::getpid()
        << " ppid = "  << ::getppid()
        << " tid = "   << getThreadID()
        << " std_id = " << std::this_thread::get_id()
        << " iter = "  << i
        << "\n";

    writeLine(oss.str());

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void funcThreadWithResult(const ThreadArgs& args,
                          std::promise<std::string> prom) {
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

  // возвращаем результат через promise
  std::ostringstream result;
  result << "thread " << args.tag
         << " finished " << COUNT_ITERATIONS << " iterations";
  prom.set_value(result.str());
}
