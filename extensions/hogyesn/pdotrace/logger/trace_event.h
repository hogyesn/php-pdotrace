#ifndef PDOTRACE_LOGGER_TRACE_EVENT_H
#define PDOTRACE_LOGGER_TRACE_EVENT_H

#include "php.h"

typedef struct {
	char *function_name;
	char *class_name;
	char *file;
	int line;
} function_call;

typedef struct {
	char *trace_id;
	char *endpoint;

	char *query;
	// char **bound_params;
	// size_t bound_param_count;

	function_call *callstack;
	size_t callstack_size;

	double runtime;
	zend_long timestamp;
} trace_event;

void free_trace_event(trace_event *event);

#endif /* PDOTRACE_LOGGER_TRACE_EVENT_H */
