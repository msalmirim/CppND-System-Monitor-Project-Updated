#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid):pid_(pid){
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const {
  
  long sysUptime = LinuxParser::UpTime();
  
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid_) + LinuxParser::kStatFilename);
  string line;
  int utime = 0;
  int stime = 0;
  int cutime = 0;
  int cstime = 0;
  float total_time = 0.0;
  float seconds = 0.0;
  float cpu_usage = 0.0;
  float starttime = 0.0;

  if (stream.is_open())
  { 
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), '(', '_');
      std::replace(line.begin(), line.end(), ')', '_');
      std::replace(line.begin(), line.end(), '-', '_');

      std::istringstream strstream(line);
      std::istream_iterator<string> end;
      std::istream_iterator<string> start(strstream);
      vector<string> fieldVec(start, end);
      
      utime = std::stoi(fieldVec[13]);
      stime = std::stoi(fieldVec[14]);
      cutime = std::stoi(fieldVec[15]);
      cstime = std::stoi(fieldVec[16]);
      starttime = std::stof(fieldVec[21]);

      break;
    }
  }
  
  total_time = (float)(utime + stime + cutime + cstime);
  seconds = sysUptime - starttime / sysconf(_SC_CLK_TCK);
  cpu_usage = (total_time / sysconf(_SC_CLK_TCK)) / seconds;

  return cpu_usage;
  
}

// TODO: Return the command that generated this process
string Process::Command() {
  return LinuxParser::Command(pid_);
}

// TODO: Return this process's memory utilization
string Process::Ram() {
  return LinuxParser::Ram(pid_);
}

// TODO: Return the user (name) that generated this process
string Process::User() {
  return LinuxParser::User(pid_);
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
  return LinuxParser::UpTime(pid_);
}

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return (this->CpuUtilization() > a.CpuUtilization());
}