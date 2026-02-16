#include "trace_event.h"

void free_trace_event(trace_event *event) {
	if (!event) {
		return;
	}

	if (event->trace_id) {
		efree(event->trace_id);
		event->trace_id = NULL;
	}

	if (event->endpoint) {
		efree(event->endpoint);
		event->endpoint = NULL;
	}

	if (event->query) {
		efree(event->query);
		event->query = NULL;
	}

	if (event->params) {
		FREE_HASHTABLE(event->params);
		event->params = NULL;
	}

	if (event->callstack) {
		for (size_t i = 0; i < event->callstack_size; i++) {
			function_call *fc = &event->callstack[i];
			if (fc->function_name) {
				efree(fc->function_name);
				fc->function_name = NULL;
			}
			if (fc->class_name) {
				efree(fc->class_name);
				fc->class_name = NULL;
			}
			if (fc->file) {
				efree(fc->file);
				fc->file = NULL;
			}
		}
		efree(event->callstack);
		event->callstack = NULL;
	}
}
