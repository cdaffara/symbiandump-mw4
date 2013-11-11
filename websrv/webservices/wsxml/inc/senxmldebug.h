/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:           Central place for debug-type macros
*
*/











#ifndef SEN_XML_DEBUG_H
#define SEN_XML_DEBUG_H

//  INCLUDES
#include "sendebug.h"

#ifdef _SENDEBUG
#define SENDEBUG_XML_FRAGMENT_STATE(i) \
    switch(i) \
        { \
        case -1: \
            SENDEBUG((_L("   Fragment state (%d): KStateNotSet"), i)); \
            break; \
        case 0: \
            SENDEBUG((_L("   Fragment state (%d): KStateIgnore"), i)); \
            break; \
        case 1: \
            SENDEBUG((_L("   Fragment state (%d): KStateSave"), i)); \
            break; \
        case 2: \
            SENDEBUG((_L("   Fragment state (%d): KStateResume"), i)); \
            break; \
        case 4: \
            SENDEBUG((_L("   Fragment state (%d): KStateParsingFramework"), i)); \
            break; \
        case 5: \
            SENDEBUG((_L("   Fragment state (%d): KStateParsingSoapFault"), i)); \
            break; \
        case 6: \
            SENDEBUG((_L("   Fragment state (%d): KStateParsingServiceDescription"), i)); \
            break; \
        case 11: \
            SENDEBUG((_L("   Fragment state (%d): KStateParsingResourceOffering"), i)); \
            break; \
        case 12: \
            SENDEBUG((_L("   Fragment state (%d): KStateParsingCredentials"), i)); \
            break; \
        case 13: \
            SENDEBUG((_L("   Fragment state (%d): KStateParsingService"), i)); \
            break; \
        case 14: \
            SENDEBUG((_L("   Fragment state (%d): KStateParsingPwTransforms"), i)); \
            break; \
        case 20: \
            SENDEBUG((_L("   Fragment state (%d): KStateParsingSoapHeader"), i)); \
            break; \
        case 40: \
            SENDEBUG((_L("   Fragment state (%d): KStateParsingSoapBody"), i)); \
            break; \
        case 122: \
            SENDEBUG((_L("   Fragment state (%d): KStateParsingSingleCredential"), i)); \
            break; \
        case 1222: \
            SENDEBUG((_L("   Fragment state (%d): KStateParsingProviderPolicy"), i)); \
            break; \
        default: \
            SENDEBUG((_L("   Fragment state (%d): UNKNOWN STATE(!)"), i)); \
            break; \
        }
#else
#define SENDEBUG_XML_FRAGMENT_STATE(i)
#endif // _SENDEBUG

#endif // SEN_XML_DEBUG_H

// End of File