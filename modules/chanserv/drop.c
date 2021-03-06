/*
 * Copyright (c) 2005 William Pitcock, et al.
 * Rights to this code are as documented in doc/LICENSE.
 *
 * This file contains code for the CService DROP function.
 */

#include "atheme.h"

static void
cmd_cs_drop_func(sourceinfo_t *const restrict si, const int __attribute__((unused)) parc, char *parv[])
{
	const char *const name = parv[0];
	const char *const key = parv[1];

	if (! name)
	{
		(void) command_fail(si, fault_needmoreparams, STR_INSUFFICIENT_PARAMS, "DROP");
		(void) command_fail(si, fault_needmoreparams, _("Syntax: DROP <#channel>"));
		return;
	}

	if (!VALID_CHANNEL_PFX(name))
	{
		(void) command_fail(si, fault_badparams, STR_INVALID_PARAMS, "DROP");
		(void) command_fail(si, fault_badparams, _("Syntax: DROP <#channel>"));
		return;
	}

	mychan_t *mc;

	if (! (mc = mychan_find(name)))
	{
		(void) command_fail(si, fault_nosuch_target, _("Channel \2%s\2 is not registered."), name);
		return;
	}

	if (si->c)
	{
		(void) command_fail(si, fault_noprivs, _("For security reasons, you may not drop a channel "
		                                         "registration with a fantasy command."));
		return;
	}

	if (! is_founder(mc, entity(si->smu)))
	{
		(void) command_fail(si, fault_noprivs, _("You are not authorized to perform this operation."));
		return;
	}

	if (metadata_find(mc, "private:close:closer"))
	{
		(void) logcommand(si, CMDLOG_REGISTER, "DROP: \2%s\2 failed to drop (closed)", mc->name);
		(void) command_fail(si, fault_noprivs, _("The channel \2%s\2 is closed; it cannot be dropped."),
		                    mc->name);
		return;
	}

	if (mc->flags & MC_HOLD)
	{
		(void) command_fail(si, fault_noprivs, _("The channel \2%s\2 is held; it cannot be dropped."),
		                    mc->name);
		return;
	}

	if (si->su)
	{
		const char *const challenge = create_weak_challenge(si, mc->name);

		if (! challenge)
		{
			(void) command_fail(si, fault_internalerror, _("Failed to create challenge"));
			return;
		}

		if (! key)
		{
			char fullcmd[BUFSIZE];

			(void) snprintf(fullcmd, sizeof fullcmd, "/%s%s DROP %s %s", (ircd->uses_rcommand == false) ?
			                "msg " : "", chansvs.me->disp, mc->name, challenge);

			(void) command_success_nodata(si, _("To avoid accidental use of this command, this operation "
			                                    "has to be confirmed. Please confirm by replying with "
			                                    "\2%s\2"), fullcmd);
			return;
		}

		if (strcmp(challenge, key) != 0)
		{
			(void) command_fail(si, fault_badparams, _("Invalid key for %s."), "DROP");
			return;
		}
	}

	(void) logcommand(si, CMDLOG_REGISTER, "DROP: \2%s\2", mc->name);

	(void) hook_call_channel_drop(mc);

	if (mc->chan && ! (mc->chan->flags & CHAN_LOG))
		(void) part(mc->name, chansvs.nick);

	(void) command_success_nodata(si, _("The channel \2%s\2 has been dropped."), mc->name);
	(void) object_unref(mc);
}

static void
cmd_cs_fdrop_func(sourceinfo_t *const restrict si, const int __attribute__((unused)) parc, char *parv[])
{
	const char *const name = parv[0];

	if (! name)
	{
		(void) command_fail(si, fault_needmoreparams, STR_INSUFFICIENT_PARAMS, "FDROP");
		(void) command_fail(si, fault_needmoreparams, _("Syntax: FDROP <#channel>"));
		return;
	}

	if (!VALID_CHANNEL_PFX(name))
	{
		(void) command_fail(si, fault_badparams, STR_INVALID_PARAMS, "FDROP");
		(void) command_fail(si, fault_badparams, _("Syntax: FDROP <#channel>"));
		return;
	}

	mychan_t *mc;

	if (! (mc = mychan_find(name)))
	{
		(void) command_fail(si, fault_nosuch_target, _("Channel \2%s\2 is not registered."), name);
		return;
	}

	if (si->c)
	{
		(void) command_fail(si, fault_noprivs, _("For security reasons, you may not drop a channel "
		                                         "registration with a fantasy command."));
		return;
	}

	if (mc->flags & MC_HOLD)
	{
		(void) command_fail(si, fault_noprivs, _("The channel \2%s\2 is held; it cannot be dropped."),
		                    mc->name);
		return;
	}

	(void) logcommand(si, CMDLOG_ADMIN | LG_REGISTER, "FDROP: \2%s\2", mc->name);
	(void) wallops("%s dropped the channel \2%s\2", get_oper_name(si), mc->name);

	(void) hook_call_channel_drop(mc);

	if (mc->chan && ! (mc->chan->flags & CHAN_LOG))
		(void) part(mc->name, chansvs.nick);

	(void) command_success_nodata(si, _("The channel \2%s\2 has been dropped."), mc->name);
	(void) object_unref(mc);
}

static command_t cmd_cs_drop = {

	.name           = "DROP",
	.desc           = N_("Drops a channel registration."),
	.access         = AC_NONE,
	.maxparc        = 2,
	.cmd            = &cmd_cs_drop_func,

	.help           = {

		.path   = "cservice/drop",
		.func   = NULL,
	},
};

static command_t cmd_cs_fdrop = {

	.name           = "FDROP",
	.desc           = N_("Forces dropping of a channel registration."),
	.access         = PRIV_CHAN_ADMIN,
	.maxparc        = 1,
	.cmd            = &cmd_cs_fdrop_func,

	.help           = {

		.path   = "cservice/fdrop",
		.func   = NULL,
	},
};

static void
mod_init(module_t __attribute__((unused)) *const restrict m)
{
	(void) service_named_bind_command("chanserv", &cmd_cs_drop);
	(void) service_named_bind_command("chanserv", &cmd_cs_fdrop);
}

static void
mod_deinit(const module_unload_intent_t __attribute__((unused)) intent)
{
	(void) service_named_unbind_command("chanserv", &cmd_cs_drop);
	(void) service_named_unbind_command("chanserv", &cmd_cs_fdrop);
}

SIMPLE_DECLARE_MODULE_V1("chanserv/drop", MODULE_UNLOAD_CAPABILITY_OK)
