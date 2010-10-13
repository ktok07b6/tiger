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
	vfprintf(stderr, format, ap);
}

#ifndef LOG_PREFIX
#define LOG_PREFIX ""
#endif

#ifndef LOG_MASK
#define LOG_MASK 0x0000000f
#endif

#define _DBG(use_timestamps, fmt, args...)		\
	do {										\
		__DBG(fmt "\n", ##args);				\
	} while (0)

#define FATAL(fmt, args...) __FATAL(LOG_PREFIX fmt "\n", ##args)

#ifdef DEBUG

#ifdef DEBUG_LONG_FORMAT
#define _LOG(use_timestamps, level, prefix, file, line, func, fmt, args...) \
	if (LOG_MASK & level) {												\
		_DBG(use_timestamps, LOG_PREFIX prefix "%s:%d(%s): " fmt, file, line, func, ##args); \
	}
#else
#define _LOG(use_timestamps, level, prefix, file, line, func, fmt, args...) \
	if (LOG_MASK & level) {												\
		_DBG(use_timestamps, LOG_PREFIX prefix fmt, ##args);			\
	}
#endif

#else
#define _LOG(level, fmt, args...)
#endif //DEBUG

#define ERROR(fmt, args...) _LOG(USE_TIMESTAMPS, 0x1, "ERR: ", __FILE__, __LINE__, __func__, fmt, ##args)
#define WARN(fmt, args...) _LOG(USE_TIMESTAMPS, 0x2, "WRN: ", __FILE__, __LINE__, __func__, fmt, ##args)
#define INFO(fmt, args...) _LOG(USE_TIMESTAMPS, 0x4, "INF: ", __FILE__, __LINE__, __func__, fmt, ##args)
#define DBG(fmt, args...) _LOG(USE_TIMESTAMPS, 0x8, "DBG: ", __FILE__, __LINE__, __func__, fmt, ##args)
#define VDBG(fmt, args...) _LOG(USE_TIMESTAMPS, 0x10, "VDBG: ", __FILE__, __LINE__, __func__, fmt, ##args)
#define PARSER_DBG(fmt, args...) _LOG(USE_TIMESTAMPS, 0x8, "DBG: ", __FILE__, __LINE__, __func__, fmt, ##args)
#define LOG(level, fmt, args...) _LOG(USE_TIMESTAMPS, level, "", __FILE__, __LINE__, __func__, fmt, ##args)


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

