/* pdotrace extension for PHP */

#ifndef PHP_PDOTRACE_H
# define PHP_PDOTRACE_H

#include "logger/logger.h"
#include "logger/trace_event.h"

extern zend_module_entry pdotrace_module_entry;
# define phpext_pdotrace_ptr &pdotrace_module_entry

# define PHP_PDOTRACE_VERSION "0.1.0"
# define PHP_PDOTRACE_EXTNAME "pdotrace"

/* Module globals */
ZEND_BEGIN_MODULE_GLOBALS(pdotrace)
	zend_bool enabled;
    char *log_file_path;
	char *trace_id;
ZEND_END_MODULE_GLOBALS(pdotrace)

ZEND_EXTERN_MODULE_GLOBALS(pdotrace)

#define PDOTRACE_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(pdotrace, v)


# if defined(ZTS) && defined(COMPILE_DL_PDOTRACE)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

extern const logger_methods *file_logger_get_methods(void);
ZEND_TLS trace_event *prepared_event = NULL;

/* Observer functions */
void pdotrace_register_observers(void);

#endif	/* PHP_PDOTRACE_H */
