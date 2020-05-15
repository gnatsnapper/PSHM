/* pshm extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_pshm.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

zend_class_entry *pshm_ce;
static zend_object_handlers pshm_object_handlers;

/* {{{ string pshm_info( string $name )
 */
PHP_FUNCTION(pshm_info)
{
	char *name = "World";
	size_t name_len = sizeof("World") - 1;
	zend_string *retval;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(name, name_len)
	ZEND_PARSE_PARAMETERS_END();

	retval = strpprintf(0, "%s", name);

	RETURN_STR(retval);
}
/* }}}*/

/* {{{ void pshm___construct( string $name )
 */
PHP_METHOD(PSHM,__construct)
{
        char *name;
        size_t name_len;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(name, name_len)
	ZEND_PARSE_PARAMETERS_END();

	php_pshm_initialize(Z_PHPPSHM_P(ZEND_THIS), name);
}
/* }}} */

/* {{{ bool Uring::unlink(  )
 */
PHP_METHOD(PSHM,unlink)
{

	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_TRUE;
}
/* }}}*/

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(pshm)
{
#if defined(ZTS) && defined(COMPILE_DL_PSHM)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pshm)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "pshm support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO(arginfo_pshm_info, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_pshm_class_construct, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_pshm_class_unlink, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ pshm_functions[]
 */
static const zend_function_entry pshm_functions[] = {
	PHP_FE(pshm_info,		arginfo_pshm_info)
	PHP_FE_END
};
/* }}} */

/* {{{ pshm_methods[]
 */
static const zend_function_entry pshm_methods[] = {
	PHP_ME(PSHM, __construct,	arginfo_pshm_class_construct, ZEND_ACC_PUBLIC)
	PHP_ME(PSHM, unlink,    arginfo_pshm_class_unlink, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

/* {{{ pshm_module_entry
 */
zend_module_entry pshm_module_entry = {
	STANDARD_MODULE_HEADER,
	"pshm",	/* Extension name */
        pshm_functions,			/* zend_function_entry */
	PHP_MINIT(pshm),			/* PHP_MINIT - Module initialization */
	NULL,					/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(pshm),			/* PHP_RINIT - Request initialization */
	NULL,					/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(pshm),			/* PHP_MINFO - Module info */
	PHP_PSHM_VERSION,/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PSHM
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(pshm)
#endif

/* {{{ PHP_MINIT_FUNCTION(pshm)
 */
PHP_MINIT_FUNCTION(pshm)
{
        zend_class_entry ce_pshm;

        INIT_CLASS_ENTRY(ce_pshm, "PSHM", pshm_methods);
	ce_pshm.create_object = pshm_object_init;
	pshm_ce = zend_register_internal_class_ex(&ce_pshm, NULL);
	memcpy(&pshm_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
//	pshm_object_handlers_pshm.offset = XtOffsetOf(php_pshm_obj, std);
//	pshm_object_handlers_pshm.free_obj = pshm_object_free_storage;
//	pshm_object_handlers_pshm.clone_obj = pshm_object_clone;
//	pshm_object_handlers_pshm.dtor_obj = pshm_object_destroy;
//	pshm_object_handlers_pshm.compare_objects = pshm_object_compare;
//	pshm_object_handlers_pshm.get_properties_for = pshm_object_get_properties_for;
//	pshm_object_handlers_pshm.get_gc = pshm_object_get_gc;

	return SUCCESS;

}
/* }}} */

/* {{{ pshm_object_init(ce)
 */
static zend_object *pshm_object_init(zend_class_entry *ce) /* {{{ */
{
	php_pshm_obj *intern = zend_object_alloc(sizeof(php_pshm_obj), ce);

	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);
	intern->std.handlers = &pshm_object_handlers;

	return &intern->std;
}
/* }}} */


/* {{{ php_pshm_initialize(*pshmobj, *name)
 */
PHPAPI int php_pshm_initialize(php_pshm_obj *pshmobj, /*const*/ char *name)
{
        pshmobj->name = name;

	return 1;
}
/* }}} */

