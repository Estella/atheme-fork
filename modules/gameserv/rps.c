/*
 * Copyright (c) 2005-2006 William Pitcock <nenolod@nenolod.net> et al
 * Rights to this code are documented in doc/LICENSE.
 *
 * Rock Paper Scissors
 */

#include "atheme.h"
#include "gameserv_common.h"

static void command_rps(sourceinfo_t *si, int parc, char *parv[]);

command_t cmd_rps = { "RPS", N_("Rock Paper Scissors."), AC_NONE, 2, command_rps, { .path = "gameserv/rps" } };

static void
mod_init(module_t *const restrict m)
{
	service_named_bind_command("gameserv", &cmd_rps);

	service_named_bind_command("chanserv", &cmd_rps);
}

static void
mod_deinit(const module_unload_intent_t intent)
{
	service_named_unbind_command("gameserv", &cmd_rps);

	service_named_unbind_command("chanserv", &cmd_rps);
}

static void command_rps(sourceinfo_t *si, int parc, char *parv[])
{
	mychan_t *mc;
	static const char *rps_responses[3] = {
		N_("Rock"),
		N_("Paper"),
		N_("Scissors")
	};

	if (!gs_do_parameters(si, &parc, &parv, &mc))
		return;

	gs_command_report(si, "%s", _(rps_responses[rand() % 3]));
}

SIMPLE_DECLARE_MODULE_V1("gameserv/rps", MODULE_UNLOAD_CAPABILITY_OK)
