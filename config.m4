dnl $Id$
dnl config.m4 for extension mcphessian

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(mcphessian, for mcphessian support,
Make sure that the comment is aligned:
[  --with-mcphessian             Include mcphessian support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(mcphessian, whether to enable mcphessian support,
dnl Make sure that the comment is aligned:
dnl [  --enable-mcphessian           Enable mcphessian support])

if test "$PHP_MCPHESSIAN" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-mcphessian -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/mcphessian.h"  # you most likely want to change this
  dnl if test -r $PHP_MCPHESSIAN/$SEARCH_FOR; then # path given as parameter
  dnl   MCPHESSIAN_DIR=$PHP_MCPHESSIAN
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for mcphessian files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       MCPHESSIAN_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$MCPHESSIAN_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the mcphessian distribution])
  dnl fi

  dnl # --with-mcphessian -> add include path
  dnl PHP_ADD_INCLUDE($MCPHESSIAN_DIR/include)

  dnl # --with-mcphessian -> check for lib and symbol presence
  dnl LIBNAME=mcphessian # you may want to change this
  dnl LIBSYMBOL=mcphessian # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $MCPHESSIAN_DIR/lib, MCPHESSIAN_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_MCPHESSIANLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong mcphessian lib version or lib not found])
  dnl ],[
  dnl   -L$MCPHESSIAN_DIR/lib -lm -ldl
  dnl ])
  dnl
  dnl PHP_SUBST(MCPHESSIAN_SHARED_LIBADD)

  PHP_NEW_EXTENSION(mcphessian, mcphessian.c, $ext_shared)
fi
