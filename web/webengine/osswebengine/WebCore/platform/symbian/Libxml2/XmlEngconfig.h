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
* Description:  Definitions and declarations that affect XML Engine Core
*
*/

#ifndef XMLENGINE_CONFIG_H_INCLUDED
#define XMLENGINE_CONFIG_H_INCLUDED

/**
 * This file contains definitions/declarations  that affect XML Engine Core 
 * 
 */
 
#include "XmlEngxeconfig.h"

// TODO: Disable debug options in RELEASE mode
#define XMLENGINE_DISABLE_STDOUT

#define XMLENGINE_EXCLUDE_UNUSED
#define XMLENGINE_EXCLUDE_FILE_FUNC
#define XMLENGINE_EXCLUDE_OBSOLETE_CODE
#define XMLENGINE_EXCLUDE_IOERRMEMORY
#define XMLENGINE_EXCLUDE_NONSYMBIAN

#ifdef _DEBUG
//  Debug-builds only
#	define XMLENGINE_MEM_DEBUG
//#define XMENGINE_DEBUG_XSLT
#else
//  Release-builds only
#	define XMLENGINE_EXCLUDE_EMBED_MSG 
#endif

#ifndef HAVE_CTYPE_H
#define HAVE_CTYPE_H
#endif

#ifndef HAVE_STDLIB_H
#define HAVE_STDLIB_H
#endif

#ifndef HAVE_MATH_H
#define HAVE_MATH_H
#endif

#ifndef HAVE_UNISTD_H
#define HAVE_UNISTD_H
#endif

#ifndef HAVE_STAT
//#define HAVE_STAT // caused problems in xmlFileOpen_real
#define HAVE_SYS_STAT_H
#endif

//----------------------------------------------------
#ifdef  XMLENGINE_XPATH
# ifdef XMLENGINE_XFORMS_EXTENSIONS
#   ifndef XMLENGINE_XPATH_EXTENSIONS
#       define XMLENGINE_XPATH_EXTENSIONS
#   endif
# endif
# ifndef XMLENGINE_DOM
#    define XMLENGINE_DOM
# endif
# define XMLENGINE_XPATH_FUNC_HASH_OPTIMIZED
# if !defined(XMLENGINE_XPATH_FUNC_HASH_OPTIMIZED) && defined(XMLENGINE_XPATH_EXTENSIONS)
       // This should be defined if XPath Extension API is present
#      define XMLENGINE_XPATH_FUNC_HASH_OPTIMIZED
# endif
#endif /* XMLENGINE_XPATH */
//----------------------------------------------------

// TODO: Move these declarations into separatre header (xedefs.h)
#if defined (XMLENGINE_USE_DOMSTRINGARGUMENT) 
typedef struct _DOMStringConverterData DOMStringConverterData;
//typedef DOMStringConverterData* DOMStringConverterDataPtr;
struct _DOMStringConverterData
{
    unsigned int lastSaved;         // index: 0-3
    unsigned int maxReusableSize;   // cut-off limit for buffers (0 = unrestricted)
    char*        cleanupItem[4];    // temporary DOMString used in API method calls
    unsigned int itemSize[4];       // size of memory allocated (an item may be reused) 
};
#endif /* XMLENGINE_USE_DOMSTRINGARGUMENT */

#endif /* XMLENGINE_CONFIG_H_INCLUDED */
