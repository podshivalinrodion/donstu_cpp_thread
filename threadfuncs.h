#pragma once

#include <sys/types.h>
#include <string>
#include <mutex>
#include <fstream>
#include <atomic>
#include <future>
#include <condition_variable>

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

// shared counter — атомарный (задание 20)
extern std::atomic<int> counter;

// --- Задание 21: производитель-потребитель ---
extern std::mutex              pcMutex;
extern std::condition_variable pcCv;
extern int                     sharedValue;
extern bool                    ready;
extern bool                    done;

// write line to log (thread-safe)
bool writeLine(const std::string& msg);

// function for thread
void funcThread(const ThreadArgs& args);

// function for thread that returns result via promise
void funcThreadWithResult(const ThreadArgs& args,
                          std::promise<std::string> prom);

// producer-consumer threads
void producerThread();
void consumerThread();

// get system TID for current linux thread
pid_t getThreadID();

// headline of software
void about();
