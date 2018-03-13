/*
 * Copyright (c) 2008 Jilles Tjoelker
 * Rights to this code are as documented in doc/LICENSE.
 *
 * Module to disable holdnick enforcers.
 */

#include "atheme.h"

int oldflag;

static void
mod_init(module_t *const restrict m)
{
	if (ircd == NULL)
	{
		slog(LG_ERROR, "Module %s must be loaded after a protocol module.", m->name);
		m->mflags = MODTYPE_FAIL;
		return;
	}
	oldflag = ircd->flags & IRCD_HOLDNICK;
	ircd->flags &= ~IRCD_HOLDNICK;
}

static void
mod_deinit(const module_unload_intent_t intent)
{
	ircd->flags |= oldflag;
}

SIMPLE_DECLARE_MODULE_V1("protocol/mixin_noholdnick", MODULE_UNLOAD_CAPABILITY_OK)
