#include "php.h"
#include "zend_observer.h"

#include "php_pdotrace.h"

/* Observer begin handler */
void pdotrace_observer_begin(zend_execute_data *execute_data)
{
	if (!ZEND_OBSERVER_ENABLED) {
		return;
	}

	if (!pdotrace_log_fp) {
		return;
	}
}

/* Observer end handler */
void pdotrace_observer_end(zend_execute_data *execute_data, zval *retval)
{
	if (!ZEND_OBSERVER_ENABLED) {
		return;
	}

	if (!pdotrace_log_fp) {
		return;
	}

	zend_execute_data *frame = execute_data;
	int depth = 0;

	zend_function *func = frame->func;

	if (!func) {
		return;
	}

	const char *func_name = ZSTR_VAL(func->common.function_name);
	const char *class_name = func->common.scope ? ZSTR_VAL(func->common.scope->name) : NULL;

	if (!class_name || class_name && strcmp(class_name, "PDO") != 0) {
		// Skip logging non-PDO frames
		return;
	}

	fprintf(pdotrace_log_fp, "--------------------------\n");
	fprintf(pdotrace_log_fp, "function executed: %s\n", func_name);

	// Log call stack
	while (frame && depth++ < 10) {
		if (func->common.function_name) {
			if (class_name) {
				fprintf(pdotrace_log_fp, "Frame: %s::%s\n", class_name, func_name);
			} else {
				fprintf(pdotrace_log_fp, "Frame: %s\n", func_name);
			}

			int argc = ZEND_CALL_NUM_ARGS(frame);
			for (int i = 0; i < argc; i++) {
				zval *arg = ZEND_CALL_ARG(frame, i + 1);
				if (arg && Z_TYPE_P(arg) != IS_UNDEF) {
					zend_string *arg_str = zval_get_string(arg);
					fprintf(pdotrace_log_fp, "  Arg[%d]: %s\n", i, ZSTR_VAL(arg_str));
					zend_string_release(arg_str);
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
	fprintf(pdotrace_log_fp, "--------------------------\n");
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
	