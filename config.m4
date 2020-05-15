dnl config.m4 for extension pshm

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use 'with':

dnl PHP_ARG_WITH([pshm],
dnl   [for pshm support],
dnl   [AS_HELP_STRING([--with-pshm],
dnl     [Include pshm support])])

dnl Otherwise use 'enable':

PHP_ARG_ENABLE([pshm],
  [whether to enable pshm support],
  [AS_HELP_STRING([--enable-pshm],
    [Enable pshm support])],
  [no])

dnl if test "$PHP_PSHM" != "no"; then
  AC_DEFINE(HAVE_PSHM, 1, [ Have pshm support ])

    LDFLAGS="$LDFLAGS -lrt -lpthread"

  PHP_NEW_EXTENSION(pshm, pshm.c, $ext_shared)
dnl fi
