#ifndef MEMSTACK_BASE_H_INCLUDED
#define MEMSTACK_BASE_H_INCLUDED

/*******************************************************************************
* Memory Stack allocation library
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/memstack
* Licensed under LGPL version 2.1 or any later version, see COPYING
*******************************************************************************/

/* memstack_base.h */

#include "memstack_comn.h"

#ifdef __cplusplus
extern "C" {
#endif

/* last item:
  ------
  |hhhh| <--- item header
  |----|
  |xxxx| <--- item mem
  |xxxx|
  |....| <--- stack bottom
  |....|
  ------ <--- stack limit (item bottom)
*/

struct memstack_base {
	void *bottom;          /* the bottom of used memory in last stack item */
	void *limit;           /* limit of last item's memory */
	size_t total_size;     /* total size of memory used by the stack to hold pushed items, 0 if stack is empty */
	size_t max_total_size; /* maximum total size of memory used by the stack for pushed items */
#ifdef MEMSTACK_DEBUG
	struct memstack_debug_info d_info; /* debug info */
#endif
};

#ifdef MEMSTACK_DEBUG
#define MEMSTACK_DEBUG_INFO_STATIC_INITIALIZER_ARG , MEMSTACK_DEBUG_INFO_STATIC_INITIALIZER
#else
#define MEMSTACK_DEBUG_INFO_STATIC_INITIALIZER_ARG
#endif

#define MEMSTACK_BASE_STATIC_INITIALIZER {NULL, NULL, 0, 0 MEMSTACK_DEBUG_INFO_STATIC_INITIALIZER_ARG}

MEMSTACK_NONNULL_ARGS
static inline void memstack_base_init(
	MEMSTACK_PRE_NOTNULL MEMSTACK_POST_VALID struct memstack_base *st)
{
	st->bottom = NULL;
	st->limit = NULL;
	st->total_size = 0;
	st->max_total_size = 0;
#ifdef MEMSTACK_DEBUG
	memstack_debug_info_init(&st->d_info);
#endif
}

MEMSTACK_NONNULL_ARGS
MEMSTACK_PRE_SATISFIES(memstack_is_aligned(aligned_size))
static inline void memstack_inc_total_size(
	MEMSTACK_INOUT struct memstack_base *st,
	MEMSTACK_NONZERO size_t aligned_size)
{
	memstack_assert(aligned_size && memstack_is_aligned(aligned_size));
	/* may overflow if aligned_size is negative, it's ok */
	(void)_memstack_sum(/*out:*/&st->total_size, aligned_size, st->total_size);
	if (st->total_size > st->max_total_size)
		st->max_total_size = st->total_size;
	memstack_assert(memstack_is_aligned(st->total_size));
}

MEMSTACK_NONNULL_ARG_1
MEMSTACK_PRE_SATISFIES(memstack_is_aligned(aligned_size))
static inline void memstack_dec_total_size(
	MEMSTACK_INOUT struct memstack_base *st,
	size_t aligned_size/*0?*/)
{
	memstack_assert(memstack_is_aligned(aligned_size));
	memstack_assert(st->total_size >= aligned_size);
	st->total_size -= aligned_size;
	memstack_assert(memstack_is_aligned(st->total_size));
}

struct memstack_item_base {
	size_t msize;
	/* ...control structure... */
};

#ifdef MEMSTACK_DEBUG
/* enable/disable memory operations log */
static inline void memstack_base_enable_log(MEMSTACK_INOUT struct memstack_base *st, int enable) {st->d_info.print_mem_log = enable;}

/* check for access violations on memory blocks allocated from the stack */
static inline void memstack_base_check(MEMSTACK_IN struct memstack_base *st) {dmemstack_check_blocks(st->d_info.last_block);}

/* print all memory blocks allocated from the stack */
static inline void memstack_base_print(MEMSTACK_IN struct memstack_base *st) {dmemstack_print_blocks(st->d_info.last_block);}
#endif

#ifdef __cplusplus
}
#endif

#endif /* MEMSTACK_BASE_H_INCLUDED */
