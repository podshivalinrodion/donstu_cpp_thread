#include <sstream>
#include <iostream>
#include <vector>
#include <thread>

#include "threadfuncs.h"

int main() {
  about();

  logFile.open("output.log", std::ios::out | std::ios::trunc);
  if (!logFile.is_open()) {
    std::cerr << "main: cannot open output.log\n";
    return 1;
  }

  {
    std::ostringstream oss;
    oss << "main: pid = " << getThreadID()
        << ", opened file: 'output.log'\n";
    if (!writeLine(oss.str())) {
      std::cerr << "main: failed to write to log\n";
    }
  }

  std::vector<ThreadArgs> args(COUNT_THREADS);
  for (int i = 0; i < COUNT_THREADS; ++i) {
    std::ostringstream oss;
    oss << "T" << i;
    args[i].id  = i;
    args[i].tag = oss.str();
  }

  std::vector<std::thread> threads;
  threads.reserve(COUNT_THREADS);

  for (int i = 0; i < COUNT_THREADS; ++i) {
    threads.emplace_back(funcThread, std::cref(args[i]));
  }

  for (auto& t : threads) {
    if (t.joinable()) t.join();
  }

  std::cout << "counter = " << counter
            << " (expected " << COUNT_THREADS * 100000 << ")\n";

  if (!writeLine("main: all threads finished\n")) {
    std::cerr << "main: failed to write final line\n";
  }

  return 0;
}
