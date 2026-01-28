#ifndef PDOTRACE_LOGGER_FORMATTER_H
#define PDOTRACE_LOGGER_FORMATTER_H

#include "trace_event.h"

typedef struct {
	char *buf;
	size_t buf_size;
} formatted_message;

formatted_message* format_trace_event(const trace_event *event);
void free_formatted_message(formatted_message *message);

#endif /* PDOTRACE_LOGGER_FORMATTER_H */
