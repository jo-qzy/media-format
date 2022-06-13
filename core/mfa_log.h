//
// Created by quzhenyu on 2022/6/13.
//

#ifndef MFA_LOG_H
#define MFA_LOG_H

#ifdef MLOG_ENABLE

enum MfaLogLevel
{
    MLOG_INFO   = 0,
    MLOG_NOTION = 1,
    MLOG_ERROR  = 2,
};

void mfa_log(enum MfaLogLevel level, const char *format, ...);

#define MLOG_STREAM(LEVEL, FORMAT, ...) mfa_log(LEVEL, FORMAT, ##__VA_ARGS__)

#define MLOG(LEVEL, FORMAT, ...) MLOG_STREAM(MLOG_##LEVEL, FORMAT, ##__VA_ARGS__)

#else

#define MLOG(LEVEL, FORMAT, ...)

#endif

#endif // MFA_LOG_H
