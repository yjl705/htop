#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

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
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string memKey, memInfo;
  string line;
  float total = 0.0, free = 0.0;
  bool memInfoFound = false;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> memKey >> memInfo;
      if (memKey == "MemTotal:"){
        total = std::stol(memInfo);
      } else if(memKey == "MemFree"){
        free = std::stol(memInfo);
        memInfoFound = true;
        break;
      }
    }
  }
  
  if(memInfoFound){
    return (float)(total - free)/total;
  }else{
   return 0;
  }
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  string line, uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()){
  	std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return std::stol(uptime);
  }
  return 0;
}

// Read and return the number of jiffies for the system
// Implemented in processor.cpp
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
// long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// Read and return the number of active jiffies for the system
// Implemented in processor.cpp
long LinuxParser::ActiveJiffies() { return 0; }

// Read and return the number of idle jiffies for the system
// Implemented in processor.cpp
long LinuxParser::IdleJiffies() { return 0; }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line, tag;
  std::ifstream stream(kProcDirectory + kStatFilename);
  vector<string>cpuJiffies;
  if (stream.is_open()){
	std::getline(stream, line);
    std::istringstream linestream(line);
    if(linestream >> tag){
      string tempJiffyVal;
      while (linestream >> tempJiffyVal) {
        cpuJiffies.push_back(tempJiffyVal);
      }
    }
  }
  
  return cpuJiffies;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line, key, val;
  string processesKey, processesValue;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == "processes") return std::stoi(val);
    }
  }
  
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, key, val;
  string processesKey, processesValue;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == "procs_running") return std::stoi(val);
    }
  }
  
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream stream(kProcDirectory + "/"+ to_string(pid)+ kCmdlineFilename);
  if(stream.is_open()){
    if(std::getline(stream, line)) return line;
  }
  return string(); 
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line, key, val;
  std::ifstream stream(kProcDirectory + "/"+ to_string(pid)+ kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> val;
      if(key == "VmSize:"){
        return to_string(std::stol(val)/1024);
      }
    }
  }
  return string(); 
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key, val;
  std::ifstream stream(kProcDirectory + "/"+ to_string(pid)+ kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> val;
      if(key == "Uid:"){
        return val;
      }
    }
  }
  return string(); 
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string line, key, val, tempToken, userToken, uidToken;
  // find the uid given the pid
  string uid = Uid(pid);
  
  std::ifstream stream(kPasswordPath);
  if(stream.is_open()){
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      int count = 0;

      // split the password file line with ':' as delimiter
      while (std::getline(linestream, tempToken, ':')) {
        // first token is the username
        if (count == 0) {
          userToken = tempToken;
        } else if (count == 2) {
          // third token is the uid
          uidToken = tempToken;
          break;
        }
        count++;
      }

      /*
       * if uid token from the password file is same as the uid read from the
       * proc status file, then we have found the right username.
       */
      if (uidToken == uid) {
        return userToken;
      }
    }
  }
  
  return string(); 
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid[[maybe_unused]]) {
  /* get process stats */
  string line, token;
  vector<string> procStats;

  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> token) {
        procStats.push_back(token);
      }
    }
  }

    /*
     * Not a very good safety check, but it protects against reading
     * empty/invalid files atleast.
     */
    if (procStats.size() >= 22) {
        return UpTime() - (std::stol(procStats[21])/sysconf(_SC_CLK_TCK));
    } else {
        return 0;
    }
}

