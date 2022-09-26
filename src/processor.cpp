#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  cpuJiffies = LinuxParser::CpuUtilization();
  long totalJiffies = Jiffies();
  long activeJiffies = ActiveJiffies();

  float utilization = (float)(activeJiffies-prevActiveJiffies)/(float)(totalJiffies-prevTotalJiffies);
  
  /* update the prev jiffy values */
  prevTotalJiffies = totalJiffies;
  prevActiveJiffies = activeJiffies;

  return utilization;
}

long Processor::Jiffies() {
  if(cpuJiffies.size() == 0) {
    return 0;
  }

  long jiffies=0;
  for (const string cpuJiffy : cpuJiffies) {
    jiffies = jiffies + std::stol(cpuJiffy);
  }

  return jiffies;
}

long Processor::IdleJiffies() {
  if(cpuJiffies.size() == 0) {
    return 0;
  }

  return std::stol(cpuJiffies[3]) + std::stol(cpuJiffies[4]);
}

long Processor::ActiveJiffies() {
  if(cpuJiffies.size() == 0) {
    return 0;
  }

  return Jiffies() - IdleJiffies();
}