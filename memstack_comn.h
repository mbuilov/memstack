#ifndef MEMSTACK_COMN_H_INCLUDED
#define MEMSTACK_COMN_H_INCLUDED

/*******************************************************************************
* Memory Stack LGPLv2.1+ library (memstack) - stack allocation library
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/memstack
* Licensed under LGPLv2.1+, see COPYING
*******************************************************************************/

/* memstack_comn.h */

/* used to align size of allocated memory blocks, must be power of 2 */
#ifndef MEMSTACK_MEM_ALIGN_SIZE
#define MEMSTACK_MEM_ALIGN_SIZE (sizeof(void*))
#endif

/* check that MEMSTACK_MEM_ALIGN_SIZE is non-zero */
typedef int _check_MEMSTACK_MEM_ALIGN_SIZE_non_zero[1-2*!(MEMSTACK_MEM_ALIGN_SIZE)];

/* check that MEMSTACK_MEM_ALIGN_SIZE value is a power of 2 */
typedef int _check_MEMSTACK_MEM_ALIGN_SIZE_power_of_2[1-2*((MEMSTACK_MEM_ALIGN_SIZE) & ((MEMSTACK_MEM_ALIGN_SIZE) - 1))];

/* check that MEMSTACK_MEM_ALIGN_SIZE value is not too big */
typedef int _check_MEMSTACK_MEM_ALIGN_SIZE_no_too_big[1-2*((MEMSTACK_MEM_ALIGN_SIZE) > ((size_t)~(size_t)0 >> 2) + 1)];

/* compute aligned size */
/* note: check for integer overflow */
#define _memstack_align_sum(size) ((size) + ((MEMSTACK_MEM_ALIGN_SIZE) - 1))
#define _memstack_align_size(sum) ((sum) & ~((MEMSTACK_MEM_ALIGN_SIZE) - 1))
#define memstack_align_size(size) _memstack_align_size(_memstack_align_sum(size))
#define memstack_is_aligned(size) (!((size) & ((MEMSTACK_MEM_ALIGN_SIZE) - 1)))

/* declaration for exported functions, such as __declspec(dllexport)/__declspec(dllimport) or __attribute__((visibility("default"))) */
#ifndef MEMSTACK_EXPORTS
#define MEMSTACK_EXPORTS
#endif

/* used to remember source of memory allocation call */
#ifdef MEMSTACK_DEBUG
#define MEMSTACK_SUFFIX(a) a##debug
#define MEMSTACK_DEBUG_ARGS , __FILE__, __LINE__
#define MEMSTACK_DEBUG_ARGS_DECL , const char *file, unsigned line
#define MEMSTACK_DEBUG_ARGS_PASS , file, line
#else
#define MEMSTACK_SUFFIX(a) a
#define MEMSTACK_DEBUG_ARGS
#define MEMSTACK_DEBUG_ARGS_DECL
#define MEMSTACK_DEBUG_ARGS_PASS
#endif

