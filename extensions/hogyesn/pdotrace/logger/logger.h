#ifndef PDOTRACE_LOGGER_H
#define PDOTRACE_LOGGER_H

#include <stddef.h>

typedef struct {
	const char *name;

	int  (*init)(void);
	void (*write)(const char *buf, size_t len);
	void (*flush)(void);
	void (*shutdown)(void);
} logger_methods;

/* set the active logger */
int set_logger(const logger_methods *methods);

/* lifecycle */
int  logger_init(void);
void logger_write(const char *buf, size_t len);
void logger_flush(void);
void logger_shutdown(void);

#endif
