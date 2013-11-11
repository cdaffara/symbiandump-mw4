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
* Description:  Defines the Feeds public views.
*
*/


#ifndef FEEDS_VIEW_H
#define FEEDS_VIEW_H


/// The app UID
enum TFeedsAppUid
    {
    EFeedsAppUid = 0x10207303
    };


/// Feeds application view ids.
enum TFeedsViewNumber
    {
    EFeedsFolderViewUid = 1,
    EFeedsTopicViewUid = 2,
    EFeedsFeedViewUid = 3,
    EFeedsSettingsViewUid = 4,
    EFeedsTestHarnessViewUid = 5
    };


/// Feeds Views active commands
enum TFeedsViewCommands
    {
    EFeedsViewCommandSubscribeToUid = 1
    };
        
#endif      // FEEDS_VIEW_H
            
// End of File
