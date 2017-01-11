/*******************************************************************************
* Memory Stack LGPLv2.1+ library (memstack) - stack allocation library
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/memstack
* Licensed under LGPLv2.1+, see COPYING
*******************************************************************************/

/* memstack_base.inl */

#include "memstack_config.h"
#include "memstack_base.h"

MEMSTACK_MUST_CHECK_RESULT
MEMSTACK_RET_MAYBENULL
MEMSTACK_RETURN_RESTRICT
MEMSTACK_POST_WRITABLE_BYTE_SIZE(size)
static inline void *_memstack_malloc(MEMSTACK_NONZERO size_t size/*>0*/)
{
	return MEMSTACK_MALLOC(size);
}

MEMSTACK_MUST_CHECK_RESULT
MEMSTACK_RET_MAYBENULL
MEMSTACK_RETURN_RESTRICT
MEMSTACK_POST_WRITABLE_BYTE_SIZE(size)
static inline void *_memstack_realloc(
	MEMSTACK_MAYBENULL MEMSTACK_WHEN(return != NULL, MEMSTACK_POST_PTR_INVALID) void *mem/*NULL?*/,
	MEMSTACK_NONZERO size_t size/*>0*/)
{
	return MEMSTACK_REALLOC(mem, size);
}

static inline void _memstack_free(MEMSTACK_MAYBENULL MEMSTACK_POST_PTR_INVALID void *mem/*NULL?*/)
{
	MEMSTACK_FREE(mem);
}

MEMSTACK_NONNULL_ARGS
MEMSTACK_PRE_SATISFIES(memstack_is_aligned(aligned_mem_size))
MEMSTACK_PRE_SATISFIES(memstack_is_aligned(aligned_hdr_size))
MEMSTACK_PRE_SATISFIES(aligned_hdr_size >= sizeof(struct memstack_item_base))
MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_MAYBENULL MEMSTACK_RET_ALIGNED
MEMSTACK_POST_WRITABLE_BYTE_SIZE(aligned_hdr_size + aligned_mem_size)
static MEMSTACK_RETURN_RESTRICT struct memstack_item_base *memstack_base_alloc_item(
	MEMSTACK_IN const struct memstack_base *st,
	MEMSTACK_NONZERO size_t aligned_mem_size,
	MEMSTACK_NONZERO unsigned aligned_hdr_size MEMSTACK_DEBUG_ARGS_DECL)
{
	size_t sum;
	struct memstack_item_base *item = NULL;
	size_t mx_size = st->max_total_size - st->total_size; /* 0? */
	memstack_assert(memstack_is_aligned(mx_size));
	memstack_assert(aligned_mem_size && memstack_is_aligned(aligned_mem_size));
	memstack_assert(memstack_is_aligned(aligned_hdr_size) && aligned_hdr_size >= sizeof(struct memstack_item_base));
	/* to avoid later allocations, first try to allocate maximum memory */
	if (mx_size > aligned_mem_size && _memstack_likely(_memstack_sum(/*out:*/&sum, aligned_hdr_size, mx_size))) {
		item = (struct memstack_item_base*)_memstack_malloc(sum);
		if (_memstack_likely(item))
			item->msize = mx_size;
	}
	/* if cannot, next try to allocate only needed amount */
	if (!item) {
		if (_memstack_likely(_memstack_sum(/*out:*/&sum, aligned_hdr_size, aligned_mem_size))) {
			item = (struct memstack_item_base*)_memstack_malloc(sum);
			if (_memstack_likely(item))
				item->msize = aligned_mem_size;
		}
#ifdef MEMSTACK_DEBUG
		if (!item && st->d_info.print_mem_log)
			MEMSTACK_ERROR(MEMSTACK_ERROR_ARG "!!!failed to alloc %lu bytes (at %s:%u)]\n",
				(unsigned long)(~0lu & sum/*minimum*/), file, line);
#endif
	}
	return item;
}

MEMSTACK_NONNULL_ARGS
MEMSTACK_PRE_SATISFIES(memstack_is_aligned(aligned_mem_size))
MEMSTACK_PRE_SATISFIES(memstack_is_aligned(aligned_hdr_size))
MEMSTACK_PRE_SATISFIES(aligned_hdr_size >= sizeof(struct memstack_item_base))
MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_MAYBENULL MEMSTACK_RET_ALIGNED
MEMSTACK_POST_WRITABLE_BYTE_SIZE(aligned_hdr_size + aligned_mem_size)
static MEMSTACK_RETURN_RESTRICT struct memstack_item_base *memstack_base_realloc_item(
	MEMSTACK_IN const struct memstack_base *st,
	MEMSTACK_NOTNULL MEMSTACK_WHEN(return != NULL, MEMSTACK_POST_PTR_INVALID)
		MEMSTACK_PRE_READABLE_BYTE_SIZE(sizeof(size_t)/*aligned_hdr_size*/)
		struct memstack_item_base *last/*item size stored at head of item*/,
	MEMSTACK_NONZERO size_t aligned_mem_size,
	MEMSTACK_NONZERO unsigned aligned_hdr_size MEMSTACK_DEBUG_ARGS_DECL)
{
	struct memstack_item_base *item = NULL;
	memstack_assert(aligned_mem_size && memstack_is_aligned(aligned_mem_size));
	memstack_assert(memstack_is_aligned(aligned_hdr_size) && aligned_hdr_size >= sizeof(struct memstack_item_base));
	if (_memstack_likely(_memstack_sum(/*out:*/&aligned_mem_size, aligned_hdr_size, aligned_mem_size))) {
		/* to avoid later allocations, first try to allocate maximum memory */
		size_t mx_size = st->max_total_size - st->total_size; /* 0? */
		memstack_assert(aligned_mem_size > aligned_hdr_size);
		memstack_assert(memstack_is_aligned(mx_size));
		if (_memstack_likely(_memstack_sum(/*out:*/&mx_size, aligned_mem_size, mx_size))) {
			memstack_assert(mx_size >= aligned_mem_size);
			item = (struct memstack_item_base*)_memstack_realloc(last, mx_size);
			if (_memstack_likely(item))
				item->msize = mx_size - aligned_hdr_size;
		}
		/* if cannot, next try to allocate only needed amount */
		if (_memstack_unlikely(!item)) {
			item = (struct memstack_item_base*)_memstack_realloc(last, aligned_mem_size);
			if (_memstack_likely(item))
				item->msize = aligned_mem_size - aligned_hdr_size;
		}
	}
#ifdef MEMSTACK_DEBUG
	if (!item && st->d_info.print_mem_log)
		MEMSTACK_ERROR(MEMSTACK_ERROR_ARG "!!!failed to realloc from %lu to %lu bytes (at %s:%u)]\n",
			(unsigned long)(~0lu & (aligned_hdr_size + last->msize)),
			(unsigned long)(~0lu & aligned_mem_size), file, line);
#endif
	return item;
}
