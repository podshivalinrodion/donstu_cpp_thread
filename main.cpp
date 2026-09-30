#include <sstream>
#include <iostream>
#include <vector>
#include <thread>
#include <future>

#include "threadfuncs.h"

int main() {
  about();

  // open global log file — defined in threadfuncs.cpp
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

  // args for threads
  std::vector<ThreadArgs> args(COUNT_THREADS);
  for (int i = 0; i < COUNT_THREADS; ++i) {
    std::ostringstream oss;
    oss << "T" << i;
    args[i].id  = i;
    args[i].tag = oss.str();
  }

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

  std::cout << "counter = " << counter
            << " (expected " << COUNT_THREADS * 100000 << ")\n";

  // Задание 14/16: возврат значения из потока через promise/future
  std::promise<std::string> prom;
  std::future<std::string>  fut = prom.get_future();

  ThreadArgs extraArgs;
  extraArgs.id  = COUNT_THREADS;
  extraArgs.tag = "ResultThread";

  std::thread tResult(funcThreadWithResult,
                      std::cref(extraArgs),
                      std::move(prom));

  std::string result = fut.get();
  tResult.join();

  std::cout << "main: got from thread: " << result << "\n";

  // Задание 21: производитель-потребитель
  std::thread producer(producerThread);
  std::thread consumer(consumerThread);

  producer.join();
  consumer.join();

  std::cout << "main: producer-consumer finished\n";

  if (!writeLine("main: all threads finished\n")) {
    std::cerr << "main: failed to write final line\n";
  }

  return 0;
}
