#ifndef MEMSTACK_H_INCLUDED
#define MEMSTACK_H_INCLUDED

/*******************************************************************************
* Memory Stack allocation library
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/memstack
* Licensed under LGPL version 2.1 or any later version, see COPYING
*******************************************************************************/

/* memstack.h */

#include "memstack/memstack_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/* memory allocated in stack */
typedef struct memstack_memory memstack_memory_t;

/* memstack bottom position */
typedef struct memstack_bottom memstack_bottom_t;

#define memstack_destroy(st)                          _memstack_destroy_((st MEMSTACK_DEBUG_ARGS))
#define memstack_push(st, sz/*>0*/)                   _memstack_push_((st, sz MEMSTACK_DEBUG_ARGS))
#define memstack_pop(st, mem)                         _memstack_pop_((st, mem MEMSTACK_DEBUG_ARGS))
#define memstack_repush_last(st, mem, new_size/*>0*/) _memstack_repush_last_((st, mem, new_size MEMSTACK_DEBUG_ARGS))
#define memstack_cleanup(st)                          _memstack_cleanup_((st MEMSTACK_DEBUG_ARGS))
#define memstack_reset(st, pos)                       _memstack_reset_((st, pos MEMSTACK_DEBUG_ARGS))

#define _memstack_destroy_(args)                      _memstack_destroy__ args
#define _memstack_push_(args)                         ((memstack_memory_t*)_memstack_assume_aligned(_memstack_push__ args))
#define _memstack_pop_(args)                          _memstack_pop__ args
#define _memstack_repush_last_(args)                  ((memstack_memory_t*)_memstack_assume_aligned(_memstack_repush_last__ args))
#define _memstack_cleanup_(args)                      _memstack_cleanup__ args
#define _memstack_reset_(args)                        _memstack_reset__ args

#define _memstack_destroy__                           MEMSTACK_SUFFIX(memstack_destroy_)
#define _memstack_push__                              MEMSTACK_SUFFIX(memstack_push_)
#define _memstack_pop__                               MEMSTACK_SUFFIX(memstack_pop_)
#define _memstack_repush_last__                       MEMSTACK_SUFFIX(memstack_repush_last_)
#define _memstack_cleanup__                           MEMSTACK_SUFFIX(memstack_cleanup_)
#define _memstack_reset__                             MEMSTACK_SUFFIX(memstack_reset_)

/* used internally: dynamically allocated memstack item */
struct memstack_item {
	struct memstack_item_base base; /* base.msize is the aligned size of stack item's memory */
	struct memstack_item *prev;     /* previously allocated stack item, NULL for the first item */
	/* ...padding... */
	/* ...memory... */
};

#define MEMSTACK_ITEM_HDR_SIZE      ((unsigned int)memstack_align_size(sizeof(struct memstack_item)))
#define _memstack_item_mem(item)    ((char*)(item) + MEMSTACK_ITEM_HDR_SIZE)
#define _memstack_item_bottom(item) (_memstack_item_mem(item) + (item)->base.msize)

/* last allocated stack item may be free (free last item has size == max_total_size - total_size),
  this item is needed to create new memory stack on top of given memory stack */
#define _memstack_is_last_free(st)  (_memstack_item_mem((st)->last) == (st)->base.bottom)
#define _memstack_remaining(st)     ((st)->base.max_total_size - (st)->base.total_size)

/* memory stack */
struct memstack {
	struct memstack_base base;
	struct memstack_item *last; /* last allocated stack item, may be NULL */
#ifdef __cplusplus

	inline void init();
	inline void destroy();

	MEMSTACK_RETURN_RESTRICT
	inline memstack_memory_t *push(MEMSTACK_NONZERO size_t size/*>0*/);

	MEMSTACK_NONNULL_ARGS
	inline void pop(MEMSTACK_NOTNULL MEMSTACK_POST_PTR_INVALID memstack_memory_t *mem);

	MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_MAYBENULL MEMSTACK_RET_ALIGNED MEMSTACK_POST_WRITABLE_BYTE_SIZE(new_size)
	MEMSTACK_RETURN_RESTRICT
	inline memstack_memory_t *repush_last(
		MEMSTACK_PRE_OPT_VALID MEMSTACK_WHEN(return != NULL, MEMSTACK_POST_PTR_INVALID) memstack_memory_t *mem/*NULL?*/,
		MEMSTACK_NONZERO size_t new_size/*>0*/);

	inline void cleanup();

	MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_MAYBENULL MEMSTACK_RET_ALIGNED
	inline memstack_bottom_t *get_bottom();

