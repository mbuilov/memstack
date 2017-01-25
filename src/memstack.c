/*******************************************************************************
* Memory Stack allocation library
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/memstack
* Licensed under LGPL version 2.1 or any later version, see COPYING
*******************************************************************************/

/* memstack.c */

#include "memstack/memstack_base.inl"
#include "memstack/memstack.h"

MEMSTACK_NONNULL_ARGS
MEMSTACK_EXPORTS void _memstack_destroy_(
	MEMSTACK_PRE_VALID MEMSTACK_POST_INVALID struct memstack *st MEMSTACK_DEBUG_ARGS_DECL)
{
#ifdef MEMSTACK_DEBUG
	dmemstack_pop_blocks(&st->base.d_info, /*until_block:*/NULL, file, line);
#endif
	{
#ifdef MEMSTACK_DEBUG
		size_t used = st->last ? (size_t)((char*)st->base.bottom - _memstack_item_mem(st->last)) : 0;
#endif
		struct memstack_item *item = st->last;
		while (item) {
			struct memstack_item *prev = item->prev;
#ifdef MEMSTACK_DEBUG
			memstack_assert(memstack_is_aligned(used));
			if (item != st->last) {
				memstack_assert(used + item->base.msize >= used); /* no integer overflow */
				used += item->base.msize;
				memstack_assert(memstack_is_aligned(used));
			}
#endif
			_memstack_free(item);
			item = prev;
		}
#ifdef MEMSTACK_DEBUG
		memstack_assert(used == st->base.total_size);
#endif
	}
}

MEMSTACK_NONNULL_ARGS
MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_MAYBENULL MEMSTACK_RET_ALIGNED
MEMSTACK_PRE_SATISFIES(memstack_is_aligned(aligned_size))
MEMSTACK_POST_WRITABLE_BYTE_SIZE(MEMSTACK_ITEM_HDR_SIZE + aligned_size)
static inline MEMSTACK_RETURN_RESTRICT struct memstack_item *memstack_alloc_item(
	MEMSTACK_IN const struct memstack *st,
	MEMSTACK_NONZERO size_t aligned_size MEMSTACK_DEBUG_ARGS_DECL)
{
	memstack_assert(aligned_size && memstack_is_aligned(aligned_size));
	{
		struct memstack_item_base *b = memstack_base_alloc_item(
			&st->base, aligned_size, MEMSTACK_ITEM_HDR_SIZE MEMSTACK_DEBUG_ARGS_PASS);
		void *p = b;
		return (struct memstack_item*)p;
	}
}

MEMSTACK_NONNULL_ARGS
MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_MAYBENULL MEMSTACK_RET_ALIGNED
MEMSTACK_PRE_SATISFIES(memstack_is_aligned(aligned_size))
MEMSTACK_POST_WRITABLE_BYTE_SIZE(MEMSTACK_ITEM_HDR_SIZE + aligned_size)
static inline MEMSTACK_RETURN_RESTRICT struct memstack_item *memstack_realloc_item(
	MEMSTACK_IN const struct memstack *st,
	MEMSTACK_PRE_VALID MEMSTACK_WHEN(return != NULL, MEMSTACK_POST_PTR_INVALID) struct memstack_item *last,
	MEMSTACK_NONZERO size_t aligned_size MEMSTACK_DEBUG_ARGS_DECL)
{
	memstack_assert(aligned_size && memstack_is_aligned(aligned_size));
	{
		struct memstack_item_base *b = memstack_base_realloc_item(
			&st->base, &last->base, aligned_size, MEMSTACK_ITEM_HDR_SIZE MEMSTACK_DEBUG_ARGS_PASS);
		void *p = b;
		return (struct memstack_item*)p;
	}
}

MEMSTACK_NONNULL_ARGS
static inline void memstack_set_last_item(
	MEMSTACK_INOUT struct memstack *st,
	MEMSTACK_IN struct memstack_item *item)
{
	st->base.bottom = _memstack_item_bottom(item);
	st->base.limit = st->base.bottom; /* last item is full, new item will be allocated on next push() */
	st->last = item;
}

