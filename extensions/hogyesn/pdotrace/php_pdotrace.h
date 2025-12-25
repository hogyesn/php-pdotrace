/* pdotrace extension for PHP */

#ifndef PHP_PDOTRACE_H
# define PHP_PDOTRACE_H

extern zend_module_entry pdotrace_module_entry;
# define phpext_pdotrace_ptr &pdotrace_module_entry

# define PHP_PDOTRACE_VERSION "0.1.0"
# define PHP_PDOTRACE_EXTNAME "pdotrace"

/* Module globals */
ZEND_BEGIN_MODULE_GLOBALS(pdotrace)
    char *log_file_path;
ZEND_END_MODULE_GLOBALS(pdotrace)

ZEND_EXTERN_MODULE_GLOBALS(pdotrace)

#define PDOTRACE_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(pdotrace, v)


# if defined(ZTS) && defined(COMPILE_DL_PDOTRACE)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_PDOTRACE_H */
