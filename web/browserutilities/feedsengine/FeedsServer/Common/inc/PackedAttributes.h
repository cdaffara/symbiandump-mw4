/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contain the normalized tokens for common attributes.
*
*/


#ifndef PACKED_ATTRIBUTES_H
#define PACKED_ATTRIBUTES_H


// INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES
enum TPackedTokens
    {
    EPackedTokenAttribute = 0  // Must be zero.
    };

enum TPackedFolderTokens
    {
    EFolderTokenFolderBegin  = 1, // Must not start at zero.
    EFolderTokenFolderEnd = 2,
    EFolderTokenFeed = 3,
    EFolderTokenItemBegin = 4,
    EFolderTokenItemEnd = 5
    };
        
enum TPackedFeedTokens
    {
    EFeedTokenFeedBegin         = 1, // Must not start at zero.
    EFeedTokenFeedEnd           = 2,
    EFeedTokenItemBegin         = 3,
    EFeedTokenItemEnd           = 4,
    EFeedTokenEnclosureBegin    = 5,
    EFeedTokenEnclosureEnd      = 6,
    EFeedTokenOtherLink         = 7,
    EFeedTokenOtherTitle        = 8,
    EFeedTokenOtherDescription  = 9
    };    
    
// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


#endif      // PACKED_ATTRIBUTES_H
            
// End of File
