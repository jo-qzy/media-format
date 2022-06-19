//
// Created by quzhenyu on 2022/6/13.
//

#ifndef MFA_LOG_H
#define MFA_LOG_H

#ifdef MLOG_ENABLE

enum MfaLogLevel
{
    MLOG_DEBUG  = 0,
    MLOG_INFO   = 1,
    MLOG_NOTICE = 2,
    MLOG_ERROR  = 3,
    MLOG_FATAL  = 4,
};

void mfa_log(enum MfaLogLevel level, const char *format, ...);

#define MLOG_STREAM(LEVEL, FORMAT, ...) mfa_log(LEVEL, FORMAT, ##__VA_ARGS__)

#define MLOG(LEVEL, FORMAT, ...) MLOG_STREAM(MLOG_##LEVEL, FORMAT, ##__VA_ARGS__)

#else

#define MLOG(LEVEL, FORMAT, ...)

#endif

#endif // MFA_LOG_H
