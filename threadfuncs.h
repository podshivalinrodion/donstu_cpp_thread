#pragma once

#include <sys/types.h>
#include <string>
#include <mutex>
#include <fstream>

// count of threads and iterations
inline constexpr int COUNT_THREADS    = 4;
inline constexpr int COUNT_ITERATIONS = 3;

// args for thread
struct ThreadArgs {
  int         id;
  std::string tag;
};

// global log file and mutex protecting it
extern std::ofstream logFile;
extern std::mutex    logMutex;

// write line to log (thread-safe)
void writeLine(const std::string& msg);

// function for thread
void funcThread(const ThreadArgs& args);

// get system TID for current linux thread
pid_t getThreadID();

// headline of software
void about();
