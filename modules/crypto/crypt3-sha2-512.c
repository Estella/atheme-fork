/*
 * Copyright (C) 2018 Atheme Development Group
 * Rights to this code are as documented in doc/LICENSE.
 *
 * POSIX-style SHA2-512 crypt(3) wrapper.
 */

#include "crypt3.h"

#ifdef HAVE_CRYPT

static bool __attribute__((warn_unused_result))
atheme_crypt3_sha2_512_selftest(void)
{
	static const char password[] = CRYPT3_MODULE_TEST_PASSWORD;
	static const char parameters[] = CRYPT3_MODULE_TEST_VECTOR_SHA2_512;

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

static const char *
atheme_crypt3_sha2_512_crypt(const char *const restrict password,
                             const char __attribute__((unused)) *const restrict parameters)
{
	static const char saltchars[CRYPT3_SHA2_SALTCHARS_LENGTH] = CRYPT3_SHA2_SALTCHARS;

	unsigned char rawsalt[CRYPT3_SHA2_SALTLENGTH];
	char salt[sizeof rawsalt + 1];
	char parv[PASSLEN + 1];

	(void) arc4random_buf(rawsalt, sizeof rawsalt);
	(void) memset(salt, 0x00, sizeof salt);

	for (size_t i = 0; i < sizeof rawsalt; i++)
		salt[i] = saltchars[rawsalt[i] % sizeof saltchars];

	if (snprintf(parv, sizeof parv, CRYPT3_SAVESALT_FORMAT_SHA2_512, salt) > PASSLEN)
	{
		(void) slog(LG_ERROR, "%s: snprintf(3) output would have been too long (BUG)", __func__);
		return NULL;
	}

	(void) slog(LG_DEBUG, CRYPT3_MODULE_WARNING, __func__);

	return atheme_crypt3_wrapper(password, parv, __func__);
}

static bool
atheme_crypt3_sha2_512_verify(const char *const restrict password, const char *const restrict parameters,
                              unsigned int *const restrict flags)
{
	char hash[BUFSIZE];

	if (sscanf(parameters, CRYPT3_LOADHASH_FORMAT_SHA2_512, hash) != 1)
	{
		if (sscanf(parameters, CRYPT3_LOADHASH_FORMAT_SHA2_512_EXT, hash) != 1)
		{
			(void) slog(LG_DEBUG, "%s: sscanf(3) was unsuccessful", __func__);
			return false;
		}
	}
	if (strlen(hash) != CRYPT3_LOADHASH_LENGTH_SHA2_512)
	{
		(void) slog(LG_DEBUG, "%s: digest not %u characters long", __func__, CRYPT3_LOADHASH_LENGTH_SHA2_512);
		return false;
	}

	*flags |= PWVERIFY_FLAG_MYMODULE;

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

	.id        = "crypt3-sha2-512",
	.crypt     = &atheme_crypt3_sha2_512_crypt,
	.verify    = &atheme_crypt3_sha2_512_verify,
};

static void
mod_init(module_t *const restrict m)
{
	if (! atheme_crypt3_sha2_512_selftest())
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

SIMPLE_DECLARE_MODULE_V1("crypto/crypt3-sha2-512", MODULE_UNLOAD_CAPABILITY_OK)
