enum class ELogLevel
{
      Info = 0,
      Warning,
      Error
  };

void tkLog(ELogLevel logLevel, const char* format, ...);
void tkLogInfo(const char* format, ...);
void tkLogWarning(const char* format, ...);
void tkLogError(const char* format, ...);
