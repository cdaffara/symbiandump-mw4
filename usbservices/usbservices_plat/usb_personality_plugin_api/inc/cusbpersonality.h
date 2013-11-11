/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Basic class for personalitites
*
*/


#ifndef CUSBPERSONALITY_H
#define CUSBPERSONALITY_H

#include <bldvariant.hrh>
#include <e32base.h>
#include <usbman.h>
#include <usbuinotif.h>
#include <cusbpersonalitynotifier.h>

class RUsb;
class TUsbPersonalityParams;

/**
*  Abstract interface to personality handler objects.
*
*  @lib -
*  @since Series 60 3.0
*/
class CUsbPersonality : public CActive
    {
    public: // New functions
    
        /**
        * Destructor.
        */
        
        virtual ~CUsbPersonality();

        /**
        * DEPRICATED
        */
        IMPORT_C virtual void ShowUsbConnectionNote();
        
        virtual void ConfirmPersonalityUnload(TRequestStatus& aStatus) = 0;
                
        /**
        * Called by personality handler when personality start needs to be
        * prepared
        * @since Series 60 3.0
        */
        virtual void PreparePersonalityStart(TRequestStatus& aStatus) = 0;
        
        /**
        * Called by personality handler when personality start needs to be 
        * finished
        * @since Series 60 3.0
        */
        virtual void FinishPersonalityStart(TRequestStatus& aStatus) = 0; 

        /**
        * Called by personality handler when personality stop needs to be 
        * prepared
        * @since Series 60 3.0
        */
        virtual void PreparePersonalityStop(TRequestStatus& aStatus) = 0; 
                
        /**
        * Called by personality handler when personality stop needs to be 
        * finished
        * @since Series 60 3.0
        */
        virtual void FinishPersonalityStop(TRequestStatus& aStatus) = 0; 
                
        /**
        * State change notify
        * @since Series 60 3.0
        * @param aState state of the device
        */
        virtual void StateChangeNotify( TUsbDeviceState aState ) = 0;

protected:
        
        /**
        * Constructor
        * @since S60 3.2
        * @param aPersonalityParams Used to deliver information for personality
        */
        IMPORT_C CUsbPersonality(TUsbPersonalityParams& aPersonalityParams);
        

        /**
         * This class contains information what might be needed by the personality
         */
        TUsbPersonalityParams& iPersonalityParams;

        /**
         * Member variable used by connection notifier
         */
        TUSBConnectionNotifierParamsPckg iPckg;
        
        /**
         * Member variable used by connection notifier
         */
        TBuf8<1> iNoResult;
    };

#endif   // CUSBPERSONALITY_H

// End of File
