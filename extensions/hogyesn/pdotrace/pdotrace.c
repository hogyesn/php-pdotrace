/* pdotrace extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_pdotrace.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif


/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(pdotrace)
{
#if defined(ZTS) && defined(COMPILE_DL_PDOTRACE)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(pdotrace)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "pdotrace support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ pdotrace_module_entry */
zend_module_entry pdotrace_module_entry = {
	STANDARD_MODULE_HEADER,
	"pdotrace",					/* Extension name */
	NULL,							/* zend_function_entry */
	NULL,							/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(pdotrace),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(pdotrace),			/* PHP_MINFO - Module info */
	PHP_PDOTRACE_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDOTRACE
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(pdotrace)
#endif
