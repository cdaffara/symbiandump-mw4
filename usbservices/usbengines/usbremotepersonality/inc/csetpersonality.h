/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles set personality request
*
*/


#ifndef CSETPERSONALITY_H
#define CSETPERSONALITY_H

#include <e32base.h>

class RUsbWatcher;
class CRemotePersonalityHandler; // call back
 
/**
 * Set Personality Request handler
 *
 *  @lib usbremotepersonality.lib
 *  @since S60 v.5.0 
 */
 
class CSetPersonality : public CActive
    {

public:
    
    /**
     * Two-phased constructor.
     *
     * @since S60 v.5.0
     * @return Constructed instance
     */ 
    static CSetPersonality* NewL(CRemotePersonalityHandler& aCallBack);
    
    /**
    * Destructor.
    *
    * @since S60 v.5.0
    */
    virtual ~CSetPersonality();
    
    /**
     * Handles SET_PERSONALITY request
     *
     * @since S60 v.5.0
     */
    void SetPersonality(TUint aPersonalityId);
    
    /**
     * Sets link to UsbWatcher
     *
     * @since S60 v.5.0
     */
    void SetUsbWatcher(RUsbWatcher* aUsbWatcher);
    
private:
    
    /**
     * Default constructor.
     *
     * @since S60 v.5.0
     */ 
    CSetPersonality(CRemotePersonalityHandler& aCallBack);

    /**
     * Two-phased constructor.
     *
     * @since S60 v.5.0
     */ 
    void ConstructL();
      
    // from CActive
    
    /**
     * From CActive.
     *
     */
    void RunL();
    
    /**
     * From CActive.
     *
     */
    void DoCancel();
    
    /**
     * From CActive.
     *
     */ 
     TInt RunError( TInt /*aError*/ );
        
private: // data
    
    /**
     * Callback class 
     * Not owns.  
     */
    CRemotePersonalityHandler& iCallBack;
    
    /**
     * USB watcher
     * Not owns.  
     */
    RUsbWatcher* iUsbWatcher;
    
     };

#endif // CSETPERSONALITY_H
