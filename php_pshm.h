/* pshm extension for PHP */

#ifndef PHP_PSHM_H
# define PHP_PSHM_H

extern zend_module_entry pshm_module_entry;
# define phpext_pvoid &pshm_module_entry

# define PHP_PSHM_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_PSHM)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

PHP_FUNCTION(pshm_info);
PHP_METHOD(PosixSharedMemory, __construct);
PHP_METHOD(PosixSharedMemory, read);
PHP_METHOD(PosixSharedMemory, write);
PHP_METHOD(PosixSharedMemory, unlink);

PHP_RINIT_FUNCTION(pshm);
PHP_RSHUTDOWN_FUNCTION(pshm);
PHP_MINIT_FUNCTION(pshm);
PHP_MSHUTDOWN_FUNCTION(pshm);
PHP_MINFO_FUNCTION(pshm);

typedef struct php_pshm_obj {
        char *name;
        char *shm;
        int size;
        int mflag;
	zend_object std;
} php_pshm_obj;

static zend_object *pshm_object_init(zend_class_entry *class_type);
PHPAPI int php_pshm_initialize(php_pshm_obj *pshmobj, char *name, char *shm, int size, int mflag);
static inline php_pshm_obj *php_pshm_obj_from_obj(zend_object *obj) {
	return (php_pshm_obj*)((char*)(obj) - XtOffsetOf(php_pshm_obj, std));
}

#define Z_PHPPSHM_P(zv)  php_pshm_obj_from_obj(Z_OBJ_P((zv)))

int set_map_flags(int oflag);
void * customShm(const char *name, int oflag, mode_t mode, int size);
void *defaultShm(const char *name);

#endif	/* PHP_PSHM_H */
