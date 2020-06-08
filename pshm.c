/* pshm extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"

#include "zend_exceptions.h"

#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "php_pshm.h"

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

    zend_string *name;
    int retval, fd;
    struct stat fd_stat;

    ZEND_PARSE_PARAMETERS_START(1, 1)
         Z_PARAM_STR(name)
    ZEND_PARSE_PARAMETERS_END();

    if((fd = shm_open(ZSTR_VAL(name),O_RDONLY,0600)) == -1)
    {
        zend_throw_exception (zend_ce_exception, strerror (errno), 0);
    }

    retval = fstat (fd, &fd_stat);

    if (retval == -1)
    {
      zend_throw_exception (zend_ce_exception, strerror (errno), 0);
    }

    retval = close (fd);

    if (retval == -1)
    {
      zend_throw_exception (zend_ce_exception, strerror (errno), 0);
    }

    array_init (return_value);
    add_assoc_long (return_value, "size", fd_stat.st_size);
    add_assoc_long (return_value, "atime", fd_stat.st_atim.tv_sec);
    add_assoc_long (return_value, "mtime", fd_stat.st_mtim.tv_sec);
    add_assoc_long (return_value, "ctime", fd_stat.st_ctim.tv_sec);

}
/* }}}*/

/* {{{ void pshm___construct( string $name [, string $flags [, int $mode [, int size ]]] )
 */
PHP_METHOD(PSHM,__construct)
{
        zend_string *name;
        zend_long flags = -1;
        int mflag;
        char *shm;
        int buffer_length = 128;
        zend_long mode = 0666;
        zend_long size = 1024;


	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STR(name)
                Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
		Z_PARAM_LONG(mode)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

        if(flags == -1)
        {

            shm = defaultShm(ZSTR_VAL(name));

        }
        else
        {
            shm = customShm(ZSTR_VAL(name), (int)flags, mode, (int)size);

        }

        mflag = set_map_flags(flags);

	php_pshm_initialize(Z_PHPPSHM_P(ZEND_THIS), ZSTR_VAL(name), shm, (int)size, mflag);

}
/* }}} */

/* {{{ string PSHM::read(  )
 */
PHP_METHOD(PSHM,read)
{
    php_pshm_obj *pshmobj;

    ZEND_PARSE_PARAMETERS_NONE();


    pshmobj = Z_PHPPSHM_P(getThis());

    RETURN_STRING(pshmobj->shm);

}

/* }}} */

/* {{{ bool PSHM::write( string $message  )
 */
PHP_METHOD(PSHM,write)
{
    php_pshm_obj *pshmobj;
    int retval;
    zend_string *message;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(message)
    ZEND_PARSE_PARAMETERS_END();

    pshmobj = Z_PHPPSHM_P(getThis());

    if(pshmobj->mflag == 1)//Check if PROT_READ only
    {
        zend_throw_exception (zend_ce_exception, "Bad permissions for mapped region", 0);
        RETURN_FALSE;
    }

    if((ZSTR_LEN(message) * sizeof(char)) > (int)pshmobj->size)//Check if will fit in mapped region
    {
       zend_throw_exception (zend_ce_exception, "Message too large for mapped region", 0);
       RETURN_FALSE;
    }

    strcpy(pshmobj->shm,ZSTR_VAL(message));

    RETURN_TRUE;

}

/* }}} */

/* {{{ array PSHM::info(  )
 */
PHP_METHOD(PSHM,info)
{
    php_pshm_obj *pshmobj;
    int fd;
    struct stat fd_stat;
    int retval;

    ZEND_PARSE_PARAMETERS_NONE();

    pshmobj = Z_PHPPSHM_P(getThis());

    if((fd = shm_open(pshmobj->name,O_RDONLY,0666)) == -1)
    {
        zend_throw_exception (zend_ce_exception, strerror (errno), 0);
    }

    retval = fstat (fd, &fd_stat);

    if (retval == -1)
    {
      zend_throw_exception (zend_ce_exception, strerror (errno), 0);
    }

    retval = close (fd);

    if (retval == -1)
    {
      zend_throw_exception (zend_ce_exception, strerror (errno), 0);
    }

    array_init (return_value);
    add_assoc_long (return_value, "size", fd_stat.st_size);
    add_assoc_long (return_value, "atime", fd_stat.st_atim.tv_sec);
    add_assoc_long (return_value, "mtime", fd_stat.st_mtim.tv_sec);
    add_assoc_long (return_value, "ctime", fd_stat.st_ctim.tv_sec);

}
/* }}}*/

/* {{{ bool PSHM::unlink(  )
 */
