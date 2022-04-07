#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  float active = LinuxParser::ActiveJiffies();
  float idle = LinuxParser::IdleJiffies();

  float total = active + idle;

  /* differentiate: actual value minus the previous one */
  float totald = total - prevTotal_;
  float idled = idle - prevIdle_;

  prevTotal_ = total;
  prevIdle_ = idle;
  
  return ((totald - idled)/totald);
}