MEMSTACK_NONNULL_ARG_1
static inline void memstack_cleanup_or_set_last_item(
	MEMSTACK_INOUT struct memstack *st,
	MEMSTACK_IN_OPT struct memstack_item *item/*NULL?*/)
{
	if (item)
		memstack_set_last_item(st, item);
	else {
		/* cleanup */
		st->base.bottom = NULL;
		st->base.limit = NULL;
		st->last = NULL;
		memstack_assert(!st->base.total_size); /* all items were freed */
	}
}

MEMSTACK_NONNULL_ARGS
MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_BOOL
MEMSTACK_PRE_SATISFIES(memstack_is_aligned(aligned_size))
static int _memstack_push_item_(
	MEMSTACK_INOUT struct memstack *st,
	MEMSTACK_NONZERO size_t aligned_size MEMSTACK_DEBUG_ARGS_DECL)
{
	memstack_assert(aligned_size && memstack_is_aligned(aligned_size));
	if (st->last) {
		if (_memstack_is_last_free(st)) {
			memstack_assert(st->last->base.msize == _memstack_remaining(st));
			memstack_assert(st->last->base.msize == (size_t)((char*)st->base.limit - (char*)st->base.bottom));
		}
		else
			memstack_assert((char*)st->base.bottom > _memstack_item_mem(st->last));
	}
	{
		struct memstack_item *prev_item = st->last;
		int free_last = (prev_item && _memstack_is_last_free(st));
		if (free_last) {
			memstack_assert(st->last->base.msize < aligned_size); /* free last item should be too small */
			prev_item = st->last->prev; /* NULL? */
			_memstack_free(st->last); /* free last item because it's too small */
		}
		{
			struct memstack_item *item = memstack_alloc_item(st, aligned_size MEMSTACK_DEBUG_ARGS_PASS);
			if (_memstack_unlikely(!item)) {
				if (free_last) {
					/* _memstack_free(st->last) was called */
					memstack_cleanup_or_set_last_item(st, prev_item/*NULL?*/);
				}
				return 0; /* not enough system memory */
			}
			item->prev = prev_item;
			if (prev_item && !free_last) {
				/* _memstack_free(st->last) wasn't called, set used size of last item */
				prev_item->base.msize = (size_t)((char*)st->base.bottom - _memstack_item_mem(prev_item));
				memstack_assert(prev_item->base.msize && memstack_is_aligned(prev_item->base.msize));
			}
			st->base.bottom = _memstack_item_mem(item);
			st->base.limit = _memstack_item_bottom(item);
			st->last = item;
		}
	}
	return 1; /* next st->base.bottom will be incremented */
}

MEMSTACK_NONNULL_ARGS
MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_MAYBENULL MEMSTACK_RET_ALIGNED MEMSTACK_POST_WRITABLE_BYTE_SIZE(size)
MEMSTACK_EXPORTS MEMSTACK_RETURN_RESTRICT memstack_memory_t *_memstack_push__(
	MEMSTACK_INOUT struct memstack *st,
	MEMSTACK_NONZERO size_t size/*>0*/ MEMSTACK_DEBUG_ARGS_DECL)
{
	memstack_assert(size);
	{
#ifdef MEMSTACK_DEBUG
		size_t orig_size = size;
		if (!_memstack_sum(/*out:*/&size, DMEMSTACK_DEBUG_INFO_SIZE, size))
			return NULL; /* too big size: integer overflow */
#endif
		{
			size_t aligned_size;
			if (_memstack_unlikely(!memstack_align_and_check(/*out:*/&aligned_size, size)))
				return NULL; /* too big size: integer overflow */
			{
				size_t tmp;
				if (_memstack_unlikely(!_memstack_sum(/*out:*/&tmp, aligned_size, st->base.total_size)))
					return NULL; /* too big size: integer overflow */
			}
			if (aligned_size > (size_t)((char*)st->base.limit - (char*)st->base.bottom)) {
				/* not enough free space in last item */
				if (_memstack_unlikely(!_memstack_push_item_(st, aligned_size MEMSTACK_DEBUG_ARGS_PASS)))
					return NULL; /* not enough system memory */
			}
			memstack_inc_total_size(&st->base, aligned_size);
			{
				void *r = st->base.bottom;
				st->base.bottom = (char*)st->base.bottom + aligned_size;
#ifdef MEMSTACK_DEBUG
				r = dmemstack_block_added(&st->base.d_info, (struct dmemstack_block*)r, orig_size, file, line);
#endif
				memstack_assert((size_t)((char*)st->base.bottom - _memstack_item_mem(st->last)) <= st->base.total_size);
				return (memstack_memory_t*)r;
			}
		}
	}
}

