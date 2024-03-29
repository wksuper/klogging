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

class KLogging {
public:
	static KLogging &Instance();

	const char *Version() const
	{
		return m_version;
	}

	int Set(int argc, char *argv[]);
	int SetFile(const char *filename);
	void EnableOptions(KLoggingOptions options) { m_options |= options; }
	void DisableOptions(KLoggingOptions options) { m_options &= ~options; }
	void SetLevel(KLoggingLevel level) { m_level = level; }

	inline bool CanPrint(char type) const
	{
		return (type == 'F' && m_level >= KLOGGING_LEVEL_FATAL)
			|| (type == 'E' && m_level >= KLOGGING_LEVEL_ERROR)
			|| (type == 'W' && m_level >= KLOGGING_LEVEL_WARNING)
			|| (type == 'I' && m_level >= KLOGGING_LEVEL_INFO)
			|| (type == 'D' && m_level >= KLOGGING_LEVEL_DEBUG)
			|| (type == 'V' && m_level >= KLOGGING_LEVEL_VERBOSE)
			|| (type == 'A');
	}

	void Print(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd,
		char type, const char *file, int line, const char *function, const char *logTag,
		const char *format, va_list argsA, va_list argsB, va_list argsC
#ifdef ANDROID
		, va_list argsD
#endif
		);

	~KLogging();

private:
	KLogging();
	KLogging(const KLogging &);
	KLogging &operator=(const KLogging &);

	char m_version[
		  3 /* 255 takes 3 chars */
		+ 1 /* dot */
		+ 3 /* 255 takes 3 chars */
		+ 1 /* dot */
		+ 3 /* 255 takes 3 chars */
		+ 1 /* \0 */];
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
	if (KLOG_PATCH_VER) {
		snprintf(m_version, sizeof(m_version), "%u.%u.%u", KLOG_MAJOR_VER, KLOG_MINOR_VER, KLOG_PATCH_VER);
	} else {
		snprintf(m_version, sizeof(m_version), "%u.%u", KLOG_MAJOR_VER, KLOG_MINOR_VER);
	}
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

			_klogging_enable_options(options);
		}

		key = "KLOG_SET_LEVEL=";
		key_len = strlen(key);
		if (arg_len > key_len && strncmp(arg, key, key_len) == 0) {
			enum KLoggingLevel level = (enum KLoggingLevel)atoi(arg + key_len);
			if (level <= KLOGGING_LEVEL_VERBOSE) {
				_klogging_set_level(level);
			}
		}

		key = "KLOG_SET_FILE=";
		key_len = strlen(key);
		if (arg_len > key_len && strncmp(arg, key, key_len) == 0) {
			int rc = _klogging_set_file(arg + key_len);
			if (rc != 0)
				ret = rc;
		}
	}
	return ret;
}

int KLogging::SetFile(const char *filename)
{
	std::lock_guard<std::mutex> _l(m_mutexForFile);

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

static void PrintToFd(FILE *fd,
	const char *timestamp, const char *logtype, const char *separator,
	const char *file, int line, const char *function,
	const char *lineEnd, KLoggingOptions options,
	const char *format, va_list args)
{
	fprintf(fd, "%s%s%s", timestamp, logtype, separator);
	vfprintf(fd, format, args);
	if (options & KLOGGING_NO_SOURCEFILE)
		fprintf(fd, "%s", lineEnd);
	else
		fprintf(fd, " (%s:%d:%s)%s", file, line, function, lineEnd);
	if (options & KLOGGING_FLUSH_IMMEDIATELY)
		fflush(fd);
}

void KLogging::Print(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd,
	char type, const char *file, int line, const char *function, const char *logTag,
	const char *format, va_list argsA, va_list argsB, va_list argsC
#ifdef ANDROID
	, va_list argsD
#endif
	)
{
	char timestamp[32];
	char logtype[3];
	char separator[3];

	KLoggingOptions options = (m_options | enOpts) & ~disOpts;
	if (!lineEnd) {
		lineEnd = "\n";
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
		sprintf(timestamp + 14, ".%06ld ", (long)now.tv_usec);
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

	{
		std::lock_guard<std::mutex> _l(m_mutexForFile);
		if (m_file) {
			PrintToFd(m_file,
				timestamp, logtype, separator,
				file, line, function,
				lineEnd, options,
				format, argsA);
		}
	}

	if (options & KLOGGING_TO_STDOUT) {
		std::lock_guard<std::mutex> _l(m_mutexForStdout);
		PrintToFd(stdout,
			timestamp, logtype, separator,
			file, line, function,
			lineEnd, options,
			format, argsB);
	}

	if (options & KLOGGING_TO_STDERR) {
		std::lock_guard<std::mutex> _l(m_mutexForStderr);
		PrintToFd(stderr,
			timestamp, logtype, separator,
			file, line, function,
			lineEnd, options,
			format, argsC);
	}

	if (options & KLOGGING_TO_LOGCAT) {
#ifdef ANDROID
		__android_log_vprint(ANDROID_LOG_INFO, logTag, format, argsD);
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
	return KLogging::Instance().Version();
}

KLOGGING_API int _klogging_version_compatible(uint8_t majorVer)
{
	return KLOG_MAJOR_VER == majorVer;
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

KLOGGING_API void _klogging_print(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd,
	char type, const char *file, int line, const char *function, const char *logTag,
	const char *format, ...)
{
	KLogging &kl = KLogging::Instance();
	if (kl.CanPrint(type)) {
		va_list argsA, argsB, argsC;
		va_start(argsA, format);
		va_start(argsB, format);
		va_start(argsC, format);
#ifdef ANDROID
		va_list argsD;
		va_start(argsD, format);
		kl.Print(enOpts, disOpts, lineEnd, type, file, line, function, logTag, format, argsA, argsB, argsC, argsD);
		va_end(argsD);
#else
		kl.Print(enOpts, disOpts, lineEnd, type, file, line, function, logTag, format, argsA, argsB, argsC);
#endif
		va_end(argsC);
		va_end(argsB);
		va_end(argsA);
	}
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
