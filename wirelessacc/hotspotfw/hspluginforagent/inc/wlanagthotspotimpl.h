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
* Description:   Agent-hotspot interaction plugin
*
*/



#ifndef WLANAGTHOTSPOTIMPL_H
#define WLANAGTHOTSPOTIMPL_H

#include <e32base.h>
#include <wlanagthotspotclient.h>
#include "hssinterface.h"

/**
* Plugin used by WLAN Agent in order to interact with Hot Spot frame work which is middleware component
*
* @lib wlanagthotspotimpl.dll
* @since Series 60 3.0
*/
class CWlanAgtHotSpotImpl :
    public CWlanAgtHotSpotClient
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aCallback specifies the callback interface
        */
        static CWlanAgtHotSpotImpl* NewL( );
        
        /**
        * Destructor.
        */
        virtual ~CWlanAgtHotSpotImpl();

    public: // Functions from base classes

        /* From CWlanAgtHotSpotClient */

	void HotSpotStart( const TUint aIapId, TRequestStatus& aStatus );
	
	void HotSpotStartAgain( const TUint aIapId, TRequestStatus& aStatus );

	void HotSpotCloseConnection( const TUint aIapId, TRequestStatus& aStatus );

	void HotSpotCancel( const TUint aIapId );
	
    private:

        /**
        * C++ default constructor.
        */
        CWlanAgtHotSpotImpl( );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Requests a notification from System Agent. 
        */
        void IssueRequest();


    private:    // Data

	RHssInterface   iHSServer;

    };

#endif // WLANAGTHOTSPOTIMPL_H
