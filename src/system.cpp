#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

// Not a good practice in general
using namespace std;

Processor& System::Cpu() {
  return cpu_;
}

vector<Process>& System::Processes() {
  processes_.clear();

  vector<int> pids = LinuxParser::Pids();
  for (int pid : pids) {
    Process cur = Process(pid);
    /* emplace_back (just forwarding the argument) is more efficient than push_back (creating a temporary object which then will need to get moved into the vector v)
    */
    processes_.emplace_back(cur);
  }

  return processes_;
}

std::string System::Kernel() {
  return LinuxParser::Kernel();
}

float System::MemoryUtilization() {
  return LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem() {
  return LinuxParser::OperatingSystem();
}

int System::RunningProcesses() {
  return LinuxParser::RunningProcesses();
}

int System::TotalProcesses() {
  return LinuxParser::TotalProcesses();
}

long int System::UpTime() {
  return LinuxParser::UpTime();
}