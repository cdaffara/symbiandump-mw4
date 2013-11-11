/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  PTP transport callback interface.
*
*/


#ifndef PTPCALLBACK_H
#define PTPCALLBACK_H

#include <e32std.h>
#include <ptptransport.h>

class MNPtpCallback
/** 
 * An interface for notification of receive completions.
 * Transport plugins use these methods to inform PTP when an event is received
 * or a data block has arrived through the transport interface.
 * @lib ptpstack.lib
 * @since S60 3.2
 */
	{
public:

	/**
	* Callback for informing PTP that an event has been received.
    * @since S60 3.2
    * @param aEventData, Container containing event info.
    * @return KErrNone, if successful, otherwise one of the other system-wide
    *         error codes.
	*/
	virtual TInt EventReceived( TNPtpContainer& aEventData  ) = 0;

    /**
	* Callback for informing PTP that a data block has been received.
	* @since S60 3.2
	* @param aContainer, Container containing operation info.
	*                    Response is returned in this parameter also.
	* @param aDataFile, Name of the file where incoming data resides.
	*                   Also if method returns data, the name of the data file
	*                   is written here. When operation or response deosn't
	*                   constain any data, this parameter should be empty.
	* @return KErrNone, if successful, otherwise one of the other system-wide
    *         error codes. KPtpErrDataMissing is returned if aData is empty
    *         though operation requires data. Transport layer then re-sends the
    *         operation with the data packed.
	*/
	virtual TInt DataReceived( TNPtpContainer& aContainer, TFileName& aFileName  ) = 0;
	
	virtual void ResponseReturned(TNPtpContainer& aContainer) = 0;
	};
	
	
#endif // PTPCALLBACK_H