MEMSTACK_NONNULL_ARGS
MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_BOOL
static int _memstack_pop_items_(
	MEMSTACK_INOUT struct memstack *st,
	MEMSTACK_NOTNULL MEMSTACK_POST_PTR_INVALID memstack_memory_t *mem)
{
#ifdef MEMSTACK_DEBUG
	void *p = DMEMSTACK_BLOCK_FROM_MEM(mem);
#else
	void *p = mem;
#endif
	struct memstack_item *item = st->last;
	size_t aligned_sz = (size_t)((char*)st->base.bottom - _memstack_item_mem(st->last));
	memstack_assert(!_memstack_is_last_free(st) ^ !aligned_sz);
	for (;;) {
		memstack_dec_total_size(&st->base, aligned_sz/*0?*/);
		{
			struct memstack_item *prev = item->prev;
			int done = (_memstack_item_mem(item) == p);
			_memstack_free(item);
			/* NOTE: current was item was deleted, 'item' pointer is invalid! */
			if (done) {
				memstack_assert(aligned_sz); /* must pop something */
				memstack_cleanup_or_set_last_item(st, prev/*NULL?*/);
				return 0; /* st->base.bottom shouldn't be changed by caller */
			}
			item = prev;
		}
		aligned_sz = item->base.msize;
		memstack_assert(aligned_sz && memstack_is_aligned(aligned_sz));
		{
			void *top = _memstack_item_mem(item);
			void *bottom = (char*)top + aligned_sz;
			memstack_assert(bottom != p); /* because cannot allocate zero bytes */
			if (top < p && p < bottom) {
				memstack_assert(memstack_is_aligned((size_t)((char*)p - (char*)top)));
				memstack_set_last_item(st, item);
				return 1; /* caller will set st->base.bottom */
			}
		}
	}
}

MEMSTACK_NONNULL_ARGS
MEMSTACK_EXPORTS void _memstack_pop_(
	MEMSTACK_INOUT struct memstack *st,
	MEMSTACK_NOTNULL MEMSTACK_POST_PTR_INVALID memstack_memory_t *mem MEMSTACK_DEBUG_ARGS_DECL)
{
#ifdef MEMSTACK_DEBUG
	void *p = DMEMSTACK_BLOCK_FROM_MEM(mem);
#else
	void *p = mem;
#endif
	/* top <= p < bottom */
#ifdef MEMSTACK_DEBUG
	dmemstack_pop_blocks(&st->base.d_info, (struct dmemstack_block*)p, file, line);
#endif
	memstack_assert(st->base.bottom != p);
	{
		void *top = _memstack_item_mem(st->last);
		memstack_assert(top <= st->base.bottom); /* top may be == st->base.bottom if last item is free */
		if (top < p && p < st->base.bottom)
			goto dec_size_set_bottom;
		if (top == p) {
			/* try to make free (preallocated, ready to use) last item */
			size_t free_space = (size_t)((char*)st->base.limit - (char*)st->base.bottom); /* 0? */
			if (_memstack_remaining(st) == free_space)
				goto dec_size_set_bottom; /* make free last item */
		}
	}
	if (_memstack_pop_items_(st, mem)) {
dec_size_set_bottom:
		memstack_dec_total_size(&st->base, (size_t)((char*)st->base.bottom - (char*)p));
		st->base.bottom = p;
	}
}