PHP_METHOD(PSHM,unlink)
{
    php_pshm_obj *pshmobj;
    int retval;

    ZEND_PARSE_PARAMETERS_NONE();

    pshmobj = Z_PHPPSHM_P(getThis());

    retval = shm_unlink(pshmobj->name);

    if(retval == -1)
    {
       if(errno == ENOENT){ RETURN_FALSE; }
       zend_throw_exception (zend_ce_exception, strerror (errno), 0);
    }

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
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, buffer_length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_pshm_class_write, 0)
	ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_pshm_class_read, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_pshm_class_info, 0)
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
	PHP_ME(PSHM, __construct, arginfo_pshm_class_construct, ZEND_ACC_PUBLIC)
	PHP_ME(PSHM, unlink,      arginfo_pshm_class_unlink, ZEND_ACC_PUBLIC)
	PHP_ME(PSHM, read,        arginfo_pshm_class_read, ZEND_ACC_PUBLIC)
	PHP_ME(PSHM, write,       arginfo_pshm_class_write, ZEND_ACC_PUBLIC)
	PHP_ME(PSHM, info,        arginfo_pshm_class_info, ZEND_ACC_PUBLIC)
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
	//pshm_ce = zend_register_internal_class_ex(&ce_pshm, NULL);
	pshm_ce = zend_register_internal_class(&ce_pshm);
	memcpy(&pshm_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	pshm_object_handlers.offset = XtOffsetOf(php_pshm_obj, std);
//	pshm_object_handlers_pshm.free_obj = pshm_object_free_storage;
//	pshm_object_handlers_pshm.clone_obj = pshm_object_clone;
//	pshm_object_handlers_pshm.dtor_obj = pshm_object_destroy;
//	pshm_object_handlers_pshm.compare_objects = pshm_object_compare;
//	pshm_object_handlers_pshm.get_properties_for = pshm_object_get_properties_for;
//	pshm_object_handlers_pshm.get_gc = pshm_object_get_gc;

        REGISTER_LONG_CONSTANT("PSHM_CREAT", O_CREAT, CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("PSHM_RDWR", O_RDWR, CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("PSHM_RDONLY", O_RDONLY, CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("PSHM_EXCL", O_EXCL, CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("PSHM_TRUNC", O_TRUNC, CONST_CS | CONST_PERSISTENT);

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

PHPAPI int php_pshm_initialize(php_pshm_obj *pshmobj, /*const*/ char *name, char *shm, int size, int mflag)
{
        pshmobj->name = name;
        pshmobj->shm = shm;
        pshmobj->size = size;
        pshmobj->mflag = mflag;

	return 1;
}
/* }}} */

/* {{{ set_map_flags(int oflag)
 */

int set_map_flags(int oflag)
{

    switch(oflag)
    {
        case O_RDONLY: return PROT_READ;
        case O_RDONLY|O_CREAT: return PROT_READ;
        case O_RDONLY|O_CREAT|O_EXCL: return PROT_READ;
        case O_RDONLY|O_CREAT|O_EXCL|O_TRUNC: return PROT_READ;
        default: return PROT_READ|PROT_WRITE;
    }

}

/* }}} */

/* {{{ defaultShm(const char *name, int *shm)
 */

void * defaultShm(const char *name)
{

    int shm_fd;
    void * shm;
    struct stat fd_stat;
    int retval;

    if((shm_fd = shm_open(name, O_EXCL | O_CREAT | O_RDWR, 0666))  == -1) //cannot create new shm
    {
        if(errno == EEXIST) //because one already exists
        {

            if((shm_fd = shm_open(name, O_RDWR, 0666)) == -1)
            {
                zend_throw_exception (zend_ce_exception, strerror (errno), 0);
            }

            retval = fstat(shm_fd, &fd_stat);

            if (retval == -1)
            {
              zend_throw_exception (zend_ce_exception, strerror (errno), 0);
            }

            if((shm = mmap(NULL, fd_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, (off_t)0)) == MAP_FAILED)
            {
                zend_throw_exception (zend_ce_exception, strerror (errno), 0);
            }

            if(close(shm_fd) == -1)
            {
                zend_throw_exception (zend_ce_exception, strerror (errno), 0);
            }

        }
        else //or for another reason
        {
            zend_throw_exception (zend_ce_exception, strerror (errno), 0);
        }

    }
    else //new shm created
    {

        if((ftruncate(shm_fd, (off_t)1024)) == -1)
        {
            zend_throw_exception (zend_ce_exception, strerror (errno), 0);
        }

        if((shm = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, (off_t)0)) == MAP_FAILED)
        {
            zend_throw_exception (zend_ce_exception, strerror (errno), 0);
        }

        if(close(shm_fd) == -1)
        {
            zend_throw_exception (zend_ce_exception, strerror (errno), 0);
        }

    }

    return shm;
}

/* }}} */

/* {{{ customShm(const char *name, int oflag, mode_t mode, size_t size)
 */
void * customShm(const char *name, int oflag, mode_t mode, int size)
{
    int shm_fd;
    void *shm;
    int mflag;


    if((shm_fd = shm_open(name, oflag, mode))  == -1)
    {
        zend_throw_exception (zend_ce_exception, strerror (errno), 0);
    }

    if(oflag >= O_CREAT)
    {

        if(ftruncate(shm_fd, (off_t)size) == -1)
        {
            zend_throw_exception (zend_ce_exception, strerror (errno), 0);
        }

    }

    mflag = set_map_flags(oflag);

    if((shm = mmap(NULL, (size_t)size, mflag, MAP_SHARED, shm_fd, (off_t)0)) == MAP_FAILED)
    {
        zend_throw_exception (zend_ce_exception, strerror (errno), 0);
    }

    if(close(shm_fd) == -1)
    {
        zend_throw_exception (zend_ce_exception, strerror (errno), 0);
    }

    return shm;

}

/* }}} */
