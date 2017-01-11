#ifndef MEMSTACK_DEBUG_H_INCLUDED
#define MEMSTACK_DEBUG_H_INCLUDED

/*******************************************************************************
* Memory Stack LGPLv2.1+ library (memstack) - stack allocation library
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/memstack
* Licensed under LGPLv2.1+, see COPYING
*******************************************************************************/

/* memstack_debug.h */

#ifndef MEMSTACK_COMN_H_INCLUDED
#error do not #include this file directly, #include "memstack_comn.h" instead
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* protection masks */
#define DMEMSTACK_BEGIN_MEMORY_MASK         0x47
#define DMEMSTACK_UNINITIALIZED_MEMORY_MASK 0xFC
#define DMEMSTACK_END_MEMORY_MASK           0x36

/* number of masks before and after allocated memory block (red-zone size), value must be aligned */
#ifndef DMEMSTACK_TEST_BYTES_COUNT
#define DMEMSTACK_TEST_BYTES_COUNT memstack_align_size(sizeof(void*))
#endif

/* check that byte is of 8 bits */
typedef int _check_8_bits_in_byte[1-2*(255 != (unsigned char)~(unsigned char)0)];

/* check that DMEMSTACK_TEST_BYTES_COUNT value is not zero */
typedef int _check_DMEMSTACK_TEST_BYTES_COUNT_nonzero[1-2*!(DMEMSTACK_TEST_BYTES_COUNT)];

/* check that DMEMSTACK_TEST_BYTES_COUNT value is not too big */
typedef int _check_DMEMSTACK_TEST_BYTES_COUNT_not_too_big[1-2*((DMEMSTACK_TEST_BYTES_COUNT) > ((size_t)~(size_t)0 >> 2) + 1)];

/* check that DMEMSTACK_TEST_BYTES_COUNT value is aligned */
typedef int _check_DMEMSTACK_TEST_BYTES_COUNT_aligned[1-2*!memstack_is_aligned(DMEMSTACK_TEST_BYTES_COUNT)];

struct dmemstack_block {
	struct dmemstack_block *dprev;
	size_t number;
	size_t dsize;   /* value may be NOT aligned */
	unsigned repushed : 1;
	unsigned line : sizeof(unsigned)*8 - 1;
	const char *file;
	/* ...padding... */
	/* ...test bytes... */  /* DMEMSTACK_TEST_BYTES_COUNT */
	/* ...user mem... */
	/* ...test bytes... */  /* DMEMSTACK_TEST_BYTES_COUNT */
};

/* check red zone of memory block,
  prints an error message and
  returns NULL if red zone was overwritten */
MEMSTACK_NONNULL_ARG_1
MEMSTACK_RET_MAYBENULL
MEMSTACK_EXPORTS const struct dmemstack_block *dmemstack_check_block(MEMSTACK_IN const struct dmemstack_block *b);

/* check red zone of all registered memory blocks */
MEMSTACK_EXPORTS void dmemstack_check_blocks(MEMSTACK_IN_OPT const struct dmemstack_block *b/*NULL?*/);

/* print all registered memory blocks */
MEMSTACK_EXPORTS void dmemstack_print_blocks(MEMSTACK_IN_OPT const struct dmemstack_block *b/*NULL?*/);

/* struct dmemstack_block will be placed at head of every allocated memory block */
#define DMEMSTACK_BLOCK_HDR_SIZE memstack_align_size(sizeof(struct dmemstack_block))

/* get user memory pointer from struct dmemstack_block */
#define DMEMSTACK_BLOCK_MEM(b)       ((void*)((char*)(b)             + DMEMSTACK_BLOCK_HDR_SIZE + (DMEMSTACK_TEST_BYTES_COUNT)))
#define DMEMSTACK_BLOCK_MEM_CONST(b) ((const void*)((const char*)(b) + DMEMSTACK_BLOCK_HDR_SIZE + (DMEMSTACK_TEST_BYTES_COUNT)))

