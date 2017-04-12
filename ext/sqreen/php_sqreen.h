/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Gaétan Njinang (nuggets)                                     |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_SQREEN_H
#define PHP_SQREEN_H

#include "php_version.h"
#define PHP_SQREEN_VERSION PHP_VERSION /* Replace with version number for your extension */

extern zend_module_entry sqreen_module_entry;
#define phpext_sqreen_ptr &sqreen_module_entry

#ifdef PHP_WIN32
#	define PHP_SQREEN_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_SQREEN_API __attribute__ ((visibility("default")))
#else
#	define PHP_SQREEN_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

typedef struct _overloading_table {
	const char *fname;
	const char *overloading_fname;
	void (*handler)(INTERNAL_FUNCTION_PARAMETERS);
	void (*overloading_handler)(INTERNAL_FUNCTION_PARAMETERS);
} overloading_table;

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:
*/
ZEND_BEGIN_MODULE_GLOBALS(sqreen)
    zend_long  enable;
ZEND_END_MODULE_GLOBALS(sqreen)


/* Always refer to the globals in your function as SQREEN_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define SQREEN_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(sqreen, v)

#if defined(ZTS) && defined(COMPILE_DL_SQREEN)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

PHP_FUNCTION(sqreenon);
PHP_FUNCTION(sqreenoff);
PHP_NAMED_FUNCTION(sqreen_fopen);

#endif	/* PHP_SQREEN_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
