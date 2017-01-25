/*******************************************************************************
* Memory Stack allocation library
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/memstack
* Licensed under LGPL version 2.1 or any later version, see COPYING
*******************************************************************************/

/* memstack_debug.c */

#ifdef MEMSTACK_DEBUG

#include "memstack/memstack_config.h"
#include "memstack/memstack_comn.h" /* #includes "memstack/memstack_debug.h" */

static inline void dmemstack_set_mem_mask(
	MEMSTACK_OUT_WRITES_ALL(DMEMSTACK_TEST_BYTES_COUNT) char s[DMEMSTACK_TEST_BYTES_COUNT],
	char m)
{
	const char *const e = s + (DMEMSTACK_TEST_BYTES_COUNT);
	do {
		*s++ = m;
	} while (s < e);
}

MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_BOOL
static inline int dmemstack_check_mem_mask(
	MEMSTACK_IN_READS(DMEMSTACK_TEST_BYTES_COUNT) const char s[DMEMSTACK_TEST_BYTES_COUNT],
	char m)
{
	const char *const e = s + (DMEMSTACK_TEST_BYTES_COUNT);
	do {
		if (m != *s++)
			return 0;
	} while (s < e);
	return 1;
}

MEMSTACK_NONNULL_ARG_1
static inline void dmemstack_inc_mem_usage(
	MEMSTACK_INOUT struct memstack_debug_info *d_info,
	MEMSTACK_NONZERO size_t sz)
{
	memstack_assert(sz);
	/* may overflow if sz is negative, it's ok */
	(void)_memstack_sum(/*out:*/&d_info->mem_usage, sz, d_info->mem_usage);
	if (d_info->mem_usage > d_info->peak_mem_usage)
		d_info->peak_mem_usage = d_info->mem_usage;
}

MEMSTACK_NONNULL_ARG_1
static inline void dmemstack_dec_mem_usage(
	MEMSTACK_INOUT struct memstack_debug_info *d_info,
	MEMSTACK_NONZERO size_t sz)
{
	memstack_assert(sz && d_info->mem_usage >= sz);
	d_info->mem_usage -= sz;
}

MEMSTACK_NONNULL_ARG_1
static inline const char *dmemstack_get_alloc_op_name(MEMSTACK_IN const struct dmemstack_block *b)
{
	return b->repushed ? "repushed" : "pushed";
}

MEMSTACK_NONNULL_ARG_1_2
MEMSTACK_RET_NEVER_NULL MEMSTACK_RET_ALIGNED MEMSTACK_RET_RANGE(==,DMEMSTACK_BLOCK_MEM(b))
void *dmemstack_block_added(
	MEMSTACK_INOUT struct memstack_debug_info *d_info,
	MEMSTACK_INOUT struct dmemstack_block *b,
	MEMSTACK_NONZERO size_t size/*>0,unaligned*/,
	MEMSTACK_IN_Z const char *file,
	unsigned line)
{
	void *m = DMEMSTACK_BLOCK_MEM(b);
	memstack_assert(d_info->mem_usage + size >= size); /* must be no integer overflow */
	dmemstack_inc_mem_usage(d_info, size);
	b->number = d_info->block_number;
	b->repushed = 0;
	b->dsize = size;
	b->line = line & (~0u >> 1);
	b->file = file;
	b->dprev = d_info->last_block;
	d_info->last_block = b;
	dmemstack_set_mem_mask((char*)b + DMEMSTACK_BLOCK_HDR_SIZE, DMEMSTACK_BEGIN_MEMORY_MASK);
	dmemstack_set_mem_mask((char*)m + size, DMEMSTACK_END_MEMORY_MASK);
	memset(m, DMEMSTACK_UNINITIALIZED_MEMORY_MASK, size);
	if (d_info->print_mem_log) {
		MEMSTACK_LOG(MEMSTACK_LOG_ARG "[%lu(%p): memstack_mem += %lu = %lu pushed (at %s:%u)]\n",
			(unsigned long)(~0ul & d_info->block_number), m, (unsigned long)(~0ul & size),
			(unsigned long)(~0ul & d_info->mem_usage), file, line);
	}
	d_info->block_number++;
	return m;
}

MEMSTACK_NONNULL_ARG_1_2
MEMSTACK_RET_NEVER_NULL MEMSTACK_RET_ALIGNED MEMSTACK_RET_RANGE(==,DMEMSTACK_BLOCK_MEM(b))
void *dmemstack_last_block_reallocated(
	MEMSTACK_INOUT struct memstack_debug_info *d_info,
	MEMSTACK_INOUT struct dmemstack_block *b,
	MEMSTACK_MAYBENULL void *old_mem/*NULL?*/,
	MEMSTACK_NONZERO size_t new_size/*>0,unaliged*/,
	MEMSTACK_IN_Z const char *file,
	unsigned line)
{
	void *m = DMEMSTACK_BLOCK_MEM(b);
	size_t diff = _memstack_diff(new_size, b->dsize); /* may be negative */
	if (diff) {
		dmemstack_inc_mem_usage(d_info, diff);
		if (d_info->print_mem_log) {
			MEMSTACK_LOG(MEMSTACK_LOG_ARG "[%lu(%p): mem %c= %lu = %lu repushed to %p (at %s:%u) (block was %s at %s:%u)]\n",
				(unsigned long)(~0ul & b->number), old_mem, b->dsize > new_size ? '-' : '+',
				(unsigned long)(~0ul & (b->dsize > new_size ? b->dsize - new_size : new_size - b->dsize)),
				(unsigned long)(~0ul & d_info->mem_usage), m, file, line,
				dmemstack_get_alloc_op_name(b), b->file, (unsigned)b->line);
		}
		b->repushed = 1;
		b->line = line & (~0u >> 1);
		b->file = file;
		if (new_size > b->dsize)
			memset((char*)m + b->dsize, DMEMSTACK_UNINITIALIZED_MEMORY_MASK, new_size - b->dsize);
		dmemstack_set_mem_mask((char*)m + new_size, DMEMSTACK_END_MEMORY_MASK);
		b->dsize = new_size;
		d_info->last_block = b; /* NOTE: b->dprev is valid, because b - last allocated block - was reallocated */
	}
	return m;
}

