#include "../include/logger.h"

/*******************************************************************************
* constants
*******************************************************************************/

static const char* log_name[] =
{
  "EMERG",
  "ALERT",
  "CRITIC",
  "ERROR",
  "WARN",
  "NOTICE",
  "INFO",
  "DEBUG"
};

/*******************************************************************************
* logging functions
*******************************************************************************/

void user_log(int level, const char* format, ...) {
    va_list ap;

    printf("%s:\t", log_name[level]);
    va_start(ap, format);
    if(level <= MAX_LOG_LEVEL) {
        vprintf(format, ap);
        // vsyslog(level, format, ap);
    }
    va_end(ap);

    return;
}