/*******************************************************************************
* Memory Stack LGPLv2.1+ library (memstack) - stack allocation library
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/memstack
* Licensed under LGPLv2.1+, see COPYING
*******************************************************************************/

/* memstack_test.c */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "memstack.h"

struct data {
	char mem[7];
};

static void test(void)
{
	struct memstack stack;
	struct memstack *st = &stack;
	memstack_init(st);
	memstack_enable_log(st, 1);
	assert(!memstack_get_bottom(st));
	{
		struct data *data = (struct data*)memstack_push(st, sizeof(*data));
		assert(data);
		memstack_pop(st, (memstack_memory_t*)data);
	}
	{
		memstack_memory_t *mem = memstack_push(st, 10);
		assert(mem);
		{
			int k = 0;
			for (; k < 10; k++)
				((char*)mem)[k] = (char)(k + 1);
		}
		{
			memstack_memory_t *mem2 = memstack_push(st, 5);
			assert(mem2);
			{
				/* number of bytes (5) must be the same as passed to previous memstack_push() */
				memstack_memory_t *mem3 = memstack_get_last_mem(st, 5);
				assert(mem3);
				assert(mem2 == mem3);
				memstack_pop(st, mem3);
			}
		}
		{
			int k = 0;
			for (; k < 10; k++)
				assert(((char*)mem)[k] == (char)(k + 1));
		}
		mem = memstack_repush_last(st, mem, 11);
		assert(mem);
		((char*)mem)[10] = 11;
		memstack_check(st);
		{
			int k = 0;
			for (; k < 11; k++)
				assert(((char*)mem)[k] == (char)(k + 1));
		}
		mem = memstack_repush_last(st, mem, 9);
		assert(mem);
		{
			int k = 0;
			for (; k < 9; k++)
				assert(((char*)mem)[k] == (char)(k + 1));
		}
		{
			struct memstack_bottom *pos = memstack_get_bottom(st);
			{
				memstack_memory_t *mem2 = memstack_push(st, 5);
				assert(mem2);
				{
					memstack_memory_t *mem3 = memstack_push(st, 50);
					assert(mem3);
					memstack_print(st);
				}
			}
			memstack_reset(st, pos);
		}
		{
			int k = 0;
			for (; k < 9; k++)
				assert(((char*)mem)[k] == (char)(k + 1));
		}
	}
	memstack_cleanup(st);
	assert(!memstack_get_bottom(st));
	{
		memstack_memory_t *mem = memstack_push(st, 1);
		assert(mem);
	}
	memstack_cleanup(st);
	{
		memstack_memory_t *mem1 = memstack_push(st, 500);
		if (mem1) {
			memstack_memory_t *mem2 = memstack_push(st, 600);
			if (mem2)
				memstack_pop(st, mem2);
			memstack_pop(st, mem1);
		}
		memstack_cleanup(st);
	}
	{
		memstack_memory_t *mem1 = memstack_push(st, 5);
		memstack_memory_t *mem2 = memstack_push(st, 6);
		memstack_memory_t *mem3 = memstack_repush_last(st, mem2, 7000);
		(void)mem1, (void)mem3;
	}
	memstack_destroy(st);
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	printf("testing...\n");
	test();
	printf("test passed\n");
	return 0;
}
