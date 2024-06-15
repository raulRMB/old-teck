#include "logger.h"
#include <cstdarg>
#include <string>
#include <iostream>
#include <cassert>

void tkLog(ELogLevel logLevel, const char* format, ...)
{
  std::string outString;
  switch(logLevel)
  {
    case ELogLevel::Info:
      outString = "[INFO]: ";
      break;
    case ELogLevel::Warning:
      outString = "[WARNING]: ";
      break;
    case ELogLevel::Error:
      outString = "[ERROR]: ";
      break;
  }

  va_list args;
  va_start(args, format);
  char buffer[8192];
  vsprintf_s(buffer, format, args);
  va_end(args);
  outString += buffer;
  std::cout << outString << std::endl;
}

void tkLogInfo(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  char buffer[8192];
  vsprintf_s(buffer, format, args);
  va_end(args);
  std::string outString = "[INFO]: ";
  outString += buffer;
  std::cout << outString << std::endl;
}

void tkLogWarning(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  char buffer[8192];
  vsprintf_s(buffer, format, args);
  va_end(args);
  std::string outString = "[WARNING]: ";
  outString += buffer;
  std::cout << outString << std::endl;
}

void tkLogError(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  char buffer[8192];
  vsprintf_s(buffer, format, args);
  va_end(args);
  std::string outString = "[ERROR]: ";
  outString += buffer;
  std::cout << outString << std::endl;
  assert(false);
}
