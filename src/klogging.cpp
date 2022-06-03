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

#include <klogging.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <mutex>
#include <string.h>
#ifdef ANDROID
#include <android/log.h>
#endif
#ifdef CONFIG_SUPPORT_LUA
#include <lua.hpp>
#endif

// CPP APIs
class KLogging {
public:
	static KLogging &Instance();

	int Set(int argc, char *argv[]);
	int SetFile(const char *filename);
	void EnableOptions(KLoggingOptions options) { m_options |= options; }
	void DisableOptions(KLoggingOptions options) { m_options &= ~options; }
	void SetLevel(KLoggingLevel level) { m_level = level; }

	void a(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...);
	void a(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, va_list args);
	void f(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...);
	void f(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, va_list args);
	void e(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...);
	void e(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, va_list args);
	void w(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...);
	void w(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, va_list args);
	void i(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...);
	void i(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, va_list args);
	void d(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...);
	void d(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, va_list args);
	void v(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...);
	void v(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, va_list args);

	~KLogging();

private:
	KLogging();
	KLogging(const KLogging &);
	KLogging &operator=(const KLogging &);
	inline bool CanPrintFatal() const { return m_level >= KLOGGING_LEVEL_FATAL; }
	inline bool CanPrintError() const { return m_level >= KLOGGING_LEVEL_ERROR; }
	inline bool CanPrintWarning() const { return m_level >= KLOGGING_LEVEL_WARNING; }
	inline bool CanPrintInfo() const { return m_level >= KLOGGING_LEVEL_INFO; }
	inline bool CanPrintDebug() const { return m_level >= KLOGGING_LEVEL_DEBUG; }
	inline bool CanPrintVerbose() const { return m_level >= KLOGGING_LEVEL_VERBOSE; }
	void Print(char type, KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, va_list args);

	FILE *m_file;
	KLoggingOptions m_options;
	KLoggingLevel m_level;

	std::mutex m_mutexForFile;
	std::mutex m_mutexForStdout;
	std::mutex m_mutexForStderr;
};

KLogging::KLogging()
	: m_file(NULL)
	, m_options(0)
	, m_level(KLOGGING_LEVEL_OFF)
{
}

KLogging::~KLogging()
{
	if (m_file)
		fclose(m_file);
}

int KLogging::Set(int argc, char *argv[])
{
	int ret = 0;

	for (int i = 0; i < argc; ++i) {
		const char *arg = argv[i];
		const size_t arg_len = strlen(arg);
		const char *key;
		size_t key_len;

		key = "KLOG_ENABLE_OPTIONS=";
		key_len = strlen(key);
		if (arg_len > key_len && strncmp(arg, key, key_len) == 0) {
			KLoggingOptions options = 0;

			if ((arg_len > key_len + 2)
			 && (arg[key_len] == '0')
			 && (arg[key_len+1] == 'x' || arg[key_len+1] == 'X')) {   /* Hex or not */
				options |= strtol(arg + key_len + 2, NULL, 16);
			} else if ((arg_len > key_len + 1) && (arg[key_len] == '0')) { /* Oct or not */
				options |= strtol(arg + key_len + 1, NULL, 8);
			} else { /* regard it as Dec */
				options |= strtol(arg + key_len, NULL, 10);
			}

			KLOG_ENABLE_OPTIONS(options);
		}

		key = "KLOG_SET_LEVEL=";
		key_len = strlen(key);
		if (arg_len > key_len && strncmp(arg, key, key_len) == 0) {
			enum KLoggingLevel level = (enum KLoggingLevel)atoi(arg + key_len);
			if (level <= KLOGGING_LEVEL_VERBOSE) {
				KLOG_SET_LEVEL(level);
			}
		}

		key = "KLOG_SET_FILE=";
		key_len = strlen(key);
		if (arg_len > key_len && strncmp(arg, key, key_len) == 0) {
			int rc = KLOG_SET_FILE(arg + key_len);
			if (rc != 0)
				ret = rc;
		}
	}
	return ret;
}

int KLogging::SetFile(const char *filename)
{
	if (m_file)
		fclose(m_file);
	if (filename) {
		m_file = fopen(filename, "w");
		return m_file ? 0 : -1;
	} else {
		m_file = NULL;
		return 0;
	}
}