#if defined _MSC_VER && _MSC_VER >= 1600 && !defined MEMSTACK_NO_SAL
#include <sal.h>
#define MEMSTACK_IN                           _In_
#define MEMSTACK_IN_Z                         _In_z_
#define MEMSTACK_IN_OPT                       _In_opt_
#define MEMSTACK_OUT                          _Out_
#define MEMSTACK_INOUT                        _Inout_
#define MEMSTACK_INOUT_OPT                    _Inout_opt_
#define MEMSTACK_WHEN(c,a)                    _When_(c,a)
#define MEMSTACK_RET_RANGE(a,b)               _Ret_range_(a,b)
#define MEMSTACK_RET_MAYBENULL                _Ret_maybenull_
#define MEMSTACK_NONZERO                      _In_range_(>,0)
#define MEMSTACK_RET_BOOL                     _Ret_range_(0,1)
#define MEMSTACK_NOTNULL                      _Notnull_
#define MEMSTACK_MAYBENULL                    _Maybenull_
#define MEMSTACK_PRE_NOTNULL                  _Pre_notnull_
#define MEMSTACK_PRE_VALID                    _Pre_valid_
#define MEMSTACK_PRE_OPT_VALID                _Pre_opt_valid_
#define MEMSTACK_POST_VALID                   _Post_valid_
#define MEMSTACK_POST_INVALID                 _Post_invalid_
#define MEMSTACK_POST_PTR_INVALID             _Post_ptr_invalid_
#define MEMSTACK_RET_MAYBENULL_Z              _Ret_maybenull_z_
#define MEMSTACK_POST_WRITABLE_BYTE_SIZE(s)   _Post_writable_byte_size_(s)
#define MEMSTACK_PRE_READABLE_BYTE_SIZE(s)    _Pre_readable_byte_size_(s)
#define MEMSTACK_PRE_SATISFIES(e)             _Pre_satisfies_(e)
#define MEMSTACK_POST_SATISFIES(e)            _Post_satisfies_(e)
#define MEMSTACK_OUT_WRITES_ALL(s)            _Out_writes_all_(s)
#define MEMSTACK_IN_READS(s)                  _In_reads_(s)
#define MEMSTACK_RETURN_RESTRICT              __declspec(restrict)
#define MEMSTACK_MUST_CHECK_RESULT            _Check_return_
#define MEMSTACK_RET_NEVER_NULL               _Ret_notnull_
#define MEMSTACK_RET_ALIGNED
#define MEMSTACK_NONNULL_ARGS
#define MEMSTACK_NONNULL_ARG_1
#define MEMSTACK_NONNULL_ARG_1_2
#else
#if defined(__GNUC__) && (__GNUC__ > 4 || (4 == __GNUC__ && __GNUC_MINOR__ > 8))
#define MEMSTACK_MUST_CHECK_RESULT            __attribute__ ((warn_unused_result))
#define MEMSTACK_RET_NEVER_NULL               __attribute__ ((returns_nonnull))
#define MEMSTACK_RET_ALIGNED                  __attribute__ ((assume_aligned( \
  (MEMSTACK_MEM_ALIGN_SIZE) < sizeof(void*) ? (MEMSTACK_MEM_ALIGN_SIZE) : sizeof(void*))))
#define MEMSTACK_NONNULL_ARGS                 __attribute__ ((nonnull))
#define MEMSTACK_NONNULL_ARG_1                __attribute__ ((nonnull(1,1)))
#define MEMSTACK_NONNULL_ARG_1_2              __attribute__ ((nonnull(1,2)))
#else
#define MEMSTACK_MUST_CHECK_RESULT
#define MEMSTACK_RET_NEVER_NULL
#define MEMSTACK_RET_ALIGNED
#define MEMSTACK_NONNULL_ARGS
#define MEMSTACK_NONNULL_ARG_1
#define MEMSTACK_NONNULL_ARG_1_2
#endif
#define MEMSTACK_IN
#define MEMSTACK_IN_Z
#define MEMSTACK_IN_OPT
#define MEMSTACK_OUT
#define MEMSTACK_INOUT
#define MEMSTACK_INOUT_OPT
#define MEMSTACK_WHEN(c,a)
#define MEMSTACK_RET_RANGE(a,b)
#define MEMSTACK_RET_MAYBENULL
#define MEMSTACK_NONZERO
#define MEMSTACK_RET_BOOL
#define MEMSTACK_NOTNULL
#define MEMSTACK_MAYBENULL
#define MEMSTACK_PRE_NOTNULL
#define MEMSTACK_PRE_VALID
#define MEMSTACK_PRE_OPT_VALID
#define MEMSTACK_POST_VALID
#define MEMSTACK_POST_INVALID
#define MEMSTACK_POST_PTR_INVALID
#define MEMSTACK_RET_MAYBENULL_Z
#define MEMSTACK_POST_WRITABLE_BYTE_SIZE(s)
#define MEMSTACK_PRE_READABLE_BYTE_SIZE(s)
#define MEMSTACK_PRE_SATISFIES(e)
#define MEMSTACK_POST_SATISFIES(e)
#define MEMSTACK_OUT_WRITES_ALL(s)
#define MEMSTACK_IN_READS(s)
#define MEMSTACK_RETURN_RESTRICT
#endif

