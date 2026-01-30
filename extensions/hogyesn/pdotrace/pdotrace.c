/* pdotrace extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_pdotrace.h"
#include "pdotrace_arginfo.h"
#include "logger/logger.h"
#include "logger/file_logger.h"
#include "logger/trace_event.h"

/* Module globals */
ZEND_DECLARE_MODULE_GLOBALS(pdotrace)

/* INI entries */
PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("pdotrace.enabled", "0", PHP_INI_SYSTEM, OnUpdateBool, enabled, zend_pdotrace_globals, pdotrace_globals)
    STD_PHP_INI_ENTRY("pdotrace.log_file_path", "/tmp/pdotrace.log", PHP_INI_SYSTEM, OnUpdateString, log_file_path, zend_pdotrace_globals, pdotrace_globals)
PHP_INI_END()

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

PHP_FUNCTION(pdotrace_log_file_path)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_printf("%s\n", PDOTRACE_G(log_file_path));
}

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(pdotrace)
{
#if defined(ZTS) && defined(COMPILE_DL_PDOTRACE)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	PDOTRACE_G(trace_id) = emalloc(33);
	for (int i = 0; i < 16; i++) {
		sprintf(PDOTRACE_G(trace_id) + i * 2, "%02x", (unsigned char) (rand() & 0xff));
	}
	PDOTRACE_G(trace_id)[32] = '\0';
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(pdotrace)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "pdotrace support", "enabled");
	php_info_print_table_row(
		2,
		"pdotrace.log_file_path",
		PDOTRACE_G(log_file_path)
	);
	php_info_print_table_end();
}
/* }}} */

/* Module globals initialization */
static void php_pdotrace_init_globals(zend_pdotrace_globals *pdotrace_globals)
{
	pdotrace_globals->enabled = 0;
	pdotrace_globals->log_file_path = NULL;
	pdotrace_globals->trace_id = NULL;
}

/* Module initialization */
PHP_MINIT_FUNCTION(pdotrace)
{
	ZEND_INIT_MODULE_GLOBALS(pdotrace, php_pdotrace_init_globals, NULL);
	REGISTER_INI_ENTRIES();

	set_logger(file_logger_get_methods());
	logger_init();

	if (PDOTRACE_G(enabled)) {
		pdotrace_register_observers();
	}

	srand((unsigned int) time(NULL) ^ getpid());

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(pdotrace)
{
	logger_flush();
	logger_shutdown();

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(pdotrace)
{
	if (PDOTRACE_G(trace_id)) {
		efree(PDOTRACE_G(trace_id));
		PDOTRACE_G(trace_id) = NULL;
	}

	if (prepared_event) {
		free_trace_event(prepared_event);
		prepared_event = NULL;
	}

	return SUCCESS;
}


/* {{{ pdotrace_module_entry */
zend_module_entry pdotrace_module_entry = {
	STANDARD_MODULE_HEADER,
	PHP_PDOTRACE_EXTNAME,			/* Extension name */
	ext_functions,					/* zend_function_entry */
	PHP_MINIT(pdotrace),			/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(pdotrace),		/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(pdotrace),			/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(pdotrace),		/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(pdotrace),			/* PHP_MINFO - Module info */
	PHP_PDOTRACE_VERSION,			/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDOTRACE
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(pdotrace)
#endif
