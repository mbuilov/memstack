/*******************************************************************************
* Memory Stack allocation library
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/memstack
* Licensed under LGPL version 2.1 or any later version, see COPYING
*******************************************************************************/

/* memstack_test.cpp */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "memstack/memstack.h"

#define ASSERT(c) do { \
	int __c = !!(c); \
	assert(__c); \
	if (!__c) \
		return -1; \
} while ((void)0,0)

struct data {
	char mem[7];
};

static int test(void)
{
	struct memstack stack;
	struct memstack *st = &stack;
	st->init();
	st->enable_log(1);
	ASSERT(!st->get_bottom());
	{
		struct data *data = (struct data*)st->push(sizeof(*data));
		ASSERT(data);
		st->pop((memstack_memory_t*)data);
	}
	{
		memstack_memory_t *mem = st->push(10);
		ASSERT(mem);
		{
			int k = 0;
			for (; k < 10; k++)
				((char*)mem)[k] = (char)(k + 1);
		}
		{
			memstack_memory_t *mem2 = st->push(5);
			ASSERT(mem2);
			{
				/* number of bytes (5) must be the same as passed to previous st->push() */
				memstack_memory_t *mem3 = st->get_last_mem(5);
				ASSERT(mem3);
				ASSERT(mem2 == mem3);
				st->pop(mem3);
			}
		}
		{
			int k = 0;
			for (; k < 10; k++)
				ASSERT(((char*)mem)[k] == (char)(k + 1));
		}
		mem = st->repush_last(mem, 11);
		ASSERT(mem);
		((char*)mem)[10] = 11;
		st->check();
		{
			int k = 0;
			for (; k < 11; k++)
				ASSERT(((char*)mem)[k] == (char)(k + 1));
		}
		mem = st->repush_last(mem, 9);
		ASSERT(mem);
		{
			int k = 0;
			for (; k < 9; k++)
				ASSERT(((char*)mem)[k] == (char)(k + 1));
		}
		{
			memstack_bottom_t *pos = st->get_bottom();
			{
				memstack_memory_t *mem2 = st->push(5);
				ASSERT(mem2);
				{
					memstack_memory_t *mem3 = st->push(50);
					ASSERT(mem3);
					st->print();
				}
			}
			st->reset(pos);
		}
		{
			int k = 0;
			for (; k < 9; k++)
				ASSERT(((char*)mem)[k] == (char)(k + 1));
		}
	}
	st->cleanup();
	ASSERT(!st->get_bottom());
	{
		memstack_memory_t *mem = st->push(1);
		ASSERT(mem);
	}
	st->cleanup();
	{
		memstack_memory_t *mem1 = st->push(500);
		if (mem1) {
			memstack_memory_t *mem2 = st->push(600);
			if (mem2)
				st->pop(mem2);
			st->pop(mem1);
		}
		st->cleanup();
	}
	{
		memstack_memory_t *mem1 = st->push(5);
		memstack_memory_t *mem2 = st->push(6);
		memstack_memory_t *mem3 = st->repush_last(mem2, 7000);
		(void)mem1, (void)mem3;
	}
	st->destroy();
	return 0;
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	return test();
}
