/*
 * Copyright (c) 2005 William Pitcock <nenolod -at- nenolod.net>
 * Copyright (c) 2007 Jilles Tjoelker
 * Rights to this code are as documented in doc/LICENSE.
 *
 * Prevents you from being added to group access lists.
 */

#include "atheme.h"
#include "uplink.h"
#include "list_common.h"
#include "list.h"

mowgli_patricia_t **ns_set_cmdtree;

static void ns_cmd_set_nevergroup(sourceinfo_t *si, int parc, char *parv[]);

command_t ns_set_nevergroup = { "NEVERGROUP", N_("Prevents you from being added to group access lists."), AC_NONE, 1, ns_cmd_set_nevergroup, { .path = "nickserv/set_nevergroup" } };

static bool has_nevergroup(const mynick_t *mn, const void *arg)
{
	myuser_t *mu = mn->owner;

	return ( mu->flags & MU_NEVERGROUP ) == MU_NEVERGROUP;
}

static void
mod_init(module_t *const restrict m)
{
	MODULE_TRY_REQUEST_SYMBOL(m, ns_set_cmdtree, "nickserv/set_core", "ns_set_cmdtree");

	command_add(&ns_set_nevergroup, *ns_set_cmdtree);

	use_nslist_main_symbols(m);

	static list_param_t nevergroup;
	nevergroup.opttype = OPT_BOOL;
	nevergroup.is_match = has_nevergroup;

	list_register("nevergroup", &nevergroup);
}

static void
mod_deinit(const module_unload_intent_t intent)
{
	command_delete(&ns_set_nevergroup, *ns_set_cmdtree);

	list_unregister("nevergroup");
}

/* SET NEVERGROUP <ON|OFF> */
static void ns_cmd_set_nevergroup(sourceinfo_t *si, int parc, char *parv[])
{
	char *params = parv[0];

	if (!params)
	{
		command_fail(si, fault_needmoreparams, STR_INSUFFICIENT_PARAMS, "NEVERGROUP");
		return;
	}

	if (!strcasecmp("ON", params))
	{
		if (MU_NEVERGROUP & si->smu->flags)
		{
			command_fail(si, fault_nochange, _("The \2%s\2 flag is already set for account \2%s\2."), "NEVERGROUP", entity(si->smu)->name);
			return;
		}

		logcommand(si, CMDLOG_SET, "SET:NEVERGROUP:ON");

		si->smu->flags |= MU_NEVERGROUP;

		command_success_nodata(si, _("The \2%s\2 flag has been set for account \2%s\2."), "NEVERGROUP", entity(si->smu)->name);

		return;
	}

	else if (!strcasecmp("OFF", params))
	{
		if (!(MU_NEVERGROUP & si->smu->flags))
		{
			command_fail(si, fault_nochange, _("The \2%s\2 flag is not set for account \2%s\2."), "NEVERGROUP", entity(si->smu)->name);
			return;
		}

		logcommand(si, CMDLOG_SET, "SET:NEVERGROUP:OFF");

		si->smu->flags &= ~MU_NEVERGROUP;

		command_success_nodata(si, _("The \2%s\2 flag has been removed for account \2%s\2."), "NEVERGROUP", entity(si->smu)->name);

		return;
	}

	else
	{
		command_fail(si, fault_badparams, STR_INVALID_PARAMS, "NEVERGROUP");
		return;
	}
}

SIMPLE_DECLARE_MODULE_V1("nickserv/set_nevergroup", MODULE_UNLOAD_CAPABILITY_OK)
