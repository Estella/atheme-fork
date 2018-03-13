/*
 * Copyright (c) 2006 William Pitcock, et al.
 * Rights to this code are documented in doc/LICENSE.
 *
 * This file is a meta-module for compatibility with old
 * setups pre-SET split.
 */

#include "atheme.h"

static void
mod_init(module_t *const restrict m)
{
	/* MODULE_TRY_REQUEST_DEPENDENCY stuff so this acts like a meta-
	 * module like chanserv/set.c .
	 */
	MODULE_TRY_REQUEST_DEPENDENCY(m, "nickserv/set_core");
	MODULE_TRY_REQUEST_DEPENDENCY(m, "nickserv/set_email");
	MODULE_TRY_REQUEST_DEPENDENCY(m, "nickserv/set_emailmemos");
	MODULE_TRY_REQUEST_DEPENDENCY(m, "nickserv/set_hidelastlogin");
	MODULE_TRY_REQUEST_DEPENDENCY(m, "nickserv/set_hidemail");

#ifdef ENABLE_NLS
	MODULE_TRY_REQUEST_DEPENDENCY(m, "nickserv/set_language");
#endif

	MODULE_TRY_REQUEST_DEPENDENCY(m, "nickserv/set_neverop");
	MODULE_TRY_REQUEST_DEPENDENCY(m, "nickserv/set_nomemo");
	MODULE_TRY_REQUEST_DEPENDENCY(m, "nickserv/set_noop");
	MODULE_TRY_REQUEST_DEPENDENCY(m, "nickserv/set_password");
	MODULE_TRY_REQUEST_DEPENDENCY(m, "nickserv/set_property");
	MODULE_TRY_REQUEST_DEPENDENCY(m, "nickserv/set_quietchg");
}

static void
mod_deinit(const module_unload_intent_t intent)
{
}

SIMPLE_DECLARE_MODULE_V1("nickserv/set", MODULE_UNLOAD_CAPABILITY_OK)