MEMSTACK_NONNULL_ARGS
MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_MAYBENULL MEMSTACK_RET_ALIGNED
MEMSTACK_PRE_SATISFIES(aligned_old_size != aligned_new_size)
MEMSTACK_PRE_SATISFIES(memstack_is_aligned(aligned_old_size))
MEMSTACK_PRE_SATISFIES(memstack_is_aligned(aligned_new_size))
MEMSTACK_POST_WRITABLE_BYTE_SIZE(aligned_new_size)
static MEMSTACK_RETURN_RESTRICT memstack_memory_t *_memstack_repush_item_(
	MEMSTACK_INOUT struct memstack *st,
	MEMSTACK_PRE_VALID MEMSTACK_WHEN(return != NULL, MEMSTACK_POST_PTR_INVALID)
		MEMSTACK_PRE_READABLE_BYTE_SIZE(aligned_old_size) memstack_memory_t *mem,
	MEMSTACK_NONZERO size_t aligned_old_size/*!=aligned_new_size*/,
	MEMSTACK_NONZERO size_t aligned_new_size MEMSTACK_DEBUG_ARGS_DECL)
{
	memstack_assert(aligned_old_size != aligned_new_size);
	memstack_assert(aligned_old_size && memstack_is_aligned(aligned_old_size));
	memstack_assert(aligned_new_size && memstack_is_aligned(aligned_new_size));
	{
		struct memstack_item *prev_item = st->last;
		int free_last = _memstack_is_last_free(st);
		if (free_last) {
			/* delete last free item  - we will realloc last used one */
			memstack_assert(st->last->base.msize == _memstack_remaining(st));
			memstack_assert(st->base.bottom == _memstack_item_mem(st->last));
			memstack_assert(st->base.limit == _memstack_item_bottom(st->last));
			prev_item = st->last->prev;
			_memstack_free(st->last);
		}
		{
#ifdef MEMSTACK_DEBUG
			void *p = DMEMSTACK_BLOCK_FROM_MEM(mem);
#else
			void *p = mem;
#endif
			if (aligned_new_size > aligned_old_size) {
				struct memstack_item *item;
				if (p == _memstack_item_mem(prev_item)) {
					/* realloc to needed size + space for future allocations */
					item = memstack_realloc_item(st, prev_item, aligned_new_size MEMSTACK_DEBUG_ARGS_PASS);
					if (_memstack_unlikely(!item)) {
						if (free_last) {
							/* _memstack_free(st->last) was called */
							memstack_set_last_item(st, prev_item);
						}
						return NULL; /* not enough system memory */
					}
				}
				else {
					/* adjust statistics to allocate what we need + more for future allocations */
					size_t diff = aligned_new_size - aligned_old_size; /* >0 */
					if (_memstack_sum(/*out:*/&st->base.max_total_size, diff, st->base.max_total_size)) {
						/* no integer overflow */
						memstack_assert(st->base.total_size >= aligned_old_size);
						memstack_assert(st->base.max_total_size - diff >= st->base.total_size);
						st->base.total_size -= aligned_old_size;
					}
					else {
						/* reset statistics - allocate exactly what we need */
						st->base.max_total_size = st->base.total_size;
						diff = 0; /* don't un-adjust statistics */
					}
					item = memstack_alloc_item(st, aligned_new_size MEMSTACK_DEBUG_ARGS_PASS);
					if (diff)
						st->base.total_size += aligned_old_size; /* restore */
					if (_memstack_unlikely(!item)) {
						st->base.max_total_size -= diff; /* un-adjust statistics */
						if (free_last) {
							/* _memstack_free(st->last) was called */
							memstack_set_last_item(st, prev_item);
						}
						return NULL; /* not enough system memory */
					}
					item->prev = prev_item;
					MEMSTACK_MEMCPY(_memstack_item_mem(item), p, aligned_old_size);
					prev_item->base.msize = (size_t)((char*)p - _memstack_item_mem(prev_item)); /* set used size */
					memstack_assert(prev_item->base.msize && memstack_is_aligned(prev_item->base.msize));
				}
				st->last = item;
				st->base.limit = _memstack_item_bottom(item);
				p = _memstack_item_mem(item);
			}
			else {
				/* reducing used memory in last allocated block - last free item must be deleted */
				memstack_assert(free_last); /* _memstack_free(st->last) was called */
				memstack_set_last_item(st, prev_item);
			}
			return (memstack_memory_t*)p; /* caller will set st->base.bottom */
		}
	}
}

