AC_INIT([mandlebrot], [0.1.0], [zach@zabot.dev], [], [https://github.com/zabot/mandlebrot])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

AC_PROG_CC

AC_ARG_ENABLE([pthread], [AS_HELP_STRING([--disable-pthread], [])])
if test "x$enable_pthread" != "xno"; then
  AX_PTHREAD
fi

AC_ARG_ENABLE([opencl], [AS_HELP_STRING([--disable-opencl], [])])
if test "x$enable_opencl" != "xno"; then
  AC_CHECK_HEADERS([CL/cl.h])
  AC_CHECK_LIB([OpenCL], [clGetPlatformIDs])
fi
AM_CONDITIONAL([HAVE_OPENCL], [test "x$ac_cv_lib_OpenCL_clGetPlatformIDs" = "xyes"])

AC_CHECK_LIB([m], [floor])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

