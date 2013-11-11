/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of iap
*
*/


#ifndef HSSIAPHANDLER_H
#define HSSIAPHANDLER_H

// INCLUDES
#include <e32base.h>
#include "hssiapsettings.h"
#include "hssiapsettingshandler.h"

// CLASS DECLARATION
/**
* @brief IAP settings handler
*/
class CHssIapHandler : public CBase
    {
    public:  // Methods

       // Constructors and destructor
        
        /**
        * Static constructor.
        * @return Pointer to the constructed object.
        */
        static CHssIapHandler* NewL( );
        
        /**
        * Destructor.
        */
        virtual ~CHssIapHandler();
    
   	    /**
        * Starts IAP  changes
        * @param aIapID, IAP id
        * @param aSettings, IAP settings struct
        * @return KErrNone if successful, otherwise one of system wide errorcodes.
        */
        TInt ChangeSettingsL( const TUint aIapID, const THssIapSettings& aSettings );
                
        /**
        * Gets Network Id
        * @param aIapId, IAP id
        * @param aNetId, Network id
        * @return None
        */
        void GetNetworkIdL( const TUint aIapId, TUint32& aNetId );
        
        /**
        * Gets all Iaps of client
        * @param aUId, UID of client
        * @param aIapIdArray, Array for client's Iaps
        * @return None
        */
        void GetClientIapsL( const TUid aUid, RArray<TUint>& aIapIdArray );
                
    protected:
    
        /**
        * C++ default constructor.
        */
        CHssIapHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();	
    
    private:    
    
        /**
        * Writes WEP keys from aSettings to current WLANServiceTable record
        * @param aSettings, IAP settings struct
        * @return KErrNone if successful, otherwice one of system wide errorcodes.
        */
        TInt WriteWepKeysL( const THssIapSettings& aSettings );
        
        /**
        * Copies wepkey string from wepkey structs TUint8 
        * buffer to a WLANServiceTable (CommsDat)
        * @param aInputKey, WepKey
        * @param aFormat, format of WepKey ( ascii/hexadecimal)
        * @param aPackedKey, packed WepKey
        */
        void PackWepKeyCommsDatL( const SHssWep& aInputKey, 
                                  const  EHssWEPKeyFormat&  aFormat, 
                                    CMDBField<TDesC8>* aPackedKey );
 
        /**
        * Converts ascii to hecxadecimal 
        * @param aSource, ascii 
        * @param aDest, hexa 
        */
        void ConvertAsciiToHex( const TDes8& aSource, 
                                                HBufC8*& aDest );   
                                                                         
        /**
        * Removes brackets from UID
        * @param aUid, UID of the client
        */
        void ModifyClientUid( TDes& aUid );
       
    private: // Data
        
        /**
        * CommsDat WLAN record instance
        * Own
        */
        CMDBGenericRecord* iWLANRecord;

    };


#endif
