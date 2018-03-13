/*
 * Copyright (c) 2005-2006 Atheme Development Group
 * Rights to this code are documented in doc/LICENSE.
 *
 * This file contains functionality which implements the OService REHASH command.
 */

#include "atheme.h"
#include "conf.h"

static void os_cmd_rehash(sourceinfo_t *si, int parc, char *parv[]);

command_t os_rehash = { "REHASH", N_("Reload the configuration data."), PRIV_ADMIN, 0, os_cmd_rehash, { .path = "oservice/rehash" } };

static void
mod_init(module_t *const restrict m)
{
        service_named_bind_command("operserv", &os_rehash);
}

static void
mod_deinit(const module_unload_intent_t intent)
{
	service_named_unbind_command("operserv", &os_rehash);
}

/* REHASH */
void os_cmd_rehash(sourceinfo_t *si, int parc, char *parv[])
{
	slog(LG_INFO, "UPDATE (due to REHASH): \2%s\2", get_oper_name(si));
	wallops("Updating database by request of \2%s\2.", get_oper_name(si));
	expire_check(NULL);
	if (db_save)
		db_save(NULL, DB_SAVE_BG_IMPORTANT);

	logcommand(si, CMDLOG_ADMIN, "REHASH");
	wallops("Rehashing \2%s\2 by request of \2%s\2.", config_file, get_oper_name(si));

	if (conf_rehash())
		command_success_nodata(si, _("REHASH completed."));
	else
		command_fail(si, fault_nosuch_target, _("REHASH of \2%s\2 failed. Please correct any errors in the file and try again."), config_file);
}

SIMPLE_DECLARE_MODULE_V1("operserv/rehash", MODULE_UNLOAD_CAPABILITY_OK)