	inline void reset(MEMSTACK_MAYBENULL memstack_bottom_t *pos/*NULL?*/);

	MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_NEVER_NULL MEMSTACK_RET_ALIGNED
	inline memstack_memory_t *get_last_mem(MEMSTACK_NONZERO size_t size/*>0*/);

	inline void check();
	inline void print();
	inline void enable_log(bool enable);

#endif /* __cplusplus */
};

/* static initializer */
#define MEMSTACK_STATIC_INITIALIZER {MEMSTACK_BASE_STATIC_INITIALIZER, NULL}

/* initialize the stack */
MEMSTACK_NONNULL_ARGS
static inline void memstack_init(
	MEMSTACK_PRE_NOTNULL MEMSTACK_POST_VALID struct memstack *st)
{
	memstack_base_init(&st->base);
	st->last = NULL;
}

/* free memory allocated for the stack, stack structure may be re-used only after memstack_init() */
MEMSTACK_NONNULL_ARGS
MEMSTACK_EXPORTS void _memstack_destroy__(
	MEMSTACK_PRE_VALID MEMSTACK_POST_INVALID struct memstack *st MEMSTACK_DEBUG_ARGS_DECL);

/* memstack_push(): get memory block from the stack,
  returned pointer will be suitably aligned for any structure,
  returns NULL if no memory */
MEMSTACK_NONNULL_ARGS
MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_MAYBENULL MEMSTACK_RET_ALIGNED MEMSTACK_POST_WRITABLE_BYTE_SIZE(size)
MEMSTACK_EXPORTS MEMSTACK_RETURN_RESTRICT memstack_memory_t *_memstack_push__(
	MEMSTACK_INOUT struct memstack *st,
	MEMSTACK_NONZERO size_t size/*>0*/ MEMSTACK_DEBUG_ARGS_DECL);

/* memstack_pop(): give memory back to stack;
  it is possible to pop multiple sequential allocations by one call - just pop the first allocation of the sequence */
/* NOTE: passed mem pointer should be the same one returned by one of memstack_push() */
MEMSTACK_NONNULL_ARGS
MEMSTACK_EXPORTS void _memstack_pop__(
	MEMSTACK_INOUT struct memstack *st,
	MEMSTACK_NOTNULL MEMSTACK_POST_PTR_INVALID memstack_memory_t *mem MEMSTACK_DEBUG_ARGS_DECL);

/* memstack_repush_last(): realloc() analog - expand or reduce size of last allocated in stack memory block,
  old memory bytes are copied into new location (if realloc() was called)
  if mem == NULL then acts like memstack_push() */
/* returns NULL if expanding of memory failed */
/* NOTE: on success returned pointer likely differs from given one (mem) - like realloc() does */
/* NOTE: unlike realloc() this function works only with LAST allocated in stack memory block! */
/* NOTE: passed mem pointer should be the same one returned by the memstack_push() */
MEMSTACK_NONNULL_ARG_1
MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_MAYBENULL MEMSTACK_RET_ALIGNED MEMSTACK_POST_WRITABLE_BYTE_SIZE(new_size)
MEMSTACK_EXPORTS MEMSTACK_RETURN_RESTRICT memstack_memory_t *_memstack_repush_last__(
	MEMSTACK_INOUT struct memstack *st,
	MEMSTACK_PRE_OPT_VALID MEMSTACK_WHEN(return != NULL, MEMSTACK_POST_PTR_INVALID) memstack_memory_t *mem/*NULL?*/,
	MEMSTACK_NONZERO size_t new_size/*>0*/ MEMSTACK_DEBUG_ARGS_DECL);

/* memstack_cleanup(): pop all stack items
  (but remember total size of popped items, first push() will try to allocate single block of maximum size) */
MEMSTACK_NONNULL_ARGS
MEMSTACK_EXPORTS void _memstack_cleanup__(MEMSTACK_INOUT struct memstack *st MEMSTACK_DEBUG_ARGS_DECL);

/* get bottom position - this position may be passed to memstack_reset() */
MEMSTACK_NONNULL_ARGS
MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_MAYBENULL MEMSTACK_RET_ALIGNED
static inline memstack_bottom_t *memstack_get_bottom(MEMSTACK_IN const struct memstack *st)
{
	return (memstack_bottom_t*)(
		!st->last ? NULL :
		!_memstack_is_last_free(st) ? st->base.bottom :
		st->last->prev ? _memstack_item_bottom(st->last->prev) :
		NULL
	);
}

