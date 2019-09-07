#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <unistd.h>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return this->pid_; }

float Process::CpuUtilization() {
  long procActiveJiffies = LinuxParser::ActiveJiffies(this->pid_); // clock ticks
  long sysUpTime = LinuxParser::UpTime(); // seconds
  long procStartTime = LinuxParser::UpTime(this->pid_); // seconds
  long seconds = sysUpTime - procStartTime;

  float cpuUsage = 100 * (procActiveJiffies / sysconf(_SC_CLK_TCK)) / seconds;
  return cpuUsage;
}

string Process::Command() {
  return LinuxParser::Command(this->pid_);
}

string Process::Ram() {
  return LinuxParser::Ram(this->pid_);
}

string Process::User() {
  return LinuxParser::User(this->pid_);
}

long int Process::UpTime() {
  return LinuxParser::UpTime(this->pid_);
}

bool Process::operator>(const Process& a) const {
  return CpuUtilization() > a.CpuUtilization();
}