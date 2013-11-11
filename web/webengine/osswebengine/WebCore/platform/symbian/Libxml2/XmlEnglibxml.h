/*
 * libxml.h: internal header only used during the compilation of libxml
 *
 * See COPYRIGHT for the status of this software
 *
 * Author: breese@users.sourceforge.net
 */

#ifndef __XML_LIBXML_H__
#define __XML_LIBXML_H__

#ifndef NO_LARGEFILE_SOURCE
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#endif

#include "Libxml2_xmlversion.h"

#ifndef WITH_TRIO
#include <stdio.h>
#else
/**
 * TRIO_REPLACE_STDIO:
 *
 * This macro is defined if teh trio string formatting functions are to
 * be used instead of the default stdio ones.
 */
#define TRIO_REPLACE_STDIO
#include "Libxml2_trio.h"
#endif

/*
 * internal error reporting routines, shared but not partof the API.
 */
#ifdef XMLENGINE_EXCLUDE_IOERRMEMORY
#  define __xmlIOErr(domain,code,extra)
#  define __xmlLoaderErr(arg1,arg2,arg3)
#else
void __xmlIOErr(int domain, int code, const char *extra);
void __xmlLoaderErr(void *ctx, const char *msg, const char *filename);
#endif

#endif /* ! __XML_LIBXML_H__ */