#ifdef __clang_analyzer__
#define memstack_assert(a) ((void)(!(a) ? __builtin_unreachable(), 0 : 1))
#endif

#ifndef memstack_assert
#ifdef MEMSTACK_DEBUG

#ifdef ASSERT
#define memstack_assert(expr) ASSERT(expr)
#else
#include <assert.h>
#define memstack_assert(expr) assert(expr)
#endif

#else /* !MEMSTACK_DEBUG */

#ifdef _MSC_VER
#define memstack_assert(a) __assume(a)
#elif defined __clang__
#define memstack_assert(a) __builtin_assume(a)
#elif defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#define memstack_assert(a) ((void)(!(a) ? __builtin_unreachable(), 0 : 1))
#else
#define memstack_assert(a) ((void)0)
#endif

#endif /* !MEMSTACK_DEBUG */
#endif /* !memstack_assert */

#ifdef MEMSTACK_DEBUG
#include "memstack_debug.h"
#endif

#if defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))
#define _memstack_likely(x)   __builtin_expect(!!(x), 1)
#define _memstack_unlikely(x) __builtin_expect(!!(x), 0)
#else
#define _memstack_likely(x)   (x)
#define _memstack_unlikely(x) (x)
#endif

#if defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7))
#define _memstack_assume_aligned(r) __builtin_assume_aligned((r), MEMSTACK_MEM_ALIGN_SIZE)
#else
#define _memstack_assume_aligned(r) (r)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* compute sum, detect overflow, returns 0 if overflow, 1 - if ok */
MEMSTACK_NONNULL_ARG_1
MEMSTACK_RET_BOOL
MEMSTACK_POST_SATISFIES(*s == (size_t)(a + b))
MEMSTACK_WHEN(return != 0, MEMSTACK_POST_SATISFIES(*s >= (size_t)(a + b)))
MEMSTACK_WHEN(return != 0, MEMSTACK_POST_SATISFIES(*s >= a))
MEMSTACK_WHEN(return != 0, MEMSTACK_POST_SATISFIES(*s >= b))
MEMSTACK_WHEN(return == 0, MEMSTACK_POST_SATISFIES(*s < a))
MEMSTACK_WHEN(return == 0, MEMSTACK_POST_SATISFIES(*s < b))
static inline int _memstack_sum(MEMSTACK_OUT size_t *s/*out*/, size_t a, size_t b)
{
#ifdef MEMSTACK_DEBUG
#define SZML ((size_t)~(size_t)0 >> 1)
#define SZMH (SZML + 1)
	*s = ((a & SZML) + (b & SZML)) ^ (a & SZMH) ^ (b & SZMH);
#undef SZMH
#undef SZML
#else
	*s = a + b;
#endif
	return *s >= a;
}

/* compute diff, wrap if overflow */
MEMSTACK_MUST_CHECK_RESULT
MEMSTACK_RET_RANGE(==, (size_t)(a - b))
static inline size_t _memstack_diff(size_t a, size_t b)
{
#ifdef MEMSTACK_DEBUG
	if (a < b)
		return (size_t)~(size_t)0 - (b - a - 1u);
#endif
	return a - b;
}

/* align size, detect overflow, returns 0 if overflow, 1 - if ok */
MEMSTACK_NONNULL_ARG_1
MEMSTACK_MUST_CHECK_RESULT MEMSTACK_RET_BOOL
MEMSTACK_POST_SATISFIES(return == *aligned >= sz)
static inline int memstack_align_and_check(MEMSTACK_OUT size_t *aligned/*out*/, size_t sz)
{
#ifdef MEMSTACK_DEBUG
	int o = _memstack_sum(aligned, (MEMSTACK_MEM_ALIGN_SIZE) - 1, sz);
	*aligned = _memstack_align_size(*aligned);
	return o;
#else
	*aligned = memstack_align_size(sz);
	return *aligned >= sz;
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* MEMSTACK_COMN_H_INCLUDED */
