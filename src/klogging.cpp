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
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <string>
#include <stdlib.h>
// #include <cutils/log.h>

static pthread_mutex_t s_mutex_for_file = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_mutex_for_stdout = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_mutex_for_stderr = PTHREAD_MUTEX_INITIALIZER;

const char *_cpp_klogging_version()
{
	return VERSION;
}

KLogging::KLogging()
	: m_file(NULL)
	, m_options(KLOGGING_PRINT_SOURCEFILE_INFO)
	, m_level(KLOGGING_LEVEL_OFF)
	, m_lineEnd("\n")
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

		key = "KLOG_SET_OPTIONS=";
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

			KLOG_SET_OPTIONS(options);
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

		key = "KLOG_SET_LINEEND=";
		key_len = strlen(key);
		if ((arg_len >= key_len) && strncmp(arg, key, key_len) == 0) {
			std::string tmp;
			const char *val = arg + key_len;
			size_t val_len = arg_len - key_len;
			for (size_t i = 0; i < val_len;) {
				if (val[i] == '\\' && (i + 1 < val_len)) {
					switch (val[i+1]) {
					case 'n':
						tmp.push_back('\n');
						i += 2;
						break;
					case 'r':
						tmp.push_back('\r');
						i += 2;
						break;
					case 't':
						tmp.push_back('\t');
						i += 2;
						break;
					default:
						tmp.push_back(val[i]);
						++i;
						break;
					}
				} else {
					tmp.push_back(val[i]);
					++i;
				}
			}
			int rc = KLOG_SET_LINEEND(tmp.c_str());
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

int KLogging::SetLineEnd(const char *end)
{
	size_t len;

	if (!end)
		return -1;

	len = strlen(end);
	if (len + 1 > sizeof(m_lineEnd))
		return -1;

	strncpy(m_lineEnd, end, sizeof(m_lineEnd));
	return 0;
}

void KLogging::c(const char *file, int line, const char *function, const char *log_tag, const char *format, ...)
{
	va_list args;
	va_start(args, format);

	if (!IsToConsole()) {
		// If log is not being printed to console, print it here for end user
		pthread_mutex_lock(&s_mutex_for_stdout);
		vfprintf(stdout, format, args);
		fflush(stdout); // update the result for end user
		pthread_mutex_unlock(&s_mutex_for_stdout);
	} else {
		// If log is being printed to console, just do nothing here
	}
	Print('C', file, line, function, log_tag, format, args);
	va_end(args);
}

void KLogging::e(const char *file, int line, const char *function, const char *log_tag, const char *format, ...)
{
	if (CanPrintError()) {
		va_list args;
		va_start(args, format);
		Print('E', file, line, function, log_tag, format, args);
		va_end(args);
	}
}

void KLogging::w(const char *file, int line, const char *function, const char *log_tag, const char *format, ...)
{
	if (CanPrintWarning()) {
		va_list args;
		va_start(args, format);
		Print('W', file, line, function, log_tag, format, args);
		va_end(args);
	}
}

void KLogging::i(const char *file, int line, const char *function, const char *log_tag, const char *format, ...)
{
	if (CanPrintInfo()) {
		va_list args;
		va_start(args, format);
		Print('I', file, line, function, log_tag, format, args);
		va_end(args);
	}
}

void KLogging::d(const char *file, int line, const char *function, const char *log_tag, const char *format, ...)
{
	if (CanPrintDebug()) {
		va_list args;
		va_start(args, format);
		Print('D', file, line, function, log_tag, format, args);
		va_end(args);
	}
}

void KLogging::v(const char *file, int line, const char *function, const char *log_tag, const char *format, ...)
{
	if (CanPrintVerbose()) {
		va_list args;
		va_start(args, format);
		Print('V', file, line, function, log_tag, format, args);
		va_end(args);
	}
}

void KLogging::Print(char type, const char *file, int line, const char *function, const char *log_tag, const char *format, va_list args)
{
	char timestr[32];

	if (m_options & KLOGGING_NO_TIMESTAMP) {
		timestr[0] = '\0';
	} else {
		struct timeval tv;

		gettimeofday(&tv, NULL);
		strftime(timestr, sizeof(timestr), "%m-%d %H:%M:%S", localtime(&tv.tv_sec));
		sprintf(timestr + 14, ".%06ld", tv.tv_usec);
	}

	if (m_file) {
		pthread_mutex_lock(&s_mutex_for_file);
		fprintf(m_file, "[%s %c] ", timestr, type);
		vfprintf(m_file, format, args);
		if (m_options & KLOGGING_PRINT_SOURCEFILE_INFO)
			fprintf(m_file, " (%s:%d:%s)%s", file, line, function, m_lineEnd);
		else
			fprintf(m_file, "%s", m_lineEnd);
		if (m_options & KLOGGING_FLUSH_IMMEDIATELY)
			fflush(m_file);
		pthread_mutex_unlock(&s_mutex_for_file);
	}

	if (m_options & KLOGGING_TO_STDOUT) {
		pthread_mutex_lock(&s_mutex_for_stdout);
		fprintf(stdout, "[%s %c] ", timestr, type);
		vfprintf(stdout, format, args);
		if (m_options & KLOGGING_PRINT_SOURCEFILE_INFO)
			fprintf(stdout, " (%s:%d:%s)%s", file, line, function, m_lineEnd);
		else
			fprintf(stdout, "%s", m_lineEnd);
		if (m_options & KLOGGING_FLUSH_IMMEDIATELY)
			fflush(stdout);
		pthread_mutex_unlock(&s_mutex_for_stdout);
	}

	if (m_options & KLOGGING_TO_STDERR) {
		pthread_mutex_lock(&s_mutex_for_stderr);
		fprintf(stderr, "[%s %c] ", timestr, type);
		vfprintf(stderr, format, args);
		if (m_options & KLOGGING_PRINT_SOURCEFILE_INFO)
			fprintf(stderr, " (%s:%d:%s)%s", file, line, function, m_lineEnd);
		else
			fprintf(stderr, "%s", m_lineEnd);
		if (m_options & KLOGGING_FLUSH_IMMEDIATELY)
			fflush(stderr);
		pthread_mutex_unlock(&s_mutex_for_stderr);
	}

	if (m_options & KLOGGING_TO_LOGCAT) {
		// __android_log_vprint(ANDROID_LOG_INFO, log_tag, format, args);
	}
}


KLogging _klogging;


extern "C" const char *_klogging_version()
{
	return VERSION;
}

extern "C" int _klogging_set(int argc, char *argv[])
{
	return _klogging.Set(argc, argv);
}

extern "C" int _klogging_set_file(const char *filename)
{
	return _klogging.SetFile(filename);
}

extern "C" void _klogging_set_options(KLoggingOptions options)
{
	_klogging.SetOptions(options);
}

extern "C" KLoggingOptions _klogging_get_options()
{
	return _klogging.GetOptions();
}

extern "C" void _klogging_set_level(enum KLoggingLevel level)
{
	_klogging.SetLevel(level);
}

extern "C" int _klogging_set_lineend(const char *end)
{
	return _klogging.SetLineEnd(end);
}

extern "C" void _klogging_c(const char *file, int line, const char *function, const char *log_tag, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	if (!_klogging.IsToConsole()) {
		// If log is not being printed to console, print it here for end user
		pthread_mutex_lock(&s_mutex_for_stdout);
		vfprintf(stdout, format, args);
		fflush(stdout); // update the result for end user
		pthread_mutex_unlock(&s_mutex_for_stdout);
	} else {
		// If log is being printed to console, just do nothing here
	}
	_klogging.Print('C', file, line, function, log_tag, format, args);
	va_end(args);
}

extern "C" void _klogging_e(const char *file, int line, const char *function, const char *log_tag, const char *format, ...)
{
	if (_klogging.CanPrintError()) {
		va_list args;
		va_start(args, format);
		_klogging.Print('E', file, line, function, log_tag, format, args);
		va_end(args);
	}
}

extern "C" void _klogging_w(const char *file, int line, const char *function, const char *log_tag, const char *format, ...)
{
	if (_klogging.CanPrintWarning()) {
		va_list args;
		va_start(args, format);
		_klogging.Print('W', file, line, function, log_tag, format, args);
		va_end(args);
	}
}

extern "C" void _klogging_i(const char *file, int line, const char *function, const char *log_tag, const char *format, ...)
{
	if (_klogging.CanPrintInfo()) {
		va_list args;
		va_start(args, format);
		_klogging.Print('I', file, line, function, log_tag, format, args);
		va_end(args);
	}
}

extern "C" void _klogging_d(const char *file, int line, const char *function, const char *log_tag, const char *format, ...)
{
	if (_klogging.CanPrintDebug()) {
		va_list args;
		va_start(args, format);
		_klogging.Print('D', file, line, function, log_tag, format, args);
		va_end(args);
	}
}

extern "C" void _klogging_v(const char *file, int line, const char *function, const char *log_tag, const char *format, ...)
{
	if (_klogging.CanPrintVerbose()) {
		va_list args;
		va_start(args, format);
		_klogging.Print('V', file, line, function, log_tag, format, args);
		va_end(args);
	}
}
