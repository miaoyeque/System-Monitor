#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iterator>
#include <typeinfo>
#include <unistd.h>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line, key, value, kb;
  string memTotal, memFree;
  float total = 0.0;
  float free = 0.0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value >> kb) {
        if (key == "MemTotal:") {
          memTotal = value;
          total = stof(memTotal);
        } else if (key == "MemFree:") {
          memFree = value;
          free = stof(memFree);
        }
      }
    }
  }
  return (total - free) / total;
}

long LinuxParser::UpTime() {
  string line, uptime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    long upTime = stol(uptime);
    return upTime;
  }
  return 0;
}

long LinuxParser::Jiffies() {
  string key, values;
  string line;
  long total = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> key) {
      if (key == "cpu") {
        getline(linestream, values);
        std::istringstream iss(values);
        vector<string> numbers((std::istream_iterator<string>(iss)),
                                 std::istream_iterator<string>());
        for (string number : numbers) {
          int curJiffie = stoi(number);
          total += curJiffie;
        }
      }
    }
  }
  // Close the ifstream after its use to prevent it from reading another file
  stream.close();
  return total;
}

long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string pidstat, comm, state, ppid, pgrp, session, tty_nr, tpgid, flags, minflt, cminflt, majflt, cmajflt, utime, stime, cutime, cstime;
  long procActiveJiffies = 0;
  string pidDir = std::to_string(pid);
  std::ifstream filestream(kProcDirectory + pidDir + kStatFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> pidstat >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >> stime >> cutime >> cstime;
    procActiveJiffies = stoi(utime) + stoi(stime) + stoi(cutime) + stoi(cstime);
    return procActiveJiffies;
  }
  return 0;
}

long LinuxParser::ActiveJiffies() {
  return Jiffies() - IdleJiffies();
}

long LinuxParser::IdleJiffies() {
  string line;
  string cpu, user, nice, system, idle, iowait;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait;
    long idleJiffies = stoi(idle) + stoi(iowait);
    return idleJiffies;
  }
  return 0;
}

vector<string> LinuxParser::CpuUtilization() { return {}; }

int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          int totalProcesses = stoi(value);
          return totalProcesses;
        }
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          int runningProcesses = stoi(value);
          return runningProcesses;
        }
      }
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  string line;
  string pidDir = std::to_string(pid);
  std::ifstream filestream(kProcDirectory + pidDir + kCmdlineFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }

  return string();
}

string LinuxParser::Ram(int pid) {
  string line, key, value;
  string pidDir = std::to_string(pid);
  std::ifstream filestream(kProcDirectory + pidDir + kStatusFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        int mem = stoi(value) / 1024;
        return to_string(mem);
      }
    }
  }
  return string();
}

string LinuxParser::Uid(int pid) {
  string line, key, value, uid;
  string pidDir = std::to_string(pid);
  std::ifstream filestream(kProcDirectory + pidDir + kStatusFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        uid = value;
        return uid;
      }
    }
  }

  return string();
}

string LinuxParser::User(int pid) {
  string line, key, x, value, user, uid;
  std::ifstream filestream(kPasswordPath);

  uid = Uid(pid);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> x >> value;

      if (value == uid) {
        user = key;
        return user;
      }
    }
  }
  return string();
}

long LinuxParser::UpTime(int pid) {
  string line;
  string pidstat, comm, state, ppid, pgrp, session, tty_nr, tpgid, flags, minflt, cminflt, majflt, cmajflt, utime, stime, cutime, cstime, priority, nice, num_threads, itrealvalue, st;
  string pidDir = std::to_string(pid);
  std::ifstream filestream(kProcDirectory + pidDir + kStatFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> pidstat >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >> stime >> cutime >> cstime >> priority >> nice >> num_threads >> itrealvalue >> st;
    long startTime = stol(st);
    long startTimeInSeconds = startTime / sysconf(_SC_CLK_TCK);

    return startTimeInSeconds;
  }
  return 0;
}