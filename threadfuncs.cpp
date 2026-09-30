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

// --- Задание 21: производитель-потребитель ---
std::mutex              pcMutex;
std::condition_variable pcCv;
int                     sharedValue = 0;
bool                    ready = false;
bool                    done  = false;

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

  std::ostringstream result;
  result << "thread " << args.tag
         << " finished " << COUNT_ITERATIONS << " iterations";
  prom.set_value(result.str());
}

void producerThread() {
  for (int i = 0; i < 10; ++i) {
    std::unique_lock<std::mutex> lock(pcMutex);

    // ждать, пока потребитель не заберёт предыдущее значение
    pcCv.wait(lock, [] { return !ready; });

    sharedValue = i;
    ready = true;

    lock.unlock();
    pcCv.notify_one();
  }

  // конец: сообщаем потребителю, что больше данных не будет
  {
    std::lock_guard<std::mutex> lock(pcMutex);
    done = true;
  }
  pcCv.notify_one();
}

void consumerThread() {
  while (true) {
    std::unique_lock<std::mutex> lock(pcMutex);

    // ждать, пока данные готовы ИЛИ производитель закончил
    pcCv.wait(lock, [] { return ready || done; });

    // если производитель закончил и данных нет — выходим
    if (done && !ready) break;

    int value = sharedValue;
    ready = false;

    lock.unlock();
    pcCv.notify_one();

    std::ostringstream oss;
    oss << "consumer: got value = " << value << "\n";
    writeLine(oss.str());
  }
}
