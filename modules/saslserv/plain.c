/*
 * Copyright (C) 2006-2017 Atheme Development Group
 * Rights to this code are as documented in doc/LICENSE.
 *
 * PLAIN mechanism provider
 */

#include "atheme.h"

static const struct sasl_core_functions *sasl_core_functions = NULL;

static unsigned int
mech_step(struct sasl_session *const restrict p, const void *const restrict in, const size_t inlen,
          void __attribute__((unused)) **const restrict out, size_t __attribute__((unused)) *const restrict outlen)
{
	if (! (p && in && inlen))
		return ASASL_ERROR;

	/*
	 * Data format: authzid 0x00 authcid 0x00 password [0x00]
	 */
	if (inlen > (NICKLEN + 1 + NICKLEN + 1 + PASSLEN + 1))
		return ASASL_ERROR;

	const char *ptr = in;
	const char *const end = ptr + inlen;

	const char *const authzid = ptr;
	if (! *authzid)
		return ASASL_ERROR;
	if (strlen(authzid) > NICKLEN)
		return ASASL_ERROR;
	if ((ptr += strlen(authzid) + 1) >= end)
		return ASASL_ERROR;

	const char *const authcid = ptr;
	if (! *authcid)
		return ASASL_ERROR;
	if (strlen(authcid) > NICKLEN)
		return ASASL_ERROR;
	if ((ptr += strlen(authcid) + 1) >= end)
		return ASASL_ERROR;

	const char *const secret = ptr;
	if (! *secret)
		return ASASL_ERROR;
	if (strlen(secret) > PASSLEN)
		return ASASL_ERROR;

	if (! sasl_core_functions->authzid_can_login(p, authzid, NULL))
		return ASASL_ERROR;

	myuser_t *mu = NULL;
	if (! sasl_core_functions->authcid_can_login(p, authcid, &mu))
		return ASASL_ERROR;

	if (mu->flags & MU_NOPASSWORD)
		/* Don't bad_password() the user */
		return ASASL_ERROR;

	if (! verify_password(mu, secret))
		return ASASL_FAIL;

	return ASASL_DONE;
}

static struct sasl_mechanism mech = {

	.name           = "PLAIN",
	.mech_start     = NULL,
	.mech_step      = &mech_step,
	.mech_finish    = NULL,
};

static void
mod_init(module_t *const restrict m)
{
	MODULE_TRY_REQUEST_SYMBOL(m, sasl_core_functions, "saslserv/main", "sasl_core_functions");

	(void) sasl_core_functions->mech_register(&mech);
}

static void
mod_deinit(const module_unload_intent_t __attribute__((unused)) intent)
{
	(void) sasl_core_functions->mech_unregister(&mech);
}

SIMPLE_DECLARE_MODULE_V1("saslserv/plain", MODULE_UNLOAD_CAPABILITY_OK)
