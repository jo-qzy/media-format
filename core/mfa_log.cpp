//
// Created by quzhenyu on 2022/6/13.
//

#include <stdarg.h>
#include <stdio.h>

#include "mfa_log.h"

typedef enum MfaLogLevel LogLevel;

static const char *log_string[] = {"DEBUG", "INFO", "NOTICE", "ERROR", "FATAL"};

static FILE *log_file = NULL;

void mfa_log(LogLevel level, const char *format, ...)
{
    va_list args;

    if (!log_file)
        log_file = stdout;

    va_start(args, format);
    fprintf(log_file, "%s %s:%d] ", log_string[level], __FILE__, __LINE__);
    vfprintf(log_file, format, args);
    fprintf(log_file, "\n");
    va_end(args);
}
