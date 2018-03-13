AC_DEFUN([ATHEME_FEATURETEST_LARGENET], [

	LARGE_NET="No"

	AC_ARG_ENABLE([large-net],
		[AS_HELP_STRING([--enable-large-net], [Enable large network support])],
		[], [enable_large_net="no"])

	case "${enable_large_net}" in
		yes)
			LARGE_NET="Yes"
			AC_DEFINE([LARGE_NETWORK], [1], [Enable large network support])
			;;
		no)
			LARGE_NET="No"
			;;
		*)
			AC_MSG_ERROR([invalid option for --enable-large-net])
			;;
	esac
])
