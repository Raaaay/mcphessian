/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header,v 1.16.2.1.2.1 2007/01/01 19:32:09 iliaa Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_mcphessian.h"

/* If you declare any globals in php_mcphessian.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(mcphessian)
*/

/* True global resources - no need for thread safety here */
static int le_mcphessian;

zend_class_entry *mcphessian_ce_ptr;

/* {{{ mcphessian_functions[]
 *
 * Every user visible function must have an entry in mcphessian_functions[].
 */
zend_function_entry mcphessian_functions[] = {
	PHP_FE(confirm_mcphessian_compiled,	NULL)		/* For testing, remove later. */
	{NULL, NULL, NULL}	/* Must be the last line in mcphessian_functions[] */
};
/* }}} */

/* {{{ mcphessian_module_entry
 */
zend_module_entry mcphessian_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"mcphessian",
	mcphessian_functions,
	PHP_MINIT(mcphessian),
	PHP_MSHUTDOWN(mcphessian),
	PHP_RINIT(mcphessian),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(mcphessian),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(mcphessian),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MCPHESSIAN
ZEND_GET_MODULE(mcphessian)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("mcphessian.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_mcphessian_globals, mcphessian_globals)
    STD_PHP_INI_ENTRY("mcphessian.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_mcphessian_globals, mcphessian_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_mcphessian_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_mcphessian_init_globals(zend_mcphessian_globals *mcphessian_globals)
{
	mcphessian_globals->global_value = 0;
	mcphessian_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ constructor, create one new instance like this:
 * $client = new McpackHessianClient('http://xxxx', array('xxx' => 'xxx')),
 * method's declaration is McpackHessianClient($strUrl, $arrOptions).
 **/
PHP_METHOD(McpackHessianClient, __construct) {
	zend_class_entry *ce;
	zval *url, *options, *p_this;

	MAKE_STD_ZVAL(options);
	array_init(options);
	p_this = getThis();

    if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, p_this,
				"Oz|z", &ce, mcphessian_ce_ptr, &url, &options) == FAILURE) {
		return;
	}
	// set the properties of instance
	zend_update_property(mcphessian_ce_ptr, p_this, ZEND_STRL("url"), url TSRMLS_CC);
	zend_update_property(mcphessian_ce_ptr, p_this, ZEND_STRL("options"), options TSRMLS_CC);
}
/* }}} */

PHP_METHOD(McpackHessianClient, getUrl) {
	zval *url, *p_this;
	
	p_this = getThis();
	url = zend_read_property(mcphessian_ce_ptr, p_this, ZEND_STRL("url"), 1 TSRMLS_CC);
	RETURN_ZVAL(url, 1, 0);
}

static zval *mcpack2array(zval *data) {
	zval *params[] = { data };
	zval *function_name, *retval_ptr;

	MAKE_STD_ZVAL(retval_ptr);
	MAKE_STD_ZVAL(function_name);
	ZVAL_STRING(function_name, "mc_pack_pack2array", 0);
	if (call_user_function(CG(function_table), NULL, function_name, 
		retval_ptr, 1, params TSRMLS_CC) == SUCCESS) {
	} else {
		php_error(E_WARNING, "call function mc_pack_pack2array fail.");
	}
	return retval_ptr;
}

static zval *array2mcpack(zval *data) {
	if (Z_TYPE_P(data) != IS_ARRAY) {
		php_error(E_ERROR, "parameter type should be array.");
	}
	
	zval *mc_pack_v;
	MAKE_STD_ZVAL(mc_pack_v);
	ZVAL_STRING(mc_pack_v, "PHP_MC_PACK_V2", 0);
	zval *params[] = { data, mc_pack_v };
	zval *function_name, *retval_ptr;
	MAKE_STD_ZVAL(function_name);
	ZVAL_STRING(function_name, "mc_pack_array2pack", 0);
	MAKE_STD_ZVAL(retval_ptr);

	if (call_user_function(CG(function_table), NULL, function_name, 
		retval_ptr, 1, params TSRMLS_CC) == SUCCESS) {
	} else {
		php_error(E_WARNING, "call function mc_pack_array2pack fail.");
	}
	return retval_ptr;
}

/**
 * override __call()
 * it require two parameters, func_name and args
 **/
PHP_METHOD(McpackHessianClient, __call) {
	zend_class_entry *ce;
	zval *p_this, *args, *params, *result, *method, *tmp;
	char *func_name, *ret_str = NULL;
	int func_name_len = 0;
	size_t return_len = 0, max_len = 1024 * 1024 * 1024;

	p_this = getThis();
	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, p_this, 
		"Osz", &ce, mcphessian_ce_ptr, &func_name, &func_name_len, &args) == FAILURE) {
		php_error(E_ERROR, "parse parameters error.");
		return;
	}

	// init params
	array_init(params);
	add_assoc_string(params, "jsonrpc", "2.0", 0);
	add_assoc_string(params, "method", func_name, 0);
	add_assoc_zval(params, "params", args);
	add_assoc_string(params, "id", "123456", 0);
	zval *pack = array2mcpack(params);

	// post data
	zval *z_url = zend_read_property(mcphessian_ce_ptr, p_this, ZEND_STRL("url"), 1 TSRMLS_CC);
	convert_to_string(z_url);
	char *url = Z_STRVAL_P(z_url);
	php_stream_context *context = php_stream_context_alloc();
	MAKE_STD_ZVAL(method);
	ZVAL_STRING(method, "POST", 0);
	php_stream_context_set_option(context, "http", "method", method);
	php_stream_context_set_option(context, "http", "content", pack);

	// read data from stream
	php_stream *stream = php_stream_open_wrapper_ex(url, "rb", REPORT_ERRORS, NULL, context);
	ret_str = php_stream_get_line(stream, NULL, max_len, &return_len);
	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, ret_str, return_len, 0);
	result = mcpack2array(tmp);
	php_stream_close(stream);
	efree(ret_str);

	// get result from array
	zval **ret_val;
	if (zend_hash_exists(Z_ARRVAL_P(result), "result", 7)) {
		zend_hash_find(Z_ARRVAL_P(result), "result", 7, (void **)&ret_val);
		RETURN_ZVAL(*ret_val, 1, 0);
	} else {
		php_error(E_ERROR, "return value illegal.");
		RETURN_NULL();
	}
}

zend_function_entry mcphessian_methods[] = {
	PHP_ME(McpackHessianClient, __construct, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(McpackHessianClient, getUrl, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(McpackHessianClient, __call, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(mcphessian)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "McpackHessianClient", mcphessian_methods);
	mcphessian_ce_ptr = zend_register_internal_class(&ce);
	zend_declare_property_null(mcphessian_ce_ptr, ZEND_STRL("url"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(mcphessian_ce_ptr, ZEND_STRL("options"), ZEND_ACC_PROTECTED TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(mcphessian)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(mcphessian)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(mcphessian)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mcphessian)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "mcphessian support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_mcphessian_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_mcphessian_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "mcphessian", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */

/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