MEMSTACK_NONNULL_ARG_1
MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_MAYBENULL MEMSTACK_RET_ALIGNED MEMSTACK_POST_WRITABLE_BYTE_SIZE(new_size)
MEMSTACK_EXPORTS MEMSTACK_RETURN_RESTRICT memstack_memory_t *_memstack_repush_last__(
	MEMSTACK_INOUT struct memstack *st,
	MEMSTACK_PRE_OPT_VALID MEMSTACK_WHEN(return != NULL, MEMSTACK_POST_PTR_INVALID) memstack_memory_t *mem/*NULL?*/,
	MEMSTACK_NONZERO size_t new_size/*>0*/ MEMSTACK_DEBUG_ARGS_DECL)
{
	memstack_assert(new_size);
	if (!mem)
		return _memstack_push_(st, new_size MEMSTACK_DEBUG_ARGS_PASS);
	{
#ifdef MEMSTACK_DEBUG
		void *p = DMEMSTACK_BLOCK_FROM_MEM(mem);
#else
		void *p = mem;
#endif
		/* top <= p < bottom */
		memstack_assert(st->base.bottom != p);
		/* p should be in the last non-free item because we repush last allocated item */
		if (_memstack_is_last_free(st)) {
			memstack_assert(st->last->base.msize == _memstack_remaining(st));
			memstack_assert(_memstack_item_mem(st->last->prev) <= (char*)p);
			memstack_assert((char*)p < _memstack_item_bottom(st->last->prev)); /* because cannot allocate zero bytes */
		}
		else
			memstack_assert(_memstack_item_mem(st->last) <= (char*)p && p < st->base.bottom);
		{
#ifdef MEMSTACK_DEBUG
			void *orig_mem = mem;
			size_t orig_new_size = new_size;
			if (!_memstack_sum(/*out:*/&new_size, DMEMSTACK_DEBUG_INFO_SIZE, new_size))
				return NULL; /* too big new_size: integer overflow */
#endif
			{
				size_t aligned_new_size;
				if (_memstack_unlikely(!memstack_align_and_check(/*out:*/&aligned_new_size, new_size)))
					return NULL; /* too big new_size: integer overflow */
				{
					void *bottom = _memstack_is_last_free(st) ? _memstack_item_bottom(st->last->prev) : st->base.bottom;
					size_t aligned_old_size = (size_t)((char*)bottom - (char*)p);
					memstack_assert(aligned_old_size && memstack_is_aligned(aligned_old_size)); /* p < bottom */
					if (aligned_new_size != aligned_old_size) {
						if (aligned_new_size > aligned_old_size) {
							size_t tmp;
							if (_memstack_unlikely(!_memstack_sum(/*out:*/&tmp, st->base.total_size, aligned_new_size - aligned_old_size)))
								return NULL; /* too big new_size: integer overflow */
						}
						{
							void *top = _memstack_item_mem(st->last);
							memstack_assert(top <= st->base.bottom); /* top may be == st->base.bottom if last item is free */
							memstack_assert(st->base.bottom <= st->base.limit);
							if (!(top <= p && p < st->base.bottom && (size_t)((char*)st->base.limit - (char*)p) >= aligned_new_size)) {
								p = _memstack_repush_item_(st, mem, aligned_old_size, aligned_new_size MEMSTACK_DEBUG_ARGS_PASS);
								if (_memstack_unlikely(!p))
									return NULL; /* not enough system memory */
							}
						}
						st->base.bottom = (char*)p + aligned_new_size;
						{
							size_t diff = _memstack_diff(aligned_new_size, aligned_old_size); /* NOTE: diff may be negative */
							memstack_inc_total_size(&st->base, diff);
						}
					}
				}
#ifdef MEMSTACK_DEBUG
				/* adjust p */
				p = dmemstack_last_block_reallocated(&st->base.d_info, (struct dmemstack_block*)p, orig_mem, orig_new_size, file, line);
#endif
			}
		}
		return (memstack_memory_t*)p;
	}
}

MEMSTACK_NONNULL_ARGS
MEMSTACK_EXPORTS void _memstack_cleanup_(MEMSTACK_INOUT struct memstack *st MEMSTACK_DEBUG_ARGS_DECL)
{
	memstack_assert(!st->base.total_size ^ !!st->last);
	memstack_assert(!st->last || !_memstack_is_last_free(st) || st->last->base.msize == _memstack_remaining(st));
	memstack_assert(memstack_is_aligned(st->base.total_size));
	memstack_assert(memstack_is_aligned(st->base.max_total_size));
	if (st->base.total_size) {
		memstack_assert(st->base.max_total_size);
		memstack_assert(st->last && memstack_is_aligned(st->last->base.msize));
		if (st->last->base.msize < st->base.max_total_size) {
			/* don't have one big item */
			_memstack_destroy_(st MEMSTACK_DEBUG_ARGS_PASS);
			st->base.bottom = NULL;
			st->base.limit = NULL;
			st->last = NULL;
		}
		else {
			/* make last free item */
			memstack_assert(st->last->base.msize == st->base.max_total_size);
			memstack_assert(st->base.limit == _memstack_item_bottom(st->last));
			memstack_assert(!st->last->prev); /* because st->last->base.msize == st->base.max_total_size */
			memstack_assert(!_memstack_is_last_free(st)); /* because st->total_size != 0 and last item is the only one */
			memstack_assert((char*)st->base.bottom > _memstack_item_mem(st->last)); /* because st->total_size != 0 */
#ifdef MEMSTACK_DEBUG
			dmemstack_pop_blocks(&st->base.d_info, (struct dmemstack_block*)_memstack_item_mem(st->last), file, line);
#endif
			st->base.bottom = _memstack_item_mem(st->last); /* make one big free item */
		}
		st->base.total_size = 0;
	}
	if (st->last) {
		memstack_assert(st->base.max_total_size);
		memstack_assert(_memstack_is_last_free(st));
		memstack_assert(st->last->base.msize == st->base.max_total_size);
		memstack_assert(st->base.limit == _memstack_item_bottom(st->last));
		memstack_assert(st->base.bottom == _memstack_item_mem(st->last));
	}
}

