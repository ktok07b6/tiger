#ifndef DEBUG_H
#define DEBUG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#define FLOG DBG("%s", __PRETTY_FUNCTION__);

/// Fatal error
inline void __FATAL(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);

	vprintf(format, ap);

	abort();
}

/// printf-style debug print
inline void __DBG(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vfprintf(stdout, format, ap);
}

#ifndef LOG_TAG
#define LOG_TAG ""
#endif

#define ERROR_ON 0x1
#define WARN_ON 0x2
#define INFO_ON 0x4
#define DEBUG_ON 0x8
#define VERBOSE_ON 0x10

#ifndef LOG_MASK
#define LOG_MASK (ERROR_ON | WARN_ON | INFO_ON | DEBUG_ON)
#endif

#define _DBG(use_timestamps, fmt, ...)		\
	do {										\
		__DBG(fmt "\n", ##__VA_ARGS__);				\
	} while (0)

#define FATAL(fmt, ...) __FATAL(LOG_TAG fmt "\n", ##__VA_ARGS__)

#ifdef DEBUG

#ifdef DEBUG_LONG_FORMAT
#define _LOG(use_timestamps, level, prefix, file, line, func, fmt, ...) \
	if (LOG_MASK & level) {												\
		_DBG(use_timestamps, LOG_TAG prefix "%s:%d(%s): " fmt, file, line, func, ##__VA_ARGS__); \
	}
#else
#define _LOG(use_timestamps, level, prefix, file, line, func, fmt, ...) \
	if (LOG_MASK & level) {												\
		_DBG(use_timestamps, LOG_TAG prefix fmt, ##__VA_ARGS__);			\
	}
#endif

#else
#define _LOG(level, fmt, ...)
#endif //DEBUG

#define ERROR(fmt, ...) _LOG(USE_TIMESTAMPS, 0x1, "ERR: ", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) _LOG(USE_TIMESTAMPS, 0x2, "WRN: ", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) _LOG(USE_TIMESTAMPS, 0x4, "INF: ", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define DBG(fmt, ...) _LOG(USE_TIMESTAMPS, 0x8, "DBG: ", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define VDBG(fmt, ...) _LOG(USE_TIMESTAMPS, 0x10, "VDBG: ", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define PARSER_DBG(fmt, ...) _LOG(USE_TIMESTAMPS, 0x20, "DBG: ", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG(level, fmt, ...) _LOG(USE_TIMESTAMPS, level, "", __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)


struct Funclog
{
	Funclog(const char *n) : name(n) {DBG("%s", name);}
	~Funclog() {DBG("%s --end", name);}
	const char *name;
};

#ifdef ENABLE_FUNCLOG
#define FUNCLOG Funclog ___flog(__PRETTY_FUNCTION__)
#else
#define FUNCLOG 
#endif

#endif //DEBUG_H

