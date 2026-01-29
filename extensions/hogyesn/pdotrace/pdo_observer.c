#include "php.h"
#include "zend_observer.h"

#include "php_pdotrace.h"

#include "logger/logger.h"
#include "logger/trace_event.h"
#include "logger/formatter.h"

static inline uint64_t monotonic_ns(void) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t) ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

ZEND_TLS uint64_t begin_time;
ZEND_TLS zend_execute_data *current_frame = NULL;

/* Observer begin handler */
void pdotrace_observer_begin(zend_execute_data *execute_data)
{
	if (!ZEND_OBSERVER_ENABLED) {
		return;
	}

	zend_execute_data *frame = execute_data;
	zend_function *func = frame->func;

	if (!func) {
		return;
	}

	const char *func_name = ZSTR_VAL(func->common.function_name);
	const char *class_name = func->common.scope ? ZSTR_VAL(func->common.scope->name) : NULL;

	if (!class_name || class_name && strcmp(class_name, "PDO") != 0) {
		// Skip logging non-PDO frames
		current_frame = NULL;
		return;
	}

	begin_time = monotonic_ns();
	current_frame = frame;
}

/* Observer end handler */
void pdotrace_observer_end(zend_execute_data *execute_data, zval *retval)
{
	if (!ZEND_OBSERVER_ENABLED) {
		return;
	}

	zend_execute_data *frame = execute_data;
	int depth = 0;

	if (current_frame != frame) {
		// Skip logging non-PDO frames
		return;
	}
	current_frame = NULL;

	uint64_t end_time = monotonic_ns();

	zend_function *func = frame->func;

	if (!func) {
		return;
	}

	const char *func_name = ZSTR_VAL(func->common.function_name);
	const char *class_name = func->common.scope ? ZSTR_VAL(func->common.scope->name) : NULL;

	trace_event event = {0};
	event.trace_id = estrdup(PDOTRACE_G(trace_id));
	event.endpoint = estrdup("todo");
	event.timestamp = (zend_long)time(NULL);
	event.runtime = (double)(end_time - begin_time) / 1000000.0;

	function_call callstack[10];
	size_t callstack_size = 0;

	// Log call stack
	while (frame && depth++ < 10) {
		if (func->common.function_name) {
			function_call *fc = &callstack[callstack_size++];
			if (class_name) {
				fc->class_name = estrdup(class_name);
			}
			fc->function_name = estrdup(func_name);

			// log caller info
			if (frame->prev_execute_data && frame->prev_execute_data->opline && frame->prev_execute_data->func->op_array.filename) {
				fc->line = frame->prev_execute_data->opline->lineno;
				fc->file = estrdup(ZSTR_VAL(frame->prev_execute_data->func->op_array.filename));
			}

			// Extract query from PDO methods
			if (strcmp(func_name, "query") == 0 || strcmp(func_name, "exec") == 0) {
				int argc = ZEND_CALL_NUM_ARGS(frame);
				for (int i = 0; i < argc; i++) {
					zval *arg = ZEND_CALL_ARG(frame, i + 1);
					if (arg && Z_TYPE_P(arg) != IS_UNDEF && Z_TYPE_P(arg) == IS_STRING) {
						zend_string *arg_str = zval_get_string(arg);
						
						// Set query in event
						event.query = estrdup(ZSTR_VAL(arg_str));

						zend_string_release(arg_str);
					}
				}
			}
		}

		// prepare for next frame (which is the caller)
		frame = frame->prev_execute_data;

		if (!frame || !frame->func) {
			break;
		}

		func = frame->func;
		func_name = ZSTR_VAL(func->common.function_name);
		class_name = func->common.scope ? ZSTR_VAL(func->common.scope->name) : NULL;
	}

	event.callstack = emalloc(sizeof(function_call) * callstack_size);
	event.callstack_size = callstack_size;
	memcpy(event.callstack, callstack, sizeof(function_call) * callstack_size);

	formatted_message *formatted = format_trace_event(&event);

	logger_write(formatted->buf, formatted->buf_size);
	logger_write("\n", strlen("\n"));
	logger_flush();
	
	free_formatted_message(formatted);
	free_trace_event(&event);
}

/* Observer initialization */
zend_observer_fcall_handlers pdotrace_observer_init(zend_execute_data *execute_data)
{
	zend_observer_fcall_handlers handlers = {0};

	/* Only observe if extension is enabled */
	if (!PDOTRACE_G(enabled)) {
		return handlers;
	}

	handlers.begin = pdotrace_observer_begin;
	handlers.end = pdotrace_observer_end;
	
	return handlers;
}

void pdotrace_register_observers()
{
	zend_observer_fcall_register(pdotrace_observer_init);
}
