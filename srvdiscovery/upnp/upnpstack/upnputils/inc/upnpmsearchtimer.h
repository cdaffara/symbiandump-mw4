/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares virtual MUpnpNotifyTimerObserver callback class
*                and the implemented active object class CUpnpNotifyTimer
*
*/


#ifndef C_CUPNPMSEARCHTIMER_H
#define C_CUPNPMSEARCHTIMER_H

// INCLUDES
#include <e32base.h>
#include "upnpnotifytimer.h"
    
// CLASS DECLARATION
/**
* Class with m-search timer
*
*  @since Series60 3.2
*/
class CUpnpMSearchTimer : public CUpnpNotifyTimer
    {
    private:

    /**
     * Constructor
     */
    IMPORT_C CUpnpMSearchTimer( MUpnpNotifyTimerObserver* aObserver);

    public: 
    IMPORT_C static CUpnpMSearchTimer* NewL(MUpnpNotifyTimerObserver* aObserver, 
                                const TDesC8& aTarget,
                                const TDesC8& aMX);
                                
    IMPORT_C static CUpnpMSearchTimer* NewLC(MUpnpNotifyTimerObserver* aObserver, 
                                const TDesC8& aTarget,
                                const TDesC8& aMX);
        
    void ConstructL(const TDesC8& aTarget,
                                const TDesC8& aMX);
        
    /**
     * Destructor.
     */
    ~CUpnpMSearchTimer();
    
    IMPORT_C TInt GetNextValue();
    IMPORT_C TPtrC8 GetTarget();
    

    private: 
    // search target
    HBufC8* iTarget;
    // mx configuration
    HBufC8* iMXString;
    // mx processor
    TLex8 iMX;
    
    };


#endif // C_CUPNPMSEARCHTIMER_H

// End Of File