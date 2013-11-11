/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UsbWatcher server API
*
*/


#ifndef RUSBWATCHER_H
#define RUSBWATCHER_H

#include <e32std.h>
#include <usb.h>

/**
 *  RUsbWatcher class
 *
 *  This class offers access to UsbWatcher server to make personality
 *  related operations.
 *
 *  @lib usbwatcher.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(RUsbWatcher) : public RSessionBase
    {
public:
	IMPORT_C RUsbWatcher();

	IMPORT_C ~RUsbWatcher();

    /**
     * Return version of the server.
     *
     * @since S60 3.2
     * @return Version of the server
     */
	IMPORT_C TVersion Version() const;

    /**
     * Connect to the server
     *
     * @since S60 3.2
     * @return KErrNone if successful, otherwise one of the other system-wide error codes.
     */
	IMPORT_C TInt Connect();
    
    /**
     * Set and select personality
     * Personality is saved to central repository and if there is an active 
     * USB connection also current personality is changed to new one.
     *
     * If the aForce parameter is set ETrue, the Ask on connection mode query
     * is not shown at the following cable connections until 
     * - the session is closed 
     * - or aForce is set EFalse in subsequent SetPersonality
     * - or CancelSetPersonality is called for outstanding SetPersonality
     * - or SetPreviousPersonality or SetPreviousPersonalitySync is called
     * - or SetPreviousPersonalityOnDisconnect is called.
     * The Ask on connection is suppressed until all the sessions using aForce 
     * have been closed or have been resetted the suppression with one of the 
     * function calls listed above. Do not leave session open without resetting 
     * aForce, if Ask on connection needs to work normally.
     *
     * Note that if KErrDiskFull is returned in aStatus, while the cable is 
     * connected, the personality was loaded, but the new personality was not 
     * stored to Central Repository.
     *
     * @since S60 3.2
     * @param aStatus The completion status of the request.
     * @param aId Identifies personality to set.
     * @param aForce If this parameter has value ETrue, USB mode is not asked from the user.
     * @param aNonBlocking If ETrue, no personality switch blocking queries are shown.
     */
    IMPORT_C void SetPersonality(TRequestStatus& aStatus, TInt aId, TBool aForce = EFalse, 
        TBool aNonBlocking = EFalse);
    
    /**
     * Cancel pending set personality request.
     *
     * @since S60 3.2
     */    
    IMPORT_C void CancelSetPersonality();

    /**
     * Set to previous personality. Central repository key is updated with the previous one and if
     * USB is connected current personality is replaced with previous one.
     * 
     * @since S60 3.2
     * @param aStatus The completion status of the request.
     */
    IMPORT_C void SetPreviousPersonality(TRequestStatus& aStatus);

    /**
     * This service is same as previous one. Except that this one is comleted before it is ready.
     * 
     * @since S60 3.2
     */
    IMPORT_C void SetPreviousPersonality();

    /**
     * This service cancels pending SetPreviousPersonality() request.
     * 
     * @since S60 3.2
     */
    IMPORT_C void CancelSetPreviousPersonality();
    
    /**
     * This service will set the previous personality on cable disconnect. If cable is disconnected
     * already when this service is used, nothing will happen.
     * 
     * @since S60 3.2
     */    
    IMPORT_C void SetPreviousPersonalityOnDisconnect();
    };
    
#endif //RUSBWATCHER_H
