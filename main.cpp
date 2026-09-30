#include <iostream>
#include <vector>
#include <thread>

#include "threadfuncs.h"

int main() {
  about();

  // open global log file — defined in threadfuncs.cpp
  logFile.open("output.log", std::ios::out | std::ios::trunc);
  if (!logFile.is_open()) {
    std::cerr << "main: cannot open output.log\n";
    return 1;
  }

  std::cout << "main: pid = " << getThreadID()
            << ", opened file: 'output.log'\n";

  // args for threads
  std::vector<ThreadArgs> args = {
    {1, "First"},
    {2, "Second"},
    {3, "Third"},
    {4, "Fourth"},
  };

  // threads are starting
  std::vector<std::thread> threads;
  threads.reserve(COUNT_THREADS);

  for (int i = 0; i < COUNT_THREADS; ++i) {
    threads.emplace_back(funcThread, std::cref(args[i]));
  }

  // wait for all threads to finish
  for (auto& t : threads) {
    if (t.joinable()) t.join();
  }

  std::cout << "main: all threads finished\n";
  return 0;
}