/* get block from user memory pointer */
#define DMEMSTACK_BLOCK_FROM_MEM(m) ((struct dmemstack_block*)((char*)(m) - (DMEMSTACK_TEST_BYTES_COUNT) - DMEMSTACK_BLOCK_HDR_SIZE))

/* aligned size of debug info added to each memory block */
#define DMEMSTACK_DEBUG_INFO_SIZE (DMEMSTACK_BLOCK_HDR_SIZE + (DMEMSTACK_TEST_BYTES_COUNT) + (DMEMSTACK_TEST_BYTES_COUNT))

struct memstack_debug_info {
	size_t mem_usage;                   /* current memory usage */
	size_t peak_mem_usage;              /* peak of total memory allocations */
	size_t block_number;                /* next allocated memory block number */
	int print_mem_log;                  /* set to non-zero to enable push/pop/repush operations log, 0 - to disable */
	struct dmemstack_block *last_block; /* last allocated block of memory in stack */
};

/* used to initialize struct memstack_debug_info */
#define MEMSTACK_DEBUG_INFO_STATIC_INITIALIZER {0, 0, 0, 0, NULL}

MEMSTACK_NONNULL_ARG_1
static inline void memstack_debug_info_init(MEMSTACK_PRE_NOTNULL MEMSTACK_POST_VALID struct memstack_debug_info *d_info)
{
	d_info->mem_usage      = 0;
	d_info->peak_mem_usage = 0;
	d_info->block_number   = 0;
	d_info->print_mem_log  = 0;
	d_info->last_block     = NULL;
}

/* register allocated memory block */
/* returns user memory inside registered struct dmemstack_block */
MEMSTACK_NONNULL_ARG_1_2
MEMSTACK_RET_NEVER_NULL MEMSTACK_RET_ALIGNED MEMSTACK_RET_RANGE(==,DMEMSTACK_BLOCK_MEM(b))
void *dmemstack_block_added(
	MEMSTACK_INOUT struct memstack_debug_info *d_info,
	MEMSTACK_INOUT struct dmemstack_block *b,
	MEMSTACK_NONZERO size_t size/*>0,unaliged*/,
	MEMSTACK_IN_Z const char *file,
	unsigned line);

/* register reallocated last memory block */
/* returns user memory inside registered struct dmemstack_block */
MEMSTACK_NONNULL_ARG_1_2
MEMSTACK_RET_NEVER_NULL MEMSTACK_RET_ALIGNED MEMSTACK_RET_RANGE(==,DMEMSTACK_BLOCK_MEM(b))
void *dmemstack_last_block_reallocated(
	MEMSTACK_INOUT struct memstack_debug_info *d_info,
	MEMSTACK_INOUT struct dmemstack_block *b,
	MEMSTACK_MAYBENULL void *old_mem/*NULL?*/,
	MEMSTACK_NONZERO size_t new_size/*>0,unaliged*/,
	MEMSTACK_IN_Z const char *file,
	unsigned line);

/* unregister freed memory blocks,
  stop after unregistering until_block,
  if until_block == NULL - unregister all blocks */
MEMSTACK_NONNULL_ARG_1
void dmemstack_pop_blocks(
	MEMSTACK_INOUT struct memstack_debug_info *d_info,
	MEMSTACK_IN_OPT struct dmemstack_block *until_block/*NULL?*/,
	MEMSTACK_IN_Z const char *file,
	unsigned line);

/* unregister freed memory blocks,
  stop after unregistering block which end equals block_end,
  if block_end == NULL - unregister all blocks */
MEMSTACK_NONNULL_ARG_1
void dmemstack_reset_blocks(
	MEMSTACK_INOUT struct memstack_debug_info *d_info,
	MEMSTACK_MAYBENULL void *block_end/*NULL?*/,
	MEMSTACK_IN_Z const char *file,
	unsigned line);

#ifdef __cplusplus
}
#endif

#endif /* MEMSTACK_DEBUG_H_INCLUDED */
