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
* Description:  
*
*/

#ifndef _PORTABLE_SNPRINTF_H_
#define _PORTABLE_SNPRINTF_H_

#ifdef HAVE_SNPRINTF
#include <stdio.h>
#else

#include <stdarg_e.h>
#define va_list __e32_va_list

#ifdef __cplusplus
extern "C"
{
#endif
    IMPORT int snprintf(char *, size_t, const char *, /*args*/ ...);
    IMPORT int vsnprintf(char *, size_t, const char *, va_list);    
#ifdef __cplusplus
}
#endif
#endif

#endif
