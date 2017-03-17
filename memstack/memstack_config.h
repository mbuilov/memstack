#ifndef MEMSTACK_CONFIG_H_INCLUDED
#define MEMSTACK_CONFIG_H_INCLUDED

/*******************************************************************************
* Memory Stack allocation library
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/memstack
* Licensed under LGPL version 2.1 or any later version, see COPYING
*******************************************************************************/

/* memstack_config.h */

/* define external functions needed for memstack */

#include <stdlib.h>
#include <string.h>

#ifdef MEMSTACK_DEBUG
#include <stdio.h>
#endif

#ifdef WIN32
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#define MEMSTACK_MALLOC(sz)           malloc(sz)
#define MEMSTACK_REALLOC(mem, sz)     realloc(mem, sz)
#define MEMSTACK_FREE(mem)            free(mem)
#define MEMSTACK_MEMCPY(dst, src, sz) memcpy(dst, src, sz)

#ifdef MEMSTACK_DEBUG
#define MEMSTACK_LOG       fprintf
#define MEMSTACK_LOG_ARG   stdout,
#define MEMSTACK_ERROR     fprintf
#define MEMSTACK_ERROR_ARG stderr,
#endif

#endif /* MEMSTACK_CONFIG_H_INCLUDED */
