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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/basic_functions.h"
#include "php_sqreen.h"

/* If you declare any globals in php_sqreen.h uncomment this:
*/
ZEND_DECLARE_MODULE_GLOBALS(sqreen)

/* This table make the link between original functions and overloadings functions
 * { "original function name", "overloading function name", original zend_function address,
     overloading binary function address }
 */
static overloading_table _overloading_func_table[] = {
    { "fopen", "sqreen_fopen", NULL, NULL, sqreen_fopen },
    { NULL, NULL, NULL, NULL, NULL }
};

#define SIZE_OF_OVERLOADING_FUNC_TABLE ( sizeof(_overloading_func_table) / \
                                         sizeof(overloading_table) )

/* sqreen_fopen is the overloading function of the standard fopen function
 */
/* {{{ PHP_NAMED_FUNCTION(sqreen_fopen)
 */
PHP_NAMED_FUNCTION(sqreen_fopen) {
    overloading_table *overloading_func_table = SQREEN_G(overloading_func_table);
    char              *filename = NULL, *mode = NULL;
    size_t             filename_len = 0, mode_len = 0;
    zend_bool          use_include_path = 0;
    zval *             zcontext = NULL;
    int                j        = 0;

    char log_msg[255];

    ZEND_PARSE_PARAMETERS_START(2, 4)
        Z_PARAM_PATH(filename, filename_len)
        Z_PARAM_STRING(mode, mode_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(use_include_path)
        Z_PARAM_RESOURCE(zcontext)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    snprintf(log_msg, 255, "fopen('%s', '%s'[, %d, %#lx])", filename, mode, use_include_path, zcontext);

    /* Log the function call */
    _php_error_log_ex(0, log_msg, 0, NULL, NULL);

    /* Call the original function */
    for (j = 0; j < SIZE_OF_OVERLOADING_FUNC_TABLE - 1; j++ ) {
        if ( strcmp(overloading_func_table[j].overloading_fname, "sqreen_fopen") == 0 ) {
            if ( overloading_func_table[j].handler )
                overloading_func_table[j].handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
        }
    }
}
/* }}} */

/* {{{ fill_overloading_func_table
 */
void fill_overloading_func_table() {
    overloading_table *overloading_func_table = SQREEN_G(overloading_func_table);
    HashTable         *target_function_table  = CG(function_table);
    zend_function     *orig_f     = NULL;
    size_t             fname_len  = 0;
    int                j          = 0;

    for (j = 0; j < SIZE_OF_OVERLOADING_FUNC_TABLE - 1; j++ ) {
        fname_len = strlen(overloading_func_table[j].fname);
        orig_f = zend_hash_str_find_ptr(target_function_table, overloading_func_table[j].fname, fname_len);

        if ( orig_f && overloading_func_table[j].overloading_handler ) {
            overloading_func_table[j].zf = orig_f;
            overloading_func_table[j].handler = orig_f->internal_function.handler;
        }
    }
}
/* }}} */

/* Overload some standard functions with custom functions which make some
 * additionnal checks and logs
 */
/* {{{ overload_functions
 */
void overload_functions() {
    overloading_table *overloading_func_table = SQREEN_G(overloading_func_table);
    zend_function *orig_f     = NULL;
    int            j          = 0;

    for (j = 0; j < SIZE_OF_OVERLOADING_FUNC_TABLE - 1; j++ ) {
        orig_f = overloading_func_table[j].zf;

        if ( orig_f && overloading_func_table[j].overloading_handler ) {
            orig_f->internal_function.handler = overloading_func_table[j].overloading_handler;
        }
    }
}
/* }}} */

/* Restore the functions overloaded
 */
/* {{{ */
void restore_functions() {
    overloading_table *overloading_func_table = SQREEN_G(overloading_func_table);
    zend_function *orig_f     = NULL;
    int            j          = 0;

    for ( j = 0; j < SIZE_OF_OVERLOADING_FUNC_TABLE - 1; j++ ) {
        orig_f = overloading_func_table[j].zf;

        if ( orig_f && overloading_func_table[j].handler )
            orig_f->internal_function.handler = overloading_func_table[j].handler;
    }
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION
*/
PHP_GINIT_FUNCTION(sqreen)
{
#if defined(COMPILE_DL_SQREEN) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif
    sqreen_globals->enable = 0;
    sqreen_globals->overloading_func_table = NULL;
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION
*/
PHP_GSHUTDOWN_FUNCTION(sqreen)
{
    sqreen_globals->enable = 0;
    sqreen_globals->overloading_func_table = NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(sqreen)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(sqreen)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(sqreen)
{
    int j;
#if defined(COMPILE_DL_SQREEN) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif
    SQREEN_G(overloading_func_table) = emalloc(sizeof(_overloading_func_table));
    if ( SQREEN_G(overloading_func_table) == NULL)
        return FAILURE;

    memcpy(SQREEN_G(overloading_func_table), _overloading_func_table,
           sizeof(_overloading_func_table));
    fill_overloading_func_table();

    SQREEN_G(enable) = 0;
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(sqreen)
{
    SQREEN_G(enable) = 0;
    efree(SQREEN_G(overloading_func_table));

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
*/
PHP_MINFO_FUNCTION(sqreen)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "sqreen support", "enabled");
    php_info_print_table_end();

    //DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_FUNCTION sqreenon
 */
PHP_FUNCTION(sqreenon)
{
    if ( SQREEN_G(enable) == 1 )
        RETURN_LONG(0);

    SQREEN_G(enable) = 1;
    overload_functions();

    RETURN_LONG(1);
}
/* }}} */

/* {{{ PHP_FUNCTION sqreenoff
 */
PHP_FUNCTION(sqreenoff)
{
    if ( SQREEN_G(enable) == 0 )
        RETURN_LONG(0);

    SQREEN_G(enable) = 0;
    restore_functions();

    RETURN_LONG(1);
}
/* }}} */

/* {{{ sqreen_functions[]
 *
 * Every user visible function must have an entry in sqreen_functions[].
 */
const zend_function_entry sqreen_functions[] = {
    PHP_FE(sqreenon, NULL)
    PHP_FE(sqreenoff, NULL)
    PHP_FE_END  /* Must be the last line in sqreen_functions[] */
};
/* }}} */

/* {{{ sqreen_module_entry
 */
zend_module_entry sqreen_module_entry = {
    STANDARD_MODULE_HEADER,
    "sqreen",
    sqreen_functions,
    PHP_MINIT(sqreen),
    PHP_MSHUTDOWN(sqreen),
    PHP_RINIT(sqreen),      /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(sqreen),  /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(sqreen),
    PHP_SQREEN_VERSION,
    PHP_MODULE_GLOBALS(sqreen),
    PHP_GINIT(sqreen),
    PHP_GSHUTDOWN(sqreen),
    NULL,
    STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_SQREEN
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(sqreen)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
