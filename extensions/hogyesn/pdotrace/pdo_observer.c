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
	
	if (PDOTRACE_G(log_file_path)) {
		FILE *log_file = fopen(PDOTRACE_G(log_file_path), "a");
		if (log_file) {
			fprintf(log_file, "PDO function executed: %s\n", execute_data->func->common.function_name->val);
			fclose(log_file);
		} else {
			php_error_docref(NULL, E_WARNING, "Failed to open log file: %s", PDOTRACE_G(log_file_path));
		}
	}
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
	