/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Internal address structure
*
*/


#ifndef C_INTERNALADDRESS_H
#define C_INTERNALADDRESS_H

#include <in_sock.h>

/**
 *  Internal address structure.
 *  This class contains internal address in secure network and all DNS
 *  addresses related to that secure network.
 *
 *  @lib ikeutils.lib
 */
class CInternalAddress : public CArrayPtrFlat<TInetAddr>
    {
	public:
	    /**
	    * Constructor.
	    * @param aGranularity Granularity
	    */
	    IMPORT_C CInternalAddress( TInt aGranularity );
	    
	    /**
	    * Destructor.
	    */
        ~CInternalAddress();
        
        /**
         * Two-phased constructor.
         * @param aInternalAddress Internal address
         */
        IMPORT_C static CInternalAddress* NewL( const CInternalAddress& aInternalAddress );
        
        /**
         * Copies internal address.
         * @param aInternalAddress Internal address
         */
        IMPORT_C void CopyL( const CInternalAddress& aData );

	public: // data 
		
	    /**
	     * Internal address.
	     * Own.
	     */
	    TInetAddr     iClientIntAddr;
    };

#endif // C_INTERNALADDRESS_H

