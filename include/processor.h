#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor() {
    IdleJiffies_ = 0;
    TotalJiffies_ = 0;
  }
  float Utilization();
  
 private:
    long IdleJiffies_;
    long TotalJiffies_;
};

#endif