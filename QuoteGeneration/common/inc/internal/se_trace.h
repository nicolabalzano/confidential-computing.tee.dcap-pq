/*
 * Copyright(c) 2011-2025 Intel Corporation
 * SPDX-License-Identifier: BSD-3-Clause
 */


/*
 *This file wrapper some trace output.
*/
#pragma once

#ifndef _SE_DEBUG_H_
#define _SE_DEBUG_H_

#include <stdio.h>
#include <stdarg.h>

typedef enum
{
    SE_TRACE_NONE,
    SE_TRACE_ERROR,
    SE_TRACE_WARNING,
    SE_TRACE_NOTICE,
    SE_TRACE_DEBUG
} se_trace_t;

#ifdef _WIN32
#ifndef SE_DEBUG_LEVEL
/* Each module need define their own SE_DEBUG_LEVEL */
#define sgx_trace_loglevel SE_TRACE_ERROR
#else
#define sgx_trace_loglevel SE_DEBUG_LEVEL
#endif
#else /* _WIN32 */
#ifndef SE_DEBUG_LEVEL
/* Each module need define their own SE_DEBUG_LEVEL */
__attribute__((weak)) se_trace_t sgx_trace_loglevel = SE_TRACE_NONE;
#else
__attribute__((weak)) se_trace_t sgx_trace_loglevel = (se_trace_t)SE_DEBUG_LEVEL;
#endif
#endif /* _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif
void se_trace_internal(int debug_level, const char *fmt, ...);
void sgx_proc_log_report_default(int channel, int debug_level, const char* fmt, ...);
#ifdef _WIN32
extern void sgx_proc_log_report(int channel, int level, const char* format, ...);
#if defined (_M_IX86)
#pragma comment(linker, "/alternatename:_sgx_proc_log_report=_sgx_proc_log_report_default")
#elif defined (_M_IA64) || defined (_M_AMD64)   
#pragma comment(linker, "/alternatename:sgx_proc_log_report=sgx_proc_log_report_default")
#else 
#endif 
#else
void  __attribute__((weak)) sgx_proc_log_report(int level, const char* format, ...);

typedef void (*sgx_logging_callback_t)(int level, const char* message);

__attribute__((weak)) sgx_logging_callback_t sgx_trace_logger_callback = NULL;
#endif
#ifdef __cplusplus
}
#endif

/* For libraries, we usually define DISABLE_TRACE to disable any trace. */
/* For apps, we usually enable trace. */
#if(defined _WIN32) && (defined DISABLE_TRACE)
#define SE_TRACE(...)
#define se_trace(...)
#define se_trace_verbose(...)
#else
#define se_trace_verbose(debug_level, fmt, ...)     \
    do {                                            \
        se_trace_t trace_level = debug_level;       \
        if(trace_level <= sgx_trace_loglevel)       \
            se_trace_internal(trace_level, "[%s %s:%d] " fmt, __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__);       \
    }while(0)

#define se_trace(debug_level, fmt, ...)         \
    do {                                        \
        se_trace_t trace_level = debug_level;   \
        if(trace_level <= sgx_trace_loglevel)   \
            se_trace_internal(trace_level, fmt, ##__VA_ARGS__);       \
    }while(0)

/* For compatibility, SE_TRACE/se_trace is used in old code. */
/* New code should use SE_TRACE_DEBUG, SE_TRACE_NOTICE, SE_TRACE_WARNING, SE_TRACE_ERROR */
#define SE_TRACE(debug_level, fmt, ...) \
        se_trace_verbose(debug_level, fmt, ##__VA_ARGS__)

/* Print the debug information plus message. */
#define SE_TRACE_DEBUG(fmt, ...) se_trace_verbose(SE_TRACE_DEBUG, fmt, ##__VA_ARGS__)
#define SE_TRACE_NOTICE(fmt, ...) se_trace_verbose(SE_TRACE_NOTICE, fmt, ##__VA_ARGS__)
#define SE_TRACE_WARNING(fmt, ...) se_trace_verbose(SE_TRACE_WARNING, fmt, ##__VA_ARGS__)
#define SE_TRACE_ERROR(fmt, ...) se_trace_verbose(SE_TRACE_ERROR, fmt, ##__VA_ARGS__)
#endif

/* SE_PROD_LOG will output message to stdout by default in production mode.
   When the executable is running as daemon, it will output to syslog. */
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : \
                     (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__))
#ifdef _WIN32
#define SE_PROD_LOG(fmt, ...)                                                                                       \
    do {                                                                                                            \
        sgx_proc_log_report(1, 1, "[%s %s:%d] " fmt, __FUNCTION__, __FILENAME__, __LINE__, ##__VA_ARGS__);              \
    }while(0)
#else
#define SE_PROD_LOG(fmt, ...)                                                                                       \
    do {                                                                                                            \
        if(sgx_proc_log_report != NULL) {                                                                                   \
            sgx_proc_log_report(1, "[%s %s:%d] " fmt, __FUNCTION__, __FILENAME__, __LINE__, ##__VA_ARGS__);             \
        } else {                                                                                                    \
            se_trace_internal(SE_TRACE_ERROR, "[%s %s:%d] " fmt, __FUNCTION__, __FILENAME__, __LINE__, ##__VA_ARGS__);  \
        }                                                                                                           \
    }while(0)
#endif
#endif
