/*
 * Copyright (c) 2005-2006 Patrick Fish, et al.
 * Rights to this code are as documented in doc/LICENSE.
 *
 * This file contains code for OS UPTIME
 */

#include "atheme.h"

static void os_cmd_uptime(sourceinfo_t *si, int parc, char *parv[]);

command_t os_uptime = { "UPTIME", N_("Shows services uptime and the number of registered nicks and channels."), PRIV_SERVER_AUSPEX, 1, os_cmd_uptime, { .path = "oservice/uptime" } };

static void
mod_init(module_t *const restrict m)
{
        service_named_bind_command("operserv", &os_uptime);
}

static void
mod_deinit(const module_unload_intent_t intent)
{
	service_named_unbind_command("operserv", &os_uptime);
}

static void os_cmd_uptime(sourceinfo_t *si, int parc, char *parv[])
{
	logcommand(si, CMDLOG_GET, "UPTIME");

#ifdef REPRODUCIBLE_BUILDS
	command_success_nodata(si, "%s [%s]", PACKAGE_STRING, revision);
#else
        command_success_nodata(si, "%s [%s] Build Date: %s", PACKAGE_STRING, revision, __DATE__);
#endif
        command_success_nodata(si, _("Services have been up for %s"), timediff(CURRTIME - me.start));
	command_success_nodata(si, _("Current PID: %d"), getpid());
        command_success_nodata(si, _("Registered accounts: %d"), cnt.myuser);
	if (!nicksvs.no_nick_ownership)
        	command_success_nodata(si, _("Registered nicknames: %d"), cnt.mynick);
        command_success_nodata(si, _("Registered channels: %d"), cnt.mychan);
        command_success_nodata(si, _("Users currently online: %d"), cnt.user - me.me->users);
}

SIMPLE_DECLARE_MODULE_V1("operserv/uptime", MODULE_UNLOAD_CAPABILITY_OK)
