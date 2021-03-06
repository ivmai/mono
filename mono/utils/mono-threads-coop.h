/*
 * mono-threads-coop.h: Cooperative suspend thread helpers
 *
 * Author:
 *	Rodrigo Kumpera (kumpera@gmail.com)
 *
 * (C) 2015 Xamarin
 */

#ifndef __MONO_THREADS_COOP_H__
#define __MONO_THREADS_COOP_H__

#include <config.h>
#include <glib.h>

/* JIT specific interface */
extern volatile size_t mono_polling_required;

/* Runtime consumable API */

static gboolean G_GNUC_UNUSED
mono_threads_is_coop_enabled (void)
{
#if defined(USE_COOP_GC)
	return TRUE;
#else
	static gboolean is_coop_enabled = -1;
	if (G_UNLIKELY (is_coop_enabled == -1))
		is_coop_enabled = g_getenv ("MONO_ENABLE_COOP") != NULL ? TRUE : FALSE;
	return is_coop_enabled;
#endif
}

/* Internal API */

void mono_threads_state_poll (void);
void mono_threads_state_poll_stack_data (void* stackdata);

void* mono_threads_prepare_blocking (void* stackdata);
void mono_threads_finish_blocking (void* cookie, void* stackdata);

void* mono_threads_reset_blocking_start (void* stackdata);
void mono_threads_reset_blocking_end (void* cookie, void* stackdata);

static inline void
mono_threads_safepoint (void)
{
	if (G_UNLIKELY (mono_polling_required))
		mono_threads_state_poll ();
}

#define MONO_PREPARE_BLOCKING	\
	do {	\
		void *__dummy;	\
		void *__blocking_cookie = mono_threads_prepare_blocking (&__dummy)

#define MONO_FINISH_BLOCKING \
		mono_threads_finish_blocking (__blocking_cookie, &__dummy);	\
	} while (0)

#define MONO_PREPARE_RESET_BLOCKING	\
	do {	\
		void *__dummy;	\
		void *__reset_cookie = mono_threads_reset_blocking_start (&__dummy)

#define MONO_FINISH_RESET_BLOCKING \
		mono_threads_reset_blocking_end (__reset_cookie, &__dummy);	\
	} while (0)

#endif
