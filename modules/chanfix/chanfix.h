/* chanfix - channel fixing service
 * Copyright (c) 2010 Atheme Development Group
 */

#ifndef __CHANFIX_H__
#define __CHANFIX_H__

#define CHANFIX_OP_THRESHHOLD   3
#define CHANFIX_ACCOUNT_WEIGHT  1.5
#define CHANFIX_MIN_FIX_SCORE	12

#define	CHANFIX_INITIAL_STEP	0.70f
#define CHANFIX_FINAL_STEP	0.30f

#define CHANFIX_RETENTION_TIME	(86400 * 28)
#define CHANFIX_FIX_TIME	(60 * 60)
#define CHANFIX_GATHER_INTERVAL	300
#define CHANFIX_EXPIRE_INTERVAL 3600

/* This value has been chosen such that the maximum score is about 8064,
 * which is the number of CHANFIX_GATHER_INTERVALs in CHANFIX_RETENTION_TIME.
 * Higher scores would decay more than they can gain (12 per hour).
 */
#define CHANFIX_EXPIRE_DIVISOR	672

typedef struct chanfix_channel {
	object_t parent;

	char *name;

	mowgli_list_t oprecords;
	time_t ts;
	time_t lastupdate;

	channel_t *chan;

	time_t fix_started;
	bool fix_requested;
} chanfix_channel_t;

typedef struct chanfix_oprecord {
	mowgli_node_t node;

	chanfix_channel_t *chan;

	myentity_t *entity;

	char user[USERLEN + 1];
	char host[HOSTLEN + 1];

	time_t firstseen;
	time_t lastevent;
	unsigned int age;
} chanfix_oprecord_t;

typedef struct chanfix_persist {
	int version;

	mowgli_heap_t *chanfix_channel_heap;
	mowgli_heap_t *chanfix_oprecord_heap;

	mowgli_patricia_t *chanfix_channels;
} chanfix_persist_record_t;

extern service_t *chanfix;
extern mowgli_patricia_t *chanfix_channels;

extern void chanfix_gather_init(chanfix_persist_record_t *);
extern void chanfix_gather_deinit(module_unload_intent_t, chanfix_persist_record_t *);

extern void chanfix_oprecord_update(chanfix_channel_t *chan, user_t *u);
extern void chanfix_oprecord_delete(chanfix_oprecord_t *orec);
extern chanfix_oprecord_t *chanfix_oprecord_create(chanfix_channel_t *chan, user_t *u);
extern chanfix_oprecord_t *chanfix_oprecord_find(chanfix_channel_t *chan, user_t *u);
extern chanfix_channel_t *chanfix_channel_create(const char *name, channel_t *chan);
extern chanfix_channel_t *chanfix_channel_find(const char *name);
extern chanfix_channel_t *chanfix_channel_get(channel_t *chan);
extern void chanfix_gather(void *unused);
extern void chanfix_expire(void *unused);

extern bool chanfix_do_autofix;
extern void chanfix_autofix_ev(void *unused);
extern void chanfix_can_register(hook_channel_register_check_t *req);

extern command_t cmd_list;
extern command_t cmd_chanfix;
extern command_t cmd_scores;
extern command_t cmd_info;
extern command_t cmd_help;
extern command_t cmd_mark;
extern command_t cmd_nofix;

#endif