MEMSTACK_NONNULL_ARG_1
static void dmemstack_mem_error(MEMSTACK_IN const struct dmemstack_block *b)
{
	MEMSTACK_ERROR(MEMSTACK_ERROR_ARG "!memstack memory corrupted: block %lu(%p) of size %lu was %s at (%s:%u)\n",
		(unsigned long)(~0ul & b->number), DMEMSTACK_BLOCK_MEM_CONST(b),
		(unsigned long)(~0ul & b->dsize), dmemstack_get_alloc_op_name(b), b->file, (unsigned)b->line);
}

MEMSTACK_NONNULL_ARG_1
MEMSTACK_RET_MAYBENULL
MEMSTACK_EXPORTS const struct dmemstack_block *dmemstack_check_block(MEMSTACK_IN const struct dmemstack_block *b)
{
	if (!dmemstack_check_mem_mask((const char*)b + DMEMSTACK_BLOCK_HDR_SIZE, DMEMSTACK_BEGIN_MEMORY_MASK) ||
		!dmemstack_check_mem_mask((const char*)DMEMSTACK_BLOCK_MEM_CONST(b) + b->dsize, DMEMSTACK_END_MEMORY_MASK))
	{
		dmemstack_mem_error(b);
		return NULL;
	}
	return b;
}

MEMSTACK_EXPORTS void dmemstack_check_blocks(MEMSTACK_IN_OPT const struct dmemstack_block *b/*NULL?*/)
{
	while (b) {
		if (!dmemstack_check_block(b))
			return;
		b = b->dprev;
	}
}

MEMSTACK_EXPORTS void dmemstack_print_blocks(MEMSTACK_IN_OPT const struct dmemstack_block *b/*NULL?*/)
{
	while (b) {
		if (!dmemstack_check_block(b))
			return;
		MEMSTACK_LOG(MEMSTACK_LOG_ARG "[%lu(%p): size=%lu, %s (at %s:%u)]\n",
			(unsigned long)(~0ul & b->number), DMEMSTACK_BLOCK_MEM_CONST(b),
			(unsigned long)(~0ul & b->dsize), dmemstack_get_alloc_op_name(b), b->file, (unsigned)b->line);
		b = b->dprev;
	}
}

MEMSTACK_NONNULL_ARG_1
void dmemstack_pop_blocks(
	MEMSTACK_INOUT struct memstack_debug_info *d_info,
	MEMSTACK_IN_OPT struct dmemstack_block *until_block/*NULL?*/,
	MEMSTACK_IN_Z const char *file,
	unsigned line)
{
	struct dmemstack_block *b;
	do {
		b = d_info->last_block;
		if (!b) {
			memstack_assert(!until_block);
			break;
		}
		if (!dmemstack_check_block(b))
			break; /* memory corrupted */
		dmemstack_dec_mem_usage(d_info, b->dsize);
		if (d_info->print_mem_log) {
			MEMSTACK_LOG(MEMSTACK_LOG_ARG "[%lu(%p): memstack_mem -= %lu = %lu popped (at %s:%u) (block was %s at %s:%u)]\n",
				(unsigned long)(~0ul & b->number), DMEMSTACK_BLOCK_MEM(b),
				(unsigned long)(~0ul & b->dsize), (unsigned long)(~0ul & d_info->mem_usage), file, line,
				dmemstack_get_alloc_op_name(b), b->file, (unsigned)b->line);
		}
		d_info->last_block = b->dprev;
	} while (b != until_block);
}

MEMSTACK_NONNULL_ARG_1
void dmemstack_reset_blocks(
	MEMSTACK_INOUT struct memstack_debug_info *d_info,
	MEMSTACK_MAYBENULL void *block_end/*NULL?*/,
	MEMSTACK_IN_Z const char *file,
	unsigned line)
{
	struct dmemstack_block *b;
	for (;;) {
		b = d_info->last_block;
		if (!b) {
			memstack_assert(!block_end);
			break;
		}
		if (!dmemstack_check_block(b))
			break; /* memory corrupted */
		if ((char*)b + memstack_align_size(DMEMSTACK_DEBUG_INFO_SIZE + b->dsize) == block_end)
			break;
		dmemstack_dec_mem_usage(d_info, b->dsize);
		if (d_info->print_mem_log) {
			MEMSTACK_LOG(MEMSTACK_LOG_ARG "[%lu(%p): memstack_mem -= %lu = %lu popped (at %s:%u) (block was %s at %s:%u)]\n",
				(unsigned long)(~0ul & b->number), DMEMSTACK_BLOCK_MEM(b),
				(unsigned long)(~0ul & b->dsize), (unsigned long)(~0ul & d_info->mem_usage), file, line,
				dmemstack_get_alloc_op_name(b), b->file, (unsigned)b->line);
		}
		d_info->last_block = b->dprev;
	}
}

#endif /* MEMSTACK_DEBUG */

typedef int make_iso_compilers_happy;