/* memstack_reset(): free stack items allocated in stack up to given bottom pos (obtained via memstack_get_bottom()) */
/* NOTE: if pos is NULL then is equivalent to memstack_cleanup() */
MEMSTACK_NONNULL_ARG_1
MEMSTACK_EXPORTS void _memstack_reset__(
	MEMSTACK_INOUT struct memstack *st,
	MEMSTACK_MAYBENULL memstack_bottom_t *pos/*NULL?*/ MEMSTACK_DEBUG_ARGS_DECL);

/* if it's known that last pushed memory block was of 'size' bytes,
  then it's possible to get pointer to that block from the stack */
MEMSTACK_NONNULL_ARGS
MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_NEVER_NULL MEMSTACK_RET_ALIGNED
static inline memstack_memory_t *memstack_get_last_mem(
	MEMSTACK_IN struct memstack *st,
	MEMSTACK_NONZERO size_t size/*>0*/)
{
	memstack_assert(size && st->last);
#ifdef MEMSTACK_DEBUG
	memstack_assert(DMEMSTACK_DEBUG_INFO_SIZE + size >= size);
	size += DMEMSTACK_DEBUG_INFO_SIZE;
#endif
	memstack_assert(memstack_align_size(size) >= size);
	size = memstack_align_size(size);
	{
		void *bottom = _memstack_is_last_free(st) ? _memstack_item_bottom(st->last->prev) : st->base.bottom;
		void *mem = (char*)bottom - size;
#ifdef MEMSTACK_DEBUG
		if (!dmemstack_check_block((struct dmemstack_block*)mem))
			memstack_assert(0); /* corrupted memory block */
		mem = DMEMSTACK_BLOCK_MEM(mem);
#endif
		return (memstack_memory_t*)mem;
	}
}

#ifdef __cplusplus
}
#endif

#ifdef MEMSTACK_DEBUG
/* check for access violations on memory blocks allocated from the stack */
#define memstack_check(st)              memstack_base_check(&(st)->base)
/* print all memory blocks allocated from the stack */
#define memstack_print(st)              memstack_base_print(&(st)->base)
/* enable/disable memory operations log */
#define memstack_enable_log(st, enable) memstack_base_enable_log(&(st)->base, enable)
#else
#define memstack_check(st)              ((void)(st))
#define memstack_print(st)              ((void)(st))
#define memstack_enable_log(st, enable) ((void)(st),(void)(enable))
#endif

#ifdef __cplusplus

void memstack::init()
{
	memstack_init(this);
}

void memstack::destroy()
{
	memstack_destroy(this);
}

MEMSTACK_RETURN_RESTRICT
memstack_memory_t *memstack::push(MEMSTACK_NONZERO size_t size/*>0*/)
{
	return memstack_push(this, size);
}

MEMSTACK_NONNULL_ARGS
void memstack::pop(MEMSTACK_NOTNULL MEMSTACK_POST_PTR_INVALID memstack_memory_t *mem)
{
	memstack_pop(this, mem);
}

MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_MAYBENULL MEMSTACK_RET_ALIGNED MEMSTACK_POST_WRITABLE_BYTE_SIZE(new_size)
MEMSTACK_RETURN_RESTRICT
memstack_memory_t *memstack::repush_last(
	MEMSTACK_PRE_OPT_VALID MEMSTACK_WHEN(return != NULL, MEMSTACK_POST_PTR_INVALID) memstack_memory_t *mem/*NULL?*/,
	MEMSTACK_NONZERO size_t new_size/*>0*/)
{
	return memstack_repush_last(this, mem, new_size);
}

void memstack::cleanup()
{
	memstack_cleanup(this);
}

MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_MAYBENULL MEMSTACK_RET_ALIGNED
memstack_bottom_t *memstack::get_bottom()
{
	return memstack_get_bottom(this);
}

void memstack::reset(MEMSTACK_MAYBENULL memstack_bottom_t *pos/*NULL?*/)
{
	memstack_reset(this, pos);
}

MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_NEVER_NULL MEMSTACK_RET_ALIGNED
memstack_memory_t *memstack::get_last_mem(MEMSTACK_NONZERO size_t size/*>0*/)
{
	return memstack_get_last_mem(this, size);
}

void memstack::check()
{
	return memstack_check(this);
}

void memstack::print()
{
	return memstack_print(this);
}

void memstack::enable_log(bool enable)
{
	return memstack_enable_log(this, enable ? 1 : 0);
}

#endif /* __cplusplus */

#endif /* MEMSTACK_H_INCLUDED */
