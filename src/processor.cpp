#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization() { 
  long idleJiffies = LinuxParser::IdleJiffies();
  long totalJiffies = LinuxParser::Jiffies();  
  int idleChange = idleJiffies - IdleJiffies_;
  int totalChange = totalJiffies - TotalJiffies_;
  float cpuUtilization = float (totalChange - idleChange) / totalChange;

  IdleJiffies_ = idleJiffies;
  TotalJiffies_ = totalJiffies;
  
  return cpuUtilization;
}