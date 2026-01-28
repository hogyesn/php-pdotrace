#include "formatter.h"

formatted_message *format_trace_event(const trace_event *event) {
	// Estimate required buffer size
	size_t estimated_size = 256; // Base size
	estimated_size += event->query ? strlen(event->query) : 0;
	estimated_size += event->callstack_size * 128; // Rough estimate per callstack entry

	// Allocate formatted_message
	formatted_message *message = emalloc(sizeof(formatted_message));
	message->buf = emalloc(estimated_size);
	message->buf_size = 0;

	// Start formatting JSON
	size_t offset = 0;
	offset += snprintf(message->buf + offset, estimated_size - offset,
					"{ \"trace_id\": \"%s\", \"endpoint\": \"%s\", \"timestamp\": %ld, \"runtime\": %.3f, \"query\":\"%s\", \"callstack\": [",
					event->trace_id,
					event->endpoint,
					event->timestamp,
					event->runtime,
					event->query ? event->query : ""
				);

	// Format callstack
	for (size_t i = 0; i < event->callstack_size; i++) {
		function_call *fc = &event->callstack[i];
		offset += snprintf(message->buf + offset, estimated_size - offset,
						"%s{\"function_name\": \"%s\", \"class_name\": \"%s\", \"file\": \"%s\", \"line\": %d}",
						(i > 0) ? "," : "",
						fc->function_name ? fc->function_name : "",
						fc->class_name ? fc->class_name : "",
						fc->file ? fc->file : "",
						fc->line);
	}

	// Close JSON
	offset += snprintf(message->buf + offset, estimated_size - offset, "] }");

	message->buf_size = offset;
	return message;
}

void free_formatted_message(formatted_message *message) {
	if (!message) {
		return;
	}

	if (message->buf) {
		efree(message->buf);
	}

	efree(message);
}

