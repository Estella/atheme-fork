AC_DEFUN([ATHEME_FEATURETEST_FHSPATHS], [

	DATADIR='${prefix}/etc'
	DOCDIR='${prefix}/doc'
	LOCALEDIR='${datadir}/locale'
	LOGDIR='${prefix}/var'
	MODDIR='${exec_prefix}'
	RUNDIR='${prefix}/var'
	SHAREDIR='${prefix}'

	AC_ARG_ENABLE([fhs-paths],
		[AS_HELP_STRING([--enable-fhs-paths], [Use more FHS-like pathnames (for packagers)])],
		[], [enable_fhs_paths="no"])

	case "${enable_fhs_paths}" in
		yes)
			DATADIR='${localstatedir}/lib/atheme'
			DOCDIR='${datadir}/doc/atheme'
			LOGDIR='${localstatedir}/log/atheme'
			MODDIR='${libdir}/atheme'
			RUNDIR='${localstatedir}/run/atheme'
			SHAREDIR='${datadir}/atheme'
			;;
		no)
			;;
		*)
			AC_MSG_ERROR([invalid option for --enable-fhs-paths])
			;;
	esac

	AC_SUBST([DATADIR])
	AC_SUBST([DOCDIR])
	AC_SUBST([LOCALEDIR])
	AC_SUBST([LOGDIR])
	AC_SUBST([MODDIR])
	AC_SUBST([RUNDIR])
	AC_SUBST([SHAREDIR])
])
