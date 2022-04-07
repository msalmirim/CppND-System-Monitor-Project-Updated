#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  int minutes = ((seconds % 3600) / 60);
  int secs = ((seconds % 3600) % 60);
  
  string hourStr = std::to_string(hours);
  string minStr = std::to_string(minutes);
  string secStr = std::to_string(secs);
 
  if(hourStr.length() == 1) { hourStr = "0" + hourStr; }
  if(minStr.length() == 1) { minStr = "0" + minStr; }
  if(secStr.length() == 1) { secStr = "0" + secStr; }
  
  string formatedTime = hourStr + ":" + minStr + ":" + secStr;
  return formatedTime;
}