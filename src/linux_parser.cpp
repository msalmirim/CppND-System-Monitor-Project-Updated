#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <filesystem>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

namespace fs = std::filesystem;

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
  string os, version, kernel;
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
/*
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
*/

//version using filesystem library
vector<int> LinuxParser::Pids() {
  vector<int> pids;

  for (const auto & entry : fs::directory_iterator(kProcDirectory)) {
      if (entry.is_directory()) {
        // Is every character of the name a digit?
        string dir_name(entry.path().filename().string());
        if (std::all_of(dir_name.begin(), dir_name.end(), isdigit)) {
          int pid = stoi(dir_name);
          pids.push_back(pid);
        }
      }
  }

  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float memTot = 0.0;
  float memFree = 0.0;
  string line;
  string key;
  string value;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value)
      {
        if (key == "MemTotal:") {
          memTot = std::stol(value);
        }
        if (key == "MemFree:") {
          memFree = std::stol(value);
        }
      }
    }
  }

  return ((memTot - memFree) / memTot);
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string uptimeStr;
  string idletime;
  string line;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptimeStr >> idletime; 
  }
  return std::stol(uptimeStr);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  
  return LinuxParser::UpTime() * sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  
  string path(kProcDirectory + std::to_string(pid) + kStatFilename);
  string line;
  string jiff;
  long actv_jiffies = 0;
  
  std::ifstream filestream(path);
  if (filestream.is_open())
  { 
    while (std::getline(filestream, line)) {
      std::istringstream strstream(line);
      std::istream_iterator<string> end;
      std::istream_iterator<string> start(strstream);
      vector<string> fieldVec(start, end);
      
      actv_jiffies += std::stol(fieldVec[13]);
      actv_jiffies += std::stol(fieldVec[14]);
      actv_jiffies += std::stol(fieldVec[15]);
      actv_jiffies += std::stol(fieldVec[16]);
      actv_jiffies += std::stol(fieldVec[17]);
    }
  }
  
  return actv_jiffies;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  
  string line;
  string jiff;
  long actv_jiffies = 0;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream strstream(line);
      std::istream_iterator<string> end;
      std::istream_iterator<string> start(strstream);
      std::vector<std::string> fieldVec(start, end);
      if (fieldVec[0] == "cpu")
      {
        actv_jiffies += std::stol(fieldVec[1]);
        actv_jiffies += std::stol(fieldVec[2]);
        actv_jiffies += std::stol(fieldVec[3]);
        actv_jiffies += std::stol(fieldVec[6]);
        actv_jiffies += std::stol(fieldVec[7]);
        actv_jiffies += std::stol(fieldVec[8]);
        break;
      }
    }
  }

  return actv_jiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  string line;
  string jeff;
  long idle_jeffs = 0;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream strstream(line);
        std::istream_iterator<string> start(strstream);
        std::istream_iterator<string> end;
        vector<string> fieldVec(start, end);
        if (fieldVec[0] == "cpu")
        {
          idle_jeffs += std::stol(fieldVec[4]);
          idle_jeffs += std::stol(fieldVec[5]);
          break;
        }
    }
  }

  return idle_jeffs;
}


// TODO: Read and return CPU utilization
float LinuxParser::CpuUtilization() {
  
  float active = ActiveJiffies();
  float idle = IdleJiffies();

  if (active + idle > 0.0f)
  {
      return active / (active + idle);
  }
  else
      return 0.0f;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  int procs = 0;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        procs = stoi(value);
        break;
      }
    }
  }
  
  return procs;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int run_process = 0;
  string line;
  string key;
  string value;

  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    while (std::getline(file, line)) {  
      std::istringstream strstream(line);
      while(strstream >> key >> value) {
        if (key == "procs_running")  {
          run_process = stoi(value);
          break;
        }
      }
    }
  }
  return run_process;
}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string path(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  string cmd;
  std::ifstream file(path);
  if(file.is_open())
    std::getline(file, cmd);
  
  return cmd;
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string path(kProcDirectory + std::to_string(pid) + kStatusFilename);
  string key;
  string value;
  string line;
  std::ifstream filestream(path);
  int memUtilization = 0;
  
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "VmSize:")
        {
          memUtilization = (std::stoi(value) / 1000);
          break;
        }
      }
    }
  }
  
  return std::to_string(memUtilization);
}

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string path(kProcDirectory + std::to_string(pid) + kStatusFilename);
  string key;
  string value;
  string line;
  std::ifstream filestream(path);
  int uid = 0;
  
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "Uid:")
        {
          uid = std::stoi(value);
          break;
        }
      }
    }
  }
  
  return std::to_string(uid);
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line;
  string user = "";
  string x;
  string uid;
  std::ifstream filestream(kPasswordPath);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> x >> uid) {
        if (uid == LinuxParser::Uid(pid)) {
          return user;
        }
      }
    }
  }
  return user;
}

// TODO: Read and return the uptime of a process
long int LinuxParser::UpTime(int pid) {
  string value;
  string line;
  long uptime = 0;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    std::istream_iterator<string> end;
    std::istream_iterator<string> start (linestream);
    vector<string> fieldVec(start, end);

    uptime = std::stol(fieldVec[21]);
  }
  return (uptime / sysconf(_SC_CLK_TCK));
}