void KLogging::a(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	a(enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	va_end(args);
}

void KLogging::a(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, va_list args)
{
	Print('A', enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
}

void KLogging::f(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	f(enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	va_end(args);
}

void KLogging::f(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, va_list args)
{
	if (CanPrintFatal()) {
		Print('F', enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	}
}

void KLogging::e(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	e(enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	va_end(args);
}

void KLogging::e(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, va_list args)
{
	if (CanPrintError()) {
		Print('E', enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	}
}

void KLogging::w(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	w(enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	va_end(args);
}

void KLogging::w(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, va_list args)
{
	if (CanPrintWarning()) {
		Print('W', enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	}
}

void KLogging::i(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	i(enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	va_end(args);
}

void KLogging::i(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, va_list args)
{
	if (CanPrintInfo()) {
		Print('I', enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	}
}

void KLogging::d(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	d(enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	va_end(args);
}

void KLogging::d(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, va_list args)
{
	if (CanPrintDebug()) {
		Print('D', enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	}
}

void KLogging::v(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	v(enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	va_end(args);
}

void KLogging::v(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, va_list args)
{
	if (CanPrintVerbose()) {
		Print('V', enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	}
}

void KLogging::Print(char type, KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, va_list args)
{
	char timestamp[32];
	char logtype[3];
	char separator[3];

	KLoggingOptions options = (m_options | enOpts) & ~disOpts;
	if (!lineEnd) {
		lineEnd =
#if defined(__APPLE__)
			"\r";
#elif defined(_WIN32)
			"\r\n";
#else
			"\n";
#endif
	}

	if (options & KLOGGING_NO_TIMESTAMP) {
		timestamp[0] = '\0';
	} else {
#ifdef _WIN32
		SYSTEMTIME now;

		GetLocalTime(&now);
		sprintf(timestamp, "%d-%d %d:%d:%d.%03d ",
			now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds);
#else
		struct timeval now;

		gettimeofday(&now, NULL);
		strftime(timestamp, sizeof(timestamp), "%m-%d %H:%M:%S", localtime(&now.tv_sec));
		sprintf(timestamp + 14, ".%06ld ", now.tv_usec);
#endif
	}

	if (options & KLOGGING_NO_LOGTYPE) {
		logtype[0] = '\0';
	} else {
		logtype[0] = type;
		logtype[1] = ' ';
		logtype[2] = '\0';
	}

	if ((options & KLOGGING_NO_TIMESTAMP) && (options & KLOGGING_NO_LOGTYPE)) {
		separator[0] = '\0';
	} else {
		separator[0] = '|';
		separator[1] = ' ';
		separator[2] = '\0';
	}

	if (m_file) {
		std::lock_guard<std::mutex> _l(m_mutexForFile);
		fprintf(m_file, "%s%s%s", timestamp, logtype, separator);
		vfprintf(m_file, format, args);
		if (options & KLOGGING_NO_SOURCEFILE)
			fprintf(m_file, "%s", lineEnd);
		else
			fprintf(m_file, " (%s:%d:%s)%s", file, line, function, lineEnd);
		if (options & KLOGGING_FLUSH_IMMEDIATELY)
			fflush(m_file);
	}

	if (options & KLOGGING_TO_STDOUT) {
		std::lock_guard<std::mutex> _l(m_mutexForStdout);
		fprintf(stdout, "%s%s%s", timestamp, logtype, separator);
		vfprintf(stdout, format, args);
		if (options & KLOGGING_NO_SOURCEFILE)
			fprintf(stdout, "%s", lineEnd);
		else
			fprintf(stdout, " (%s:%d:%s)%s", file, line, function, lineEnd);
		if (options & KLOGGING_FLUSH_IMMEDIATELY)
			fflush(stdout);
	}

	if (options & KLOGGING_TO_STDERR) {
		std::lock_guard<std::mutex> _l(m_mutexForStderr);
		fprintf(stderr, "%s%s%s", timestamp, logtype, separator);
		vfprintf(stderr, format, args);
		if (options & KLOGGING_NO_SOURCEFILE)
			fprintf(stderr, "%s", lineEnd);
		else
			fprintf(stderr, " (%s:%d:%s)%s", file, line, function, lineEnd);
		if (options & KLOGGING_FLUSH_IMMEDIATELY)
			fflush(stderr);
	}

	if (options & KLOGGING_TO_LOGCAT) {
#ifdef ANDROID
		__android_log_vprint(ANDROID_LOG_INFO, logTag, format, args);
#endif
	}
}

KLogging &KLogging::Instance()
{
	static KLogging s_klogging;
	return s_klogging;
}


KLOGGING_API const char *_klogging_version()
{
	return "0.9";
}

KLOGGING_API int _klogging_set(int argc, char *argv[])
{
	return KLogging::Instance().Set(argc, argv);
}

KLOGGING_API int _klogging_set_file(const char *filename)
{
	return KLogging::Instance().SetFile(filename);
}

KLOGGING_API void _klogging_enable_options(KLoggingOptions options)
{
	KLogging::Instance().EnableOptions(options);
}

KLOGGING_API void _klogging_disable_options(KLoggingOptions options)
{
	KLogging::Instance().DisableOptions(options);
}

KLOGGING_API void _klogging_set_level(enum KLoggingLevel level)
{
	KLogging::Instance().SetLevel(level);
}

KLOGGING_API void _klogging_a(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	KLogging::Instance().a(enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	va_end(args);
}

KLOGGING_API void _klogging_f(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	KLogging::Instance().f(enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	va_end(args);
}

KLOGGING_API void _klogging_e(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	KLogging::Instance().e(enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	va_end(args);
}

KLOGGING_API void _klogging_w(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	KLogging::Instance().w(enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	va_end(args);
}

KLOGGING_API void _klogging_i(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	KLogging::Instance().i(enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	va_end(args);
}

KLOGGING_API void _klogging_d(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	KLogging::Instance().d(enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	va_end(args);
}

KLOGGING_API void _klogging_v(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd, const char *file, int line, const char *function, const char *logTag, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	KLogging::Instance().v(enOpts, disOpts, lineEnd, file, line, function, logTag, format, args);
	va_end(args);
}

#ifdef CONFIG_SUPPORT_LUA
/********** Lua Interfaces **********/

static int l_KLOG_SET_LEVEL(lua_State* L)
{
	if (lua_isinteger(L, 1))
		KLOG_SET_LEVEL((enum KLoggingLevel)lua_tointeger(L, 1));
	return 0;
}

static int l_KLOG_ENABLE_OPTIONS(lua_State* L)
{
	if (lua_isinteger(L, 1))
		KLOG_ENABLE_OPTIONS((KLoggingOptions)lua_tointeger(L, 1));
	return 0;
}

static int l_KLOG_DISABLE_OPTIONS(lua_State* L)
{
	if (lua_isinteger(L, 1))
		KLOG_DISABLE_OPTIONS((KLoggingOptions)lua_tointeger(L, 1));
	return 0;
}

static int l_KLOG_VERSION(lua_State* L)
{
	lua_pushstring(L, KLOG_VERSION());
	return 1;
}

static int l_KLOGA(lua_State* L)
{
	if (lua_isstring(L, 1))
		KLOGA(lua_tostring(L, 1));
	return 0;
}

static int l_KLOGF(lua_State* L)
{
	if (lua_isstring(L, 1))
		KLOGF(lua_tostring(L, 1));
	return 0;
}

static int l_KLOGE(lua_State* L)
{
	if (lua_isstring(L, 1))
		KLOGE(lua_tostring(L, 1));
	return 0;
}

static int l_KLOGW(lua_State* L)
{
	if (lua_isstring(L, 1))
		KLOGW(lua_tostring(L, 1));
	return 0;
}

static int l_KLOGI(lua_State* L)
{
	if (lua_isstring(L, 1))
		KLOGI(lua_tostring(L, 1));
	return 0;
}

static int l_KLOGD(lua_State* L)
{
	if (lua_isstring(L, 1))
		KLOGD(lua_tostring(L, 1));
	return 0;
}

static int l_KLOGV(lua_State* L)
{
	if (lua_isstring(L, 1))
		KLOGV(lua_tostring(L, 1));
	return 0;
}

#define STR_AND_VAL(X) { #X, X }
struct NameAndVal {
	const char *name;
	uint32_t val;
};

extern "C" KLOGGING_API int luaopen_libklogging(lua_State* L)
{
	/* Register functions */
	static const struct luaL_Reg l_libklogging[] = {
		STR_AND_VAL(l_KLOG_VERSION),
		STR_AND_VAL(l_KLOG_SET_LEVEL),
		STR_AND_VAL(l_KLOG_ENABLE_OPTIONS),
		STR_AND_VAL(l_KLOG_DISABLE_OPTIONS),
		STR_AND_VAL(l_KLOGA),
		STR_AND_VAL(l_KLOGF),
		STR_AND_VAL(l_KLOGE),
		STR_AND_VAL(l_KLOGW),
		STR_AND_VAL(l_KLOGI),
		STR_AND_VAL(l_KLOGD),
		STR_AND_VAL(l_KLOGV),
		{ NULL, NULL }
	};
	luaL_newlib(L, l_libklogging);

	/* Register enums, constants, ... */
	static const struct NameAndVal l_syms[] = {
		STR_AND_VAL(KLOGGING_LEVEL_OFF),
		STR_AND_VAL(KLOGGING_LEVEL_FATAL),
		STR_AND_VAL(KLOGGING_LEVEL_ERROR),
		STR_AND_VAL(KLOGGING_LEVEL_WARNING),
		STR_AND_VAL(KLOGGING_LEVEL_DEBUG),
		STR_AND_VAL(KLOGGING_LEVEL_VERBOSE),
		STR_AND_VAL(KLOGGING_TO_STDOUT),
		STR_AND_VAL(KLOGGING_TO_STDERR),
		STR_AND_VAL(KLOGGING_TO_LOGCAT),
		STR_AND_VAL(KLOGGING_FLUSH_IMMEDIATELY),
		STR_AND_VAL(KLOGGING_NO_TIMESTAMP),
		STR_AND_VAL(KLOGGING_NO_LOGTYPE),
		STR_AND_VAL(KLOGGING_NO_SOURCEFILE)
	};
	for (size_t i = 0; i < sizeof(l_syms)/sizeof(l_syms[0]); ++i) {
		lua_pushinteger(L, l_syms[i].val);
		lua_setglobal(L, l_syms[i].name);
	}

	/* Disable this by default, since lua has no __FILE__, __LINE__, __FUNCTION__ info */
	KLOG_ENABLE_OPTIONS(KLOGGING_NO_SOURCEFILE);

	return 1;
}
#endif
