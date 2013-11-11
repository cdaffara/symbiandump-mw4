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
* Description:  Contain the normalized tokens for the folder attributes.
*
*/


#ifndef FOLDER_ATTRIBUTES_H
#define FOLDER_ATTRIBUTES_H

#warning The Feeds Engine API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES
    
enum TPackedFolderAttributes
    {
    EFolderAttributeUnused     = 0,
    EFolderAttributeEnd        = 1,
    EFolderAttributeTitle      = 2,
    EFolderAttributeUrl        = 3,
    EFolderAttributeEntryId    = 4,
    EFolderAttributeFeedId     = 5,
    EFolderAttributeTimestamp  = 6,
    EFolderAttributeUnreadCount  = 7,
    EFolderAttributeMiniItemTitle = 8,
    EFolderAttributeMiniItemId = 9,
    EFolderAttributeStatus = 10,
    EFolderAttributeFreq       = 11
    };
    
// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


#endif      // FOLDER_ATTRIBUTES_H
            
// End of File
