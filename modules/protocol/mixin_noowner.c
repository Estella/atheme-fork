/*
 * Copyright (c) 2006 Jilles Tjoelker
 * Rights to this code are as documented in doc/LICENSE.
 *
 * Module to disable owner (+q) mode.
 * This will stop Atheme setting this mode by itself, but it can still
 * be used via OperServ MODE etc.
 */

#include "atheme.h"

bool oldflag;

static void
mod_init(module_t *const restrict m)
{
	if (ircd == NULL)
	{
		slog(LG_ERROR, "Module %s must be loaded after a protocol module.", m->name);
		m->mflags = MODTYPE_FAIL;
		return;
	}
	if (cnt.mychan > 0)
	{
		slog(LG_ERROR, "Module %s must be loaded from the configuration file, not via MODLOAD.", m->name);
		m->mflags = MODTYPE_FAIL;
		return;
	}
	oldflag = ircd->uses_owner;
	ircd->uses_owner = false;
	update_chanacs_flags();
}

static void
mod_deinit(const module_unload_intent_t intent)
{
	ircd->uses_owner = oldflag;
	update_chanacs_flags();
}

SIMPLE_DECLARE_MODULE_V1("protocol/mixin_noowner", MODULE_UNLOAD_CAPABILITY_NEVER)