MEMSTACK_NONNULL_ARGS
static void _memstack_reset_items_(
	MEMSTACK_INOUT struct memstack *st,
	MEMSTACK_NOTNULL struct memstack_bottom *pos)
{
	struct memstack_item *item = st->last;
	size_t aligned_sz = (size_t)((char*)st->base.bottom - _memstack_item_mem(st->last)); /* 0? */
	for (;;) {
		memstack_assert(!_memstack_is_last_free(st) ^ !aligned_sz);
		memstack_dec_total_size(&st->base, aligned_sz);
		{
			struct memstack_item *prev = item->prev;
			_memstack_free(item);
			item = prev;
		}
		aligned_sz = item->base.msize;
		memstack_assert(aligned_sz && memstack_is_aligned(aligned_sz));
		{
			void *top = _memstack_item_mem(item);
			void *bottom = (char*)top + aligned_sz;
			memstack_assert(top != pos); /* top < pos <= bottom */
			if (top < (void*)pos && (void*)pos <= bottom) {
				memstack_set_last_item(st, item);
				return;
			}
		}
	}
}

MEMSTACK_NONNULL_ARG_1
MEMSTACK_EXPORTS void _memstack_reset_(
	MEMSTACK_INOUT struct memstack *st,
	MEMSTACK_MAYBENULL struct memstack_bottom *pos MEMSTACK_DEBUG_ARGS_DECL)
{
	if (pos == st->base.bottom)
		return;
	if (!pos) {
		_memstack_cleanup_(st MEMSTACK_DEBUG_ARGS_PASS);
		return;
	}
	/* top < pos <= bottom */
	memstack_assert(_memstack_item_mem(st->last) != (char*)pos);
#ifdef MEMSTACK_DEBUG
	dmemstack_reset_blocks(&st->base.d_info, pos, file, line);
#endif
	memstack_assert(_memstack_item_mem(st->last) <= (char*)st->base.bottom); /* top may be == st->base.bottom if last item is free */
	if (_memstack_item_mem(st->last) < (char*)pos && (void*)pos < st->base.bottom)
		goto dec_size_set_bottom;
	memstack_assert(st->last->prev); /* pos must be > _memstack_item_mem(?) */
	if (pos == (struct memstack_bottom*)_memstack_item_bottom(st->last->prev)) {
		memstack_assert(_memstack_item_mem(st->last->prev) != (char*)pos);
		if (_memstack_is_last_free(st)) {
			memstack_assert(st->last->base.msize == _memstack_remaining(st));
			return; /* no items were pushed after the pos */
		}
		{
			/* try to make free last item */
			size_t free_space = (size_t)((char*)st->base.limit - (char*)st->base.bottom); /* 0? */
			if (_memstack_remaining(st) == free_space) {
				/* make free item */
				pos = (struct memstack_bottom*)_memstack_item_mem(st->last);
				memstack_assert(pos != st->base.bottom); /* st->last is not free */
				goto dec_size_set_bottom;
			}
		}
	}
	_memstack_reset_items_(st, pos);
dec_size_set_bottom:
	memstack_dec_total_size(&st->base, (size_t)((char*)st->base.bottom - (char*)pos)); /* 0? after _memstack_reset_items_() */
	st->base.bottom = pos;
}
