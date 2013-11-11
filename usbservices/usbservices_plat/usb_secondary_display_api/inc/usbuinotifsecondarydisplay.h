/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains dialog index for cover UI.
*
*/


#ifndef USBUINOTIFSECONDARYDISPLAY_H
#define USBUINOTIFSECONDARYDISPLAY_H

// Category
const TUid KUSBUINotifCategory = { 0x102068DC };

/**
* Enumerates types.
* Do not use value 0 as a dialog ID, since it is reserved for special cases 
* in AknGlobalUI. Note that the notes start from 1, queries from 0x100, and
* list queries from 0x200. 
*/
enum TUSBUINotifDialogIndex
    {
    /**
     * Not a valid dialog ID.
     * This value is not associated with any dialog to be shown. 
     */
    EUSBCoverInvalidDialogId = -1,
    
    // ===== notes =====
    
    /**
     * Connected in %U mode. 
     * %U is for example "PC Suite" or "Mass storage".
     */
    EUSBCoverConnectionNote = 0x1,
    
    // ===== queries =====

#ifndef RD_MULTIPLE_DRIVE    
    /** Memory card needs to be unlocked: OK */
    EUSBCoverMemoryCardUnlocked = 0x100, 
#endif //RD_MULTIPLE_DRIVE

    /** Storage media not accessible: OK */
    EUSBCoverStorageMediaFailure = 0x101, 
    
#ifndef RD_MULTIPLE_DRIVE    
    /** 
     * Data loss may have occurred. In future, stop file transfers 
     * before removing the cable: OK
     */
    EUSBCoverPossibleDataLossCable = 0x102,
    
    /** In future, stop file transfers before removing the memory card: OK */
    EUSBCoverPossibleDataLossMMC = 0x103,
#endif //RD_MULTIPLE_DRIVE
    
    /** Data loss might occur. Change mode anyway? OK, Cancel */
    EUSBCoverChangeFromMassStorage = 0x104,

    EUSBCoverNoMemoryCard = 0x105,
               
    // ===== list queries =====
    
    /** USB mode: <list> */
    EUSBCoverCableConnected = 0x200
    };
    
#endif      // USBUINOTIFSECONDARYDISPLAY_H

// End of File
