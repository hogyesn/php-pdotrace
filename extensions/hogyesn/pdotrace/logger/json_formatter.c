#include "formatter.h"

#define ADD_PARAM_TO_JSON_ASSOC(key, value) \
	offset += snprintf(message->buf + offset, estimated_size - offset, \
					"\"%s\": \"%s\",", \
					key, \
					value)

#define ADD_PARAM_TO_JSON_PACKED(value) \
	offset += snprintf(message->buf + offset, estimated_size - offset, \
					"\"%s\",", \
					value)

#define ADD_PARAM_TO_JSON(key, value) \
		if (is_packed) { \
			ADD_PARAM_TO_JSON_PACKED(value); \
		} else { \
			ADD_PARAM_TO_JSON_ASSOC(key, value); \
		} 

formatted_message *format_trace_event(const trace_event *event) {
	// Estimate required buffer size
	size_t estimated_size = 256; // Base size
	estimated_size += event->query ? strlen(event->query) : 0;
	estimated_size += event->callstack_size * 128; // Rough estimate per callstack entry
	// add extra space for params if needed
	if (event->params) {
		estimated_size += zend_hash_num_elements(event->params) * 64; // Rough estimate per param
	}

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

	offset += snprintf(message->buf + offset, estimated_size - offset, "]");

	// Format parameters if available
	if (event->params) {
		offset += snprintf(message->buf + offset, estimated_size - offset, ", \"params\": ");
		bool is_packed = HT_IS_PACKED(event->params);
		if (is_packed) {
			offset += snprintf(message->buf + offset, estimated_size - offset, "[");
		} else {
			offset += snprintf(message->buf + offset, estimated_size - offset, "{");
		}
		zend_string *key;
		zval *value;
		zend_ulong idx;
		ZEND_HASH_FOREACH_KEY_VAL(event->params, idx, key, value) {
		
			if (is_packed) {
				key = zend_ulong_to_str(idx);
			}
			switch (Z_TYPE_P(value)) {
				case IS_STRING:
					ADD_PARAM_TO_JSON(ZSTR_VAL(key), Z_STRVAL_P(value));
					break;
				case IS_LONG:
					ADD_PARAM_TO_JSON(ZSTR_VAL(key), ZSTR_VAL(zend_long_to_str(Z_LVAL_P(value))));
					break;
				case IS_DOUBLE:
					ADD_PARAM_TO_JSON(ZSTR_VAL(key), ZSTR_VAL(zend_double_to_str(Z_DVAL_P(value))));
					break;
				case IS_NULL:
					ADD_PARAM_TO_JSON(ZSTR_VAL(key), "null");
					break;
				case IS_TRUE:
					ADD_PARAM_TO_JSON(ZSTR_VAL(key), "true");
					break;
				case IS_FALSE:
					ADD_PARAM_TO_JSON(ZSTR_VAL(key), "false");
					break;
				default:
					offset += snprintf(message->buf + offset, estimated_size - offset,
								"\"%s\": \"(type %d)\",",
								ZSTR_VAL(key),
								Z_TYPE_P(value));
					break;
			}
		} ZEND_HASH_FOREACH_END();

		zend_string_release(key);

		// remove trailing comma if needed
		if (message->buf[offset - 1] == ',') {
			offset--; // remove last comma
		}
		if (is_packed) {
			offset += snprintf(message->buf + offset, estimated_size - offset, "]");
		} else {
			offset += snprintf(message->buf + offset, estimated_size - offset, "}");
		}
	}

	// Close JSON
	offset += snprintf(message->buf + offset, estimated_size - offset, " }");

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
