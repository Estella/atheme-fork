/*
 * Copyright (c) 2005 Atheme Development Group
 * Rights to this code are documented in doc/LICENSE.
 *
 * This file contains the main() routine.
 */

#include "atheme.h"

service_t *gs;

static void
mod_init(module_t *const restrict m)
{
	gs = service_add("gameserv", NULL);
}

static void
mod_deinit(const module_unload_intent_t intent)
{
        if (gs != NULL)
                service_delete(gs);
}

SIMPLE_DECLARE_MODULE_V1("gameserv/main", MODULE_UNLOAD_CAPABILITY_OK)
