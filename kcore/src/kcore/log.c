#include <stdarg.h>
#include <stdio.h>

#include "log.h"

#define LOG_BLUE   "\x1b[34m"
#define LOG_GREEN  "\x1b[32m"
#define LOG_WHITE  "\x1b[37m"
#define LOG_YELLOW "\x1b[33m"
#define LOG_ORANGE "\x1b[91m"
#define LOG_RED    "\x1b[31m"
#define LOG_RESET  "\x1b[0m"

typedef struct Log_Global {
	Kcore_Log_Level level;
	Kcore_Log_Hook  hook;
} Log_Global;

static Log_Global log;

typedef struct Log_Level_Info {
	const char  *name;
	const char  *style;
	FILE       **stream;
} Log_Level_Info;

static const Log_Level_Info log_level_info_table[KCORE_LOG_LEVEL_N] = {
	[KCORE_LOG_LEVEL_TRACE] = {.name = "Trace", .style = LOG_BLUE,   .stream = &stdout},
	[KCORE_LOG_LEVEL_DEBUG] = {.name = "Debug", .style = LOG_GREEN,  .stream = &stdout},
	[KCORE_LOG_LEVEL_INFO]  = {.name = "Info",  .style = LOG_WHITE,  .stream = &stdout},
	[KCORE_LOG_LEVEL_WARN]  = {.name = "Warn",  .style = LOG_YELLOW, .stream = &stderr},
	[KCORE_LOG_LEVEL_ERROR] = {.name = "Error", .style = LOG_ORANGE, .stream = &stderr},
	[KCORE_LOG_LEVEL_FATAL] = {.name = "Fatal", .style = LOG_RED,    .stream = &stderr},
};

Kcore_Log_Level kcore_log_level(void)
{
	return log.level;
}

void kcore_log_level_set(Kcore_Log_Level level)
{
	log.level = level;
}

void kcore_log_hook_set(Kcore_Log_Hook hook)
{
	log.hook = hook;
}

void kcore_log_print_line(Kcore_Log_Level level, const char *file_name, int line_number, const char *tag, const char *format, ...)
{
	static char buffer[KCORE_LOG_LENGTH];

	KCORE_DEBUG_ASSERT(KCORE_LOG_LEVEL_TRACE <= level && level <= KCORE_LOG_LEVEL_FATAL, "Log level is unknown: `%d`.", level);
	
	if (level < log.level) return;

	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof buffer, format, args);
	va_end(args);

	if (log.hook != NULL) {
		log.hook(level, file_name, line_number, tag, buffer);
		return;
	}

	const Log_Level_Info *level_info = log_level_info_table + level;
	fprintf(*level_info->stream, "%s %s[%s] %s:%d [%s]" LOG_RESET " %s\n", tag, level_info->style, level_info->name, file_name, line_number, level_info->name, buffer);
}

void kcore_panic(void)
{
	*(int *)NULL = 0;
}
