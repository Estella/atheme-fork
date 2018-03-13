/*
 * Copyright (C) 2010-2018 Atheme development group
 * Rights to this code are as documented in doc/LICENSE.
 */

#include "atheme.h"

static void
cmd_os_genhash_func(sourceinfo_t *const restrict si, const int parc, char **const restrict parv)
{
	if (parc < 1)
	{
		(void) command_fail(si, fault_needmoreparams, STR_INSUFFICIENT_PARAMS, "GENHASH");
		(void) command_fail(si, fault_needmoreparams, _("Syntax: GENHASH <password>"));
		return;
	}

	const char *const result = crypt_password(parv[0]);

	if (result)
		(void) command_success_string(si, result, "%s", result);
	else
		(void) command_fail(si, fault_internalerror, _("Password hash generation failure"));

	(void) logcommand(si, CMDLOG_GET, "GENHASH");
}

static command_t cmd_os_genhash = {

	.name           = "GENHASH",
	.desc           = N_("Generates a password hash for SOPER."),
	.access         = PRIV_ADMIN,
	.maxparc        = 1,
	.cmd            = &cmd_os_genhash_func,

	.help           = {

		.path   = "operserv/genhash",
		.func   = NULL,
	},
};

static void
mod_init(module_t __attribute__((unused)) *const restrict m)
{
	(void) service_named_bind_command("operserv", &cmd_os_genhash);
}

static void
mod_deinit(const module_unload_intent_t __attribute__((unused)) intent)
{
	(void) service_named_unbind_command("operserv", &cmd_os_genhash);
}

SIMPLE_DECLARE_MODULE_V1("operserv/genhash", MODULE_UNLOAD_CAPABILITY_OK)
