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

void set_log_level(int level) {
    user_log(LOG_INFO, "Set maximum log level to %-8s\n", log_name[level]);
    setlogmask(LOG_UPTO (level));
    return;
}

void open_syslog(const char *name) {
    // openlog(name, LOG_PERROR, 0);
    openlog(name, 0, 0);
    user_log(LOG_INFO, "Starting system log\n");
    return;
}

void close_syslog() {
    user_log(LOG_INFO, "Closing system log\n");
    closelog();
    return;
}

void user_log(int level, const char* format, ...) {
    va_list ap;
    char time_string[200];

    time_t ts;
    struct tm ts_struct;

    // get current log mask
    int current_log_mask = setlogmask(0);
    int max_l = 0;
    while(current_log_mask > 0) {
        current_log_mask = current_log_mask >> 1;
        max_l++;
    }
    max_l--;
    // max_l = (max_l > LOG_DEBUG ? LOG_DEBUG : max_l);

    // get current timestamp
    ts = time(NULL);
    ts_struct = *localtime(&ts);
    strftime(time_string, sizeof(time_string), "%a %Y-%m-%d %H:%M:%S %Z", &ts_struct);

    // print to console
    #if CONSOLE_LOG_EN == 1
        va_start(ap, format);
        if(level <= max_l) {
            printf("%s - Level: %-8s - ", time_string, log_name[level]);
            vprintf(format, ap);
        }
        va_end(ap);
    #endif

    // print to journal
    #if SYS_LOG_EN == 1
        va_start(ap, format);
        vsyslog(level, format, ap);
        va_end(ap);
    #else
    
    #endif

    return;
}