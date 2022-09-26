#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using namespace LinuxParser;

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const { 
  /* get process stats */
  string line, token;
  vector<string> procStats;

  std::ifstream filestream(kProcDirectory + "/" + to_string(pid_) + kStatFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> token) {
        procStats.push_back(token);
      }
    }
  }
  
  /*
   * Reference: https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
   * Not a very good safety check, but it protects against reading
   * empty/invalid files atleast.
   */
    if (procStats.size() >= 22) {
      const long totalTime = std::stol(procStats[13]) +
                                std::stol(procStats[14]) +
                                std::stol(procStats[15]) +
                                std::stol(procStats[16]);
        return ((float)totalTime/sysconf(_SC_CLK_TCK))/UpTime();
    } else {
        return 0;
    }
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_);}

// Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
    return (CpuUtilization() - a.CpuUtilization()) > 0;
}