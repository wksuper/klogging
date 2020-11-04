/*
 * Copyright (c) 2020 Kui Wang
 *
 * This file is part of klogging.
 *
 * klogging is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * klogging is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with klogging; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __KLOGGING_H__
#define __KLOGGING_H__

#include <stdint.h>
#include <stdio.h>

#ifndef LOG_TAG
#define LOG_TAG NULL
#endif

typedef uint16_t KLoggingOptions;

static const KLoggingOptions KLOGGING_TO_STDOUT = (0x1 << 0);
static const KLoggingOptions KLOGGING_TO_STDERR = (0x1 << 1);
static const KLoggingOptions KLOGGING_TO_LOGCAT = (0x1 << 2);
static const KLoggingOptions KLOGGING_PRINT_FILE_NAME = (0x1 << 11);
static const KLoggingOptions KLOGGING_PRINT_LINE_NUM = (0x1 << 12);
static const KLoggingOptions KLOGGING_PRINT_FUNCTION_NAME = (0x1 << 13);
static const KLoggingOptions KLOGGING_FLUSH_IMMEDIATELY = (0x1 << 14);
static const KLoggingOptions KLOGGING_NO_TIMESTAMP = (0x1 << 15);

enum KLoggingLevel {
	KLOGGING_LEVEL_OFF = 0,
	KLOGGING_LEVEL_ERROR,
	KLOGGING_LEVEL_WARNING,
	KLOGGING_LEVEL_INFO,
	KLOGGING_LEVEL_DEBUG,
	KLOGGING_LEVEL_VERBOSE
};

#ifdef __cplusplus

const char *_cpp_klogging_version();

// CPP APIs
class KLogging {
public:
	KLogging();
	int SetFile(const char *filename);

	void SetOptions(KLoggingOptions options) { m_options = options; }
	KLoggingOptions GetOptions() const { return m_options; }
	void SetLevel(KLoggingLevel level) { m_level = level; }
	inline bool CanPrintError() const { return m_level >= KLOGGING_LEVEL_ERROR; }
	inline bool CanPrintWarning() const { return m_level >= KLOGGING_LEVEL_WARNING; }
	inline bool CanPrintInfo() const { return m_level >= KLOGGING_LEVEL_INFO; }
	inline bool CanPrintDebug() const { return m_level >= KLOGGING_LEVEL_DEBUG; }
	inline bool CanPrintVerbose() const { return m_level >= KLOGGING_LEVEL_VERBOSE; }
	inline bool IsToConsole() const { return m_options & (KLOGGING_TO_STDOUT | KLOGGING_TO_STDERR); }

	void c(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
	void e(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
	void w(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
	void i(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
	void d(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
	void v(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);

	void Print(char type, const char *file, int line, const char *function, const char *log_tag, const char *format, va_list args);

	~KLogging();

private:
	KLogging(const KLogging &);
	KLogging &operator=(const KLogging &);

	FILE *m_file;
	KLoggingOptions m_options;
	KLoggingLevel m_level;
};

extern KLogging _klogging;

static inline int KLOG_SET_FILE(const char *filename) { return _klogging.SetFile(filename); }
static inline void KLOG_SET_OPTIONS(KLoggingOptions options) { _klogging.SetOptions(options); }
static inline KLoggingOptions KLOG_GET_OPTIONS() { return _klogging.GetOptions(); }
static inline void KLOG_SET_LEVEL(enum KLoggingLevel level) { _klogging.SetLevel(level); }
#define KVERSION()        _cpp_klogging_version()
#define KCONSOLE(args...) _klogging.c(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, ##args)
#define KLOGE(args...)    _klogging.e(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, ##args)
#define KLOGW(args...)    _klogging.w(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, ##args)
#define KLOGI(args...)    _klogging.i(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, ##args)
#define KLOGD(args...)    _klogging.d(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, ##args)
#define KLOGV(args...)    _klogging.v(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, ##args)

#else

/* C APIs */
const char *_klogging_version();
int _klogging_set_file(const char *filename);
void _klogging_set_options(KLoggingOptions options);
KLoggingOptions _klogging_get_options();
void _klogging_set_level(enum KLoggingLevel level);
void _klogging_c(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
void _klogging_e(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
void _klogging_w(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
void _klogging_i(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
void _klogging_d(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
void _klogging_v(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);

static inline int KLOG_SET_FILE(const char *filename) { return _klogging_set_file(filename); }
static inline void KLOG_SET_OPTIONS(KLoggingOptions options) { _klogging_set_options(options); }
static inline KLoggingOptions KLOG_GET_OPTIONS() { return _klogging_get_options(); }
static inline void KLOG_SET_LEVEL(enum KLoggingLevel level) { _klogging_set_level(level); }
#define KVERSION()        _klogging_version()
#define KCONSOLE(args...) _klogging_c(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, ##args)
#define KLOGE(args...)    _klogging_e(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, ##args)
#define KLOGW(args...)    _klogging_w(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, ##args)
#define KLOGI(args...)    _klogging_i(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, ##args)
#define KLOGD(args...)    _klogging_d(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, ##args)
#define KLOGV(args...)    _klogging_v(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, ##args)

#endif

#endif
