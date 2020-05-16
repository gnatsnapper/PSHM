/* pshm extension for PHP */

#ifndef PHP_PSHM_H
# define PHP_PSHM_H

extern zend_module_entry pshm_module_entry;
# define phpext_pshm_ptr &pshm_module_entry

# define PHP_PSHM_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_PSHM)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

PHP_FUNCTION(pshm_info);
PHP_METHOD(PSHM, __construct);
PHP_METHOD(PSHM, unlink);

PHP_RINIT_FUNCTION(pshm);
PHP_RSHUTDOWN_FUNCTION(pshm);
PHP_MINIT_FUNCTION(pshm);
PHP_MSHUTDOWN_FUNCTION(pshm);
PHP_MINFO_FUNCTION(pshm);

typedef struct _php_pshm_obj php_pshm_obj;

struct shm_st {


}

struct _php_pshm_obj {
        char *name;
        shm_st *shm;
	zend_object   std;
};

static zend_object *pshm_object_init(zend_class_entry *class_type);
PHPAPI int php_pshm_initialize(php_pshm_obj *pshmobj, char *name, shm_st *shm);
static inline php_pshm_obj *php_pshm_obj_from_obj(zend_object *obj) {
	return (php_pshm_obj*)((char*)(obj) - XtOffsetOf(php_pshm_obj, std));
}

#define Z_PHPPSHM_P(zv)  php_pshm_obj_from_obj(Z_OBJ_P((zv)))

#endif	/* PHP_PSHM_H */
