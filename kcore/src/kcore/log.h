#ifndef KCORE_LOG_H
#define KCORE_LOG_H

#define KCORE_LOG_LENGTH 1024

#ifndef KCORE_LOG_TAG
#	define KCORE_LOG_TAG "kcore"
#endif

#define KCORE_LOG(LEVEL, ...) kcore_log_print_line(LEVEL, __FILE__, __LINE__, KCORE_LOG_TAG, __VA_ARGS__)

#define KCORE_TRACE(...) KCORE_LOG(KCORE_LOG_LEVEL_TRACE, __VA_ARGS__)
#define KCORE_DEBUG(...) KCORE_LOG(KCORE_LOG_LEVEL_DEBUG, __VA_ARGS__)
#define KCORE_INFO(...)  KCORE_LOG(KCORE_LOG_LEVEL_INFO, __VA_ARGS__)
#define KCORE_WARN(...)  KCORE_LOG(KCORE_LOG_LEVEL_WARN, __VA_ARGS__)
#define KCORE_ERROR(...) KCORE_LOG(KCORE_LOG_LEVEL_ERROR, __VA_ARGS__)
#define KCORE_FATAL(...) KCORE_LOG(KCORE_LOG_LEVEL_FATAL, __VA_ARGS__)

#define KCORE_PANIC(...)                            \
	do {                                        \
		KCORE_FATAL("PANIC: " __VA_ARGS__); \
		kcore_panic();                      \
	} while (0)
#define KCORE_ASSERT(EXPRESSION, ...)                                                             \
	do {                                                                                      \
		if (!(EXPRESSION)) KCORE_PANIC("Assertion " #EXPRESSION " failed: " __VA_ARGS__); \
	} while (0)
#define KCORE_EXPECT(EXPRESSION, ...)                                                            \
	do {                                                                                     \
		if (!(EXPRESSION)) KCORE_WARN("Assertion " #EXPRESSION " failed: " __VA_ARGS__); \
	} while (0)

#ifdef KCORE_DEBUG
#	define KCORE_DEBUG_PANIC(...) KCORE_PANIC(__VA_ARGS__)
#	define KCORE_DEBUG_ASSERT(EXPRESSION, ...) KCORE_ASSERT(EXPRESSION, __VA_ARGS__)
#	define KCORE_DEBUG_EXPECT(EXPRESSION, ...) KCORE_EXPECT(EXPRESSION, __VA_ARGS__)
#else
#	define KCORE_DEBUG_PANIC(...)
#	define KCORE_DEBUG_ASSERT(EXPRESSION, ...)
#	define KCORE_DEBUG_EXPECT(EXPRESSION, ...)
#endif

typedef enum Kcore_Log_Level {
	KCORE_LOG_LEVEL_TRACE = 0,
	KCORE_LOG_LEVEL_DEBUG = 1,
	KCORE_LOG_LEVEL_INFO  = 2,
	KCORE_LOG_LEVEL_WARN  = 3,
	KCORE_LOG_LEVEL_ERROR = 4,
	KCORE_LOG_LEVEL_FATAL = 5,
	KCORE_LOG_LEVEL_N
} Kcore_Log_Level;

typedef void(*Kcore_Log_Hook)(Kcore_Log_Level level, const char *file_name, int line_number, const char *tag, const char *message);

Kcore_Log_Level kcore_log_level(void);
void kcore_log_level_set(Kcore_Log_Level level);
void kcore_log_hook_set(Kcore_Log_Hook hook);
void kcore_log_print_line(Kcore_Log_Level level, const char *file_name, int line_number, const char *tag, const char *format, ...);
void kcore_panic(void);

#endif
