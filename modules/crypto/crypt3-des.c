/*
 * Copyright (C) 2018 Atheme Development Group
 * Rights to this code are as documented in doc/LICENSE.
 *
 * POSIX-style DES crypt(3) wrapper (verify-only).
 */

#include "crypt3.h"

#ifdef HAVE_CRYPT

static bool __attribute__((warn_unused_result))
atheme_crypt3_des_selftest(void)
{
	static const char password[] = CRYPT3_MODULE_TEST_PASSWORD;
	static const char parameters[] = CRYPT3_MODULE_TEST_VECTOR_DES;

	const char *const result = atheme_crypt3_wrapper(password, parameters, __func__);

	if (! result)
		return false;

	if (strcmp(result, parameters) != 0)
	{
		(void) slog(LG_ERROR, "%s: crypt(3) returned an incorrect result", __func__);
		(void) slog(LG_ERROR, "%s: expected '%s', got '%s'", __func__, parameters, result);
		return false;
	}

	return true;
}

static bool
atheme_crypt3_des_verify(const char *const restrict password, const char *const restrict parameters,
                         unsigned int __attribute__((unused)) *const restrict flags)
{
	char hash[BUFSIZE];

	if (strlen(parameters) != CRYPT3_LOADHASH_LENGTH_DES)
	{
		(void) slog(LG_DEBUG, "%s: params not %u characters long", __func__, CRYPT3_LOADHASH_LENGTH_DES);
		return false;
	}
	if (sscanf(parameters, CRYPT3_LOADHASH_FORMAT_DES, hash) != 1)
	{
		(void) slog(LG_DEBUG, "%s: sscanf(3) was unsuccessful", __func__);
		return false;
	}
	if (strcmp(parameters, hash) != 0)
	{
		(void) slog(LG_DEBUG, "%s: strcmp(3) mismatch", __func__);
		return false;
	}

	/* The above strlen(3)/sscanf(3)/strcmp(3) are only cautionary, to avoid wasting some time passing data
	 * to crypt(3) that it obviously did not generate.  However, the DES hash format is not at all unique
	 * or reliably identifiable, and so we cannot be sure that even though we got to this point, that the
	 * digest was indeed produced by DES crypt(3).  For this reason, we DON'T set PWVERIFY_FLAG_MYMODULE.
	 */

	(void) slog(LG_DEBUG, CRYPT3_MODULE_WARNING, __func__);

	const char *const result = atheme_crypt3_wrapper(password, parameters, __func__);

	if (! result)
		return false;

	if (strcmp(parameters, result) != 0)
	{
		(void) slog(LG_DEBUG, "%s: strcmp(3) mismatch, invalid password?", __func__);
		(void) slog(LG_DEBUG, "%s: expected '%s', got '%s'", __func__, parameters, result);
		return false;
	}

	return true;
}

static crypt_impl_t crypto_crypt3_impl = {

	.id        = "crypt3-des",
	.verify    = &atheme_crypt3_des_verify,
};

static void
mod_init(module_t *const restrict m)
{
	if (! atheme_crypt3_des_selftest())
	{
		(void) slog(LG_ERROR, "%s: self-test failed, does this platform support this algorithm?", m->name);

		m->mflags = MODTYPE_FAIL;
		return;
	}

	(void) slog(LG_INFO, CRYPT3_MODULE_WARNING, m->name);

	(void) crypt_register(&crypto_crypt3_impl);
}

static void
mod_deinit(const module_unload_intent_t __attribute__((unused)) intent)
{
	(void) crypt_unregister(&crypto_crypt3_impl);
}

#else /* HAVE_CRYPT */

static void
mod_init(module_t *const restrict m)
{
	(void) slog(LG_ERROR, "%s: crypt(3) is not available on this platform", m->name);

	m->mflags = MODTYPE_FAIL;
}

static void
mod_deinit(const module_unload_intent_t __attribute__((unused)) intent)
{
	/* Nothing To Do */
}

#endif /* !HAVE_CRYPT */

SIMPLE_DECLARE_MODULE_V1("crypto/crypt3-des", MODULE_UNLOAD_CAPABILITY_OK)
