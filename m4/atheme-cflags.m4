ATHEME_CC_TEST_CFLAGS_RESULT="no"

AC_DEFUN([ATHEME_CC_TEST_CFLAGS],
	[
		AC_MSG_CHECKING([for C compiler flag(s) $1 ])

		CFLAGS_SAVED="${CFLAGS}"
		CFLAGS="${CFLAGS} $1"

		AC_COMPILE_IFELSE(
			[
				AC_LANG_PROGRAM([[]], [[]])
			], [
				ATHEME_CC_TEST_CFLAGS_RESULT='yes'

				AC_MSG_RESULT([yes])
			], [
				ATHEME_CC_TEST_CFLAGS_RESULT='no'
				CFLAGS="${CFLAGS_SAVED}"

				AC_MSG_RESULT([no])
			], [
				ATHEME_CC_TEST_CFLAGS_RESULT='no'
				CFLAGS="${CFLAGS_SAVED}"

				AC_MSG_RESULT([skipped as we are cross-compiling])
			]
		)

		unset CFLAGS_SAVED
	]
)
