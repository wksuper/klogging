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
// #include <cutils/log.h>

const char *_cpp_klogging_version()
{
	return VERSION;
}

KLogging::KLogging()
	: m_file(NULL), m_options(0), m_level(KLOGGING_LEVEL_OFF)
{
}

KLogging::~KLogging()
{
	if (m_file)
		fclose(m_file);
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

void KLogging::c(const char *file, int line, const char *function, const char *log_tag, const char *format, ...)
{
	va_list args;
	va_start(args, format);

	if (!IsToConsole()) {
		// If log is not being printed to console, print it here for end user
		vfprintf(stdout, format, args);
		fflush(stdout);
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
	static pthread_mutex_t s_mutex_for_file = PTHREAD_MUTEX_INITIALIZER;
	static pthread_mutex_t s_mutex_for_stdout = PTHREAD_MUTEX_INITIALIZER;
	static pthread_mutex_t s_mutex_for_stderr = PTHREAD_MUTEX_INITIALIZER;

	char timestr[32];
	char linestr[16];
	struct timeval tv;

	if (m_options & KLOGGING_PRINT_FILE_NAME) {
	} else {
		file = "";
	}

	if (m_options & KLOGGING_PRINT_LINE_NUM) {
		sprintf(linestr, "%d", line);
	} else {
		linestr[0] = '\0';
	}

	if (m_options & KLOGGING_PRINT_FUNCTION_NAME) {
	} else {
		function = "";
	}

	if (m_options & KLOGGING_NO_TIMESTAMP) {
		timestr[0] = '\0';
	} else {
		gettimeofday(&tv, NULL);
		strftime(timestr, sizeof(timestr), "%m-%d %H:%M:%S", localtime(&tv.tv_sec));
		sprintf(timestr + 14, ".%06ld ", tv.tv_usec);
	}

	if (m_file) {
		pthread_mutex_lock(&s_mutex_for_file);
		fprintf(m_file, "[%s%c:%s:%s:%s] ", timestr, type, file, linestr, function);
		vfprintf(m_file, format, args);
		pthread_mutex_unlock(&s_mutex_for_file);

		if (m_options & KLOGGING_FLUSH_IMMEDIATELY)
			fflush(m_file);
	}

	if (m_options & KLOGGING_TO_STDOUT) {
		pthread_mutex_lock(&s_mutex_for_stdout);
		fprintf(stdout, "[%s%c:%s:%s:%s] ", timestr, type, file, linestr, function);
		vfprintf(stdout, format, args);
		pthread_mutex_unlock(&s_mutex_for_stdout);

		if (m_options & KLOGGING_FLUSH_IMMEDIATELY)
			fflush(stdout);
	}
	if (m_options & KLOGGING_TO_STDERR) {
		pthread_mutex_lock(&s_mutex_for_stderr);
		fprintf(stderr, "[%s%c:%s:%s:%s] ", timestr, type, file, linestr, function);
		vfprintf(stderr, format, args);
		pthread_mutex_unlock(&s_mutex_for_stderr);

		if (m_options & KLOGGING_FLUSH_IMMEDIATELY)
			fflush(stderr);
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

extern "C" void _klogging_c(const char *file, int line, const char *function, const char *log_tag, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	if (!_klogging.IsToConsole()) {
		// If log is not being printed to console, print it here for end user
		vfprintf(stdout, format, args);
		fflush(stdout);
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
