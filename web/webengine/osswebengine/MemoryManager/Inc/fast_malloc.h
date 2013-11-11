/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
*  Description : Header file for fast_malloc.cpp
*
*
*/

// NOTE:
// This header file is used by fast_malloc.cpp, which is a public domain file.
// fast_malloc.cpp is a version (aka dlmalloc) of malloc/free/realloc written
// by Doug Lea and released to the public domain, as explained at
// http://creativecommons.org/licenses/publicdomain.  Send questions,
// comments, complaints, performance data, etc to dl@cs.oswego.edu
#ifndef FAST_MALLOC_H
#define FAST_MALLOC_H

#include <stdlib.h>

void *fast_malloc(size_t n);
void *fast_calloc(size_t n_elements, size_t element_size);
void *fast_realloc(void* p, size_t n);
void fast_free(void* p);
int  free_memory( size_t& pool, size_t& heap, size_t& sys );
bool owned_by_pool( void* p );
unsigned int fast_malloc_usable_size(void*);
bool fast_pre_check( size_t, size_t );
void fast_post_check();
void close_mem_pool();
void fast_set_rescue_buffer_size(int size);
unsigned int fast_malloc_size(void* p);
void alloc_rescue_buffer();

#endif /* FAST_MALLOC_H */
