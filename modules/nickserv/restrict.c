/*
 * Copyright (c) 2005 William Pitcock
 * Rights to this code are as documented in doc/LICENSE.
 *
 * Restrictions for nicknames.
 */

#include "atheme.h"
#include "list_common.h"
#include "list.h"

static void ns_cmd_restrict(sourceinfo_t *si, int parc, char *parv[]);

command_t ns_restrict = { "RESTRICT", N_("Restrict a user from using certain commands."), PRIV_MARK, 3, ns_cmd_restrict, { .path = "nickserv/restrict" } };

static bool is_restricted(const mynick_t *mn, const void *arg) {
	myuser_t *mu = mn->owner;

	return !!metadata_find(mu, "private:restrict:setter");
}

static bool restricted_match(const mynick_t *mn, const void *arg) {
	const char *restrictedpattern = (const char*)arg;
	metadata_t *mdrestricted;

	myuser_t *mu = mn->owner;

	mdrestricted = metadata_find(mu, "private:restrict:reason");

	if (mdrestricted != NULL && !match(restrictedpattern, mdrestricted->value))
		return true;

	return false;
}

static void info_hook(hook_user_req_t *hdata)
{
	metadata_t *md;

	if (has_priv(hdata->si, PRIV_USER_AUSPEX) && (md = metadata_find(hdata->mu, "private:restrict:setter")))
	{
		const char *setter = md->value;
		const char *reason;
		time_t ts;
		struct tm tm;
		char strfbuf[BUFSIZE];

		md = metadata_find(hdata->mu, "private:restrict:reason");
		reason = md != NULL ? md->value : "unknown";

		md = metadata_find(hdata->mu, "private:restrict:timestamp");
		ts = md != NULL ? atoi(md->value) : 0;

		tm = *localtime(&ts);
		strftime(strfbuf, sizeof strfbuf, TIME_FORMAT, &tm);

		command_success_nodata(hdata->si, _("%s was \2RESTRICTED\2 by %s on %s (%s)"), entity(hdata->mu)->name, setter, strfbuf, reason);
	}
}

static void
mod_init(module_t *const restrict m)
{
	service_named_bind_command("nickserv", &ns_restrict);

	hook_add_event("user_info");
	hook_add_user_info(info_hook);

	use_nslist_main_symbols(m);

	static list_param_t restricted;
	restricted.opttype = OPT_BOOL;
	restricted.is_match = is_restricted;

	static list_param_t restrict_match;
	restrict_match.opttype = OPT_STRING;
	restrict_match.is_match = restricted_match;

	list_register("restricted", &restricted);
	list_register("restricted-reason", &restrict_match);
}

static void
mod_deinit(const module_unload_intent_t intent)
{
	service_named_unbind_command("nickserv", &ns_restrict);

	hook_del_user_info(info_hook);

	list_unregister("restricted");
	list_unregister("restricted-reason");
}

static void ns_cmd_restrict(sourceinfo_t *si, int parc, char *parv[])
{
	char *target = parv[0];
	char *action = parv[1];
	char *info = parv[2];
	myuser_t *mu;

	if (!target || !action)
	{
		command_fail(si, fault_needmoreparams, STR_INSUFFICIENT_PARAMS, "RESTRICT");
		command_fail(si, fault_needmoreparams, _("Usage: RESTRICT <target> <ON|OFF> [note]"));
		return;
	}

	if (!(mu = myuser_find_ext(target)))
	{
		command_fail(si, fault_nosuch_target, _("\2%s\2 is not registered."), target);
		return;
	}

	if (!strcasecmp(action, "ON"))
	{
		if (!info)
		{
			command_fail(si, fault_needmoreparams, STR_INSUFFICIENT_PARAMS, "RESTRICT");
			command_fail(si, fault_needmoreparams, _("Usage: RESTRICT <target> ON <note>"));
			return;
		}

		if (metadata_find(mu, "private:restrict:setter"))
		{
			command_fail(si, fault_badparams, _("\2%s\2 is already restricted."), entity(mu)->name);
			return;
		}

		metadata_add(mu, "private:restrict:setter", get_oper_name(si));
		metadata_add(mu, "private:restrict:reason", info);
		metadata_add(mu, "private:restrict:timestamp", number_to_string(time(NULL)));

		wallops("%s restricted the account \2%s\2.", get_oper_name(si), entity(mu)->name);
		logcommand(si, CMDLOG_ADMIN, "RESTRICT:ON: \2%s\2 (reason: \2%s\2)", entity(mu)->name, info);
		command_success_nodata(si, _("\2%s\2 is now restricted."), entity(mu)->name);
	}
	else if (!strcasecmp(action, "OFF"))
	{
		if (!metadata_find(mu, "private:restrict:setter"))
		{
			command_fail(si, fault_badparams, _("\2%s\2 is not restricted."), entity(mu)->name);
			return;
		}

		metadata_delete(mu, "private:restrict:setter");
		metadata_delete(mu, "private:restrict:reason");
		metadata_delete(mu, "private:restrict:timestamp");

		wallops("%s unrestricted the account \2%s\2.", get_oper_name(si), entity(mu)->name);
		logcommand(si, CMDLOG_ADMIN, "RESTRICT:OFF: \2%s\2", entity(mu)->name);
		command_success_nodata(si, _("\2%s\2 is now unrestricted."), entity(mu)->name);
	}
	else
	{
		command_fail(si, fault_needmoreparams, STR_INVALID_PARAMS, "RESTRICT");
		command_fail(si, fault_needmoreparams, _("Usage: RESTRICT <target> <ON|OFF> [note]"));
	}
}

SIMPLE_DECLARE_MODULE_V1("nickserv/restrict", MODULE_UNLOAD_CAPABILITY_OK)
