#include "trace_event.h"

void free_trace_event(trace_event *event) {
	if (!event) {
		return;
	}

	if (event->trace_id) {
		efree(event->trace_id);
	}

	if (event->endpoint) {
		efree(event->endpoint);
	}

	if (event->query) {
		efree(event->query);
	}

	if (event->callstack) {
		for (size_t i = 0; i < event->callstack_size; i++) {
			function_call *fc = &event->callstack[i];
			if (fc->function_name) {
				efree(fc->function_name);
			}
			if (fc->class_name) {
				efree(fc->class_name);
			}
			if (fc->file) {
				efree(fc->file);
			}
		}
		efree(event->callstack);
	}
